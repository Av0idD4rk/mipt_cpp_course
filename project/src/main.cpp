#include <charconv>
#include <cstdio>
#include <exception>
#include <memory>
#include <print>
#include <string>
#include <string_view>

#include "agent.h"
#include "file_source.h"
#include "os_source.h"

namespace {
struct Options {
    std::string path;
    bool quiet = false;
    bool file = false;
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
        } else if (arg == "--file") {
            if (i + 1 >= argc || !options.path.empty())
                return false;

            const std::string_view path = argv[++i];
            if (path.empty() || path.front() == '-')
                return false;

            options.path = path;
            options.file = true;

        } else if (!arg.empty() && arg.front() != '-' &&
                   options.path.empty()) {
            options.path = arg;
        } else {
            return false;
        }
    }

    if (options.path.empty())
        return false;

    *out = std::move(options);
    return true;
}

int main(const int argc, char** argv) {
    Options options;
    if (!ParseOptions(argc, argv, &options)) {
        std::print(
            stderr,
            "Использование:\n"
            "  nano-edr [опции] <журнал.log>          — события через os.h\n"
            "  nano-edr [опции] --file <журнал.log>   — прямое чтение файла\n"
            "\n"
            "Опции:\n"
            "  --quiet           выводить только детекты\n"
            "  --window-size N   хранить последние N событий\n"
            "                    по умолчанию 64; 0 — без ограничения\n"
            "\n"
            "Пример:\n"
            "  nano-edr --quiet --file scenarios/phishing_macro.log\n");
        return 2;
    }
    try {
        nano_edr::Agent agent(options.windowSize, options.quiet);

        std::unique_ptr<nano_edr::IEventSource> source;
        if (options.file) {
            source = std::make_unique<nano_edr::FileSource>(options.path);
        } else {
            source = std::make_unique<nano_edr::OsSource>(options.path);
        }
        source->Run(&agent);

        agent.PrintSummary();
    } catch (const std::exception& error) {
        std::print(stderr, "Ошибка: {}\n", error.what());
        return 1;
    }

    return 0;
}
