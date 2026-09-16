#include <charconv>
#include <cstdio>
#include <fstream>
#include <map>
#include <print>
#include <string>
#include <string_view>
#include <vector>

#include "event_list.h"
#include "parse.h"

static const std::vector<std::string> SIGNATURES = {
    "wscript.exe",
    ".locked",
    "certutil.exe",
    "\\Startup\\"};

namespace {
struct Options {
    std::string logPath;
    bool quiet = false;
    std::size_t windowSize = 64;
};
}  // namespace

static bool ParseOptions(const int argc, char** argv, Options* out) {
    if (!out || argc < 2)
        return false;

    Options options;
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i];

        if (arg == "--quiet") {
            options.quiet = true;
        } else if (arg == "--window-size") {
            if (i + 1 >= argc)
                return false;

            const std::string_view value = argv[++i];

            const auto [end, error] = std::from_chars(
                value.data(),
                value.data() + value.size(),
                options.windowSize);

            if (error != std::errc{} ||
                end != value.data() + value.size()) {
                return false;
            }
        } else if (!arg.empty() && arg.front() != '-' &&
                   options.logPath.empty()) {
            options.logPath = arg;
        } else {
            return false;
        }
    }

    if (options.logPath.empty())
        return false;

    *out = std::move(options);
    return true;
}

static void PrintContext(const nano_edr::EventNode* first) {
    if (!first)
        return;

    if (first->next) {
        std::print(
            "[CTX] -2: ts={} type={} pid={}\n",
            first->event.ts,
            first->event.type,
            first->event.pid);
        first = first->next;
    }
    std::print(
        "[CTX] -1: ts={} type={} pid={}\n",
        first->event.ts,
        first->event.type,
        first->event.pid);
}
int main(int argc, char** argv) {
    Options options;
    if (!ParseOptions(argc, argv, &options)) {
        std::print(
            stderr,
            "использование: nano-edr [--quiet] [--window-size N] "
            "<журнал.log>\n");
        return 2;
    }

    std::ifstream log(options.logPath);
    if (!log) {
        std::print(
            stderr,
            "не удалось открыть журнал: {}\n",
            options.logPath);
        return 2;
    }

    nano_edr::EventList window;
    window.capacity = options.windowSize;
    const nano_edr::EventNode* context = nullptr;

    std::size_t lines = 0;
    std::size_t comments = 0;
    std::size_t events = 0;
    std::map<std::string, std::size_t> events_by_type;

    std::string line;

    while (std::getline(log, line)) {
        ++lines;

        if (nano_edr::IsBlankOrComment(&line) && !options.quiet) {
            ++comments;
            continue;
        }

        nano_edr::Event event;
        if (!nano_edr::ParseEventLine(&line, &event))
            continue;

        if (!options.quiet) {
            ++events;
            ++events_by_type[event.type];
        }

        bool detected = false;
        for (const auto& signature : SIGNATURES) {
            if (line.find(signature) != std::string::npos) {
                detected = true;
                std::print(
                    "[DETECT] строка {}, признак {}: {}\n",
                    lines,
                    signature,
                    line);
            }
        }
        if (detected && !options.quiet)
            PrintContext(context);

        context = window.capacity == 1 ? nullptr : window.tail;
        nano_edr::ListPushBack(&window, &event);
        if (!context)
            context = window.head;
    }
    if (!options.quiet) {
        std::print("--------------------------------------------\n");
        std::print(
            "Строк: {}. Из них комментариев: {}\n",
            lines,
            comments);
        std::print("Всего событий: {}\n", events);

        for (const auto& [type, count] : events_by_type)
            std::print("{}: {}\n", type, count);
    }

    return 0;
}
