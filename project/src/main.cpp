#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <print>
#include <string>
#include <string_view>
#include <unordered_map>

static constexpr std::array<std::string_view, 4> SIGNATURES = {
    "wscript.exe",
    ".locked",
    "certutil.exe",
    "\\startup\\"
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::print(stderr, "использование: nano-edr <журнал.log> [--quiet]\n");
        return 2;
    }

    bool quiet = false;

    for (int i = 2; i < argc; ++i) {
        if (std::string_view(argv[i]) == "--quiet") {
            quiet = true;
        } else {
            std::print(stderr, "неизвестный аргумент: {}\n", argv[i]);
            return 2;
        }
    }

    std::ifstream log(argv[1]);

    if (!log) {
        std::print(stderr, "не удалось открыть журнал: {}\n", argv[1]);
        return 2;
    }

    std::size_t lines = 0;
    std::size_t comments = 0;
    std::size_t events = 0;

    std::unordered_map<std::string, std::size_t> events_by_type;

    std::string line;

    while (std::getline(log, line)) {
        ++lines;

        if (line.empty()) {
            continue;
        }

        const auto first_char = line.find_first_not_of(" \t");

        if (first_char == std::string::npos) {
            continue;
        }

        if (line[first_char] == '#' || line[first_char] == ';') {
            ++comments;
            continue;
        }

        if (!line.starts_with("ts=")) {
            continue;
        }

        const auto type_pos = line.find(" type=");

        if (type_pos == std::string::npos) {
            continue;
        }

        const auto type_begin = type_pos + 6;
        const auto type_end = line.find_first_of(" \t", type_begin);

        const std::string type =
            line.substr(type_begin, type_end - type_begin);

        if (type.empty()) {
            continue;
        }

        ++events;
        ++events_by_type[type];

        std::string lower_line = line;

        std::transform(
            lower_line.begin(),
            lower_line.end(),
            lower_line.begin(),
            [](unsigned char ch) {
                return static_cast<char>(std::tolower(ch));
            }
        );

        for (const auto signature : SIGNATURES) {
            if (lower_line.find(signature) != std::string::npos) {
                std::print(
                    "[DETECT] строка {}, признак {}: {}\n",
                    lines,
                    signature,
                    line
                );
            }
        }
    }

    if (!quiet) {
        std::print("--------------------------------------------\n");
        std::print(
            "Строк: {}. Из них комментариев: {}\n",
            lines,
            comments
        );
        std::print("Всего событий: {}\n", events);

        for (const auto& [type, count] : events_by_type) {
            std::print("{}: {}\n", type, count);
        }
    }

    return 0;
}