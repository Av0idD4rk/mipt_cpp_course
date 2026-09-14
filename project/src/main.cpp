#include <cstdio>
#include <fstream>
#include <map>
#include <print>
#include <string>
#include <vector>

#include "event_list.h"
#include "parse.h"

static const std::vector<std::string> SIGNATURES = {
    "wscript.exe",
    ".locked",
    "certutil.exe",
    "\\Startup\\"
};

namespace {
    struct Options {
        std::string logPath;
        bool quiet = false;
        std::size_t windowSize = 64;
    };
}

static bool ParseOptions(const int argc, char **argv, Options *out) {
    if (!out || argc < 2)
        return false;

    Options options;
    options.logPath = argv[1];

    for (int i = 2; i < argc; ++i) {
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
                options.windowSize
            );

            if (error != std::errc{} ||
                end != value.data() + value.size()) {
                return false;
            }
        } else {
            return false;
        }
    }

    *out = std::move(options);
    return true;
}

static void PrintContext(const nano_edr::EventList *list) {
    if (!list)
        return;

    const nano_edr::EventNode *prev = nullptr;
    const nano_edr::EventNode *last = nullptr;

    for (const auto *node = list->head; node; node = node->next) {
        prev = last;
        last = node;
    }

    if (prev) {
        std::print(
            "[CTX] -2: ts={} type={} pid={}\n",
            prev->event.ts,
            prev->event.type,
            prev->event.pid
        );
    }
    if (last) {
        std::print(
            "[CTX] -1: ts={} type={} pid={}\n",
            last->event.ts,
            last->event.type,
            last->event.pid
        );
    }
}
int main(int argc, char** argv)
{
    Options options;
    if (!ParseOptions(argc, argv, &options)) {
        std::print(
            stderr,
            "использование: nano-edr <журнал.log> "
            "[--quiet] [--window-size N]\n"
        );
        return 2;
    }

    std::ifstream log(options.logPath);
    if (!log) {
        std::print(
            stderr,
            "не удалось открыть журнал: {}\n",
            options.logPath
        );
        return 2;
    }

    nano_edr::EventList window;
    window.capacity = options.windowSize;

    std::size_t lines = 0;
    std::size_t comments = 0;
    std::size_t events = 0;
    std::map<std::string, std::size_t> events_by_type;

    std::string line;

    while (std::getline(log, line)) {
        ++lines;

        if (nano_edr::IsBlankOrComment(&line)) {
            const auto pos = line.find_first_not_of(" \t\r\n\v\f");
            if (pos != std::string::npos &&
                (line[pos] == '#' || line[pos] == ';')) {
                ++comments;
            }

            continue;
        }

        nano_edr::Event event;
        if (!nano_edr::ParseEventLine(&line, &event))
            continue;

        ++events;
        ++events_by_type[event.type];

        for (const auto& signature : SIGNATURES) {
            if (line.find(signature) != std::string::npos) {
                std::print(
                    "[DETECT] строка {}, признак {}: {}\n",
                    lines,
                    signature,
                    line
                );
                if (!options.quiet)
                    PrintContext(&window);
            }
        }

        nano_edr::ListPushBack(&window, &event);
    }

    if (log.bad() || (log.fail() && !log.eof())) {
        std::print(
            stderr,
            "ошибка чтения журнала: {}\n",
            options.logPath
        );
        return 2;
    }

    if (!options.quiet) {
        std::print("--------------------------------------------\n");
        std::print(
            "Строк: {}. Из них комментариев: {}\n",
            lines,
            comments
        );
        std::print("Всего событий: {}\n", events);

        for (const auto& [type, count] : events_by_type)
            std::print("{}: {}\n", type, count);
    }

    return 0;
}