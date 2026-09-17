#include "parse.h"

#include <string_view>

namespace nano_edr {
bool IsBlankOrComment(const std::string* line) {
    if (!line)
        return true;
    for (const char c : *line) {
        if (!std::isspace(static_cast<unsigned char>(c)))
            return c == '#' || c == ';';
    }
    return true;
}

bool ParseEventLine(const std::string* line, Event* out) {
    if (!out)
        return false;

    *out = Event{};

    if (IsBlankOrComment(line))
        return false;

    const std::string_view s = *line;
    std::size_t i = 0;
    bool hasTs = false;
    bool hasType = false;
    bool hasPid = false;

    const auto isSpace = [](const char c) {
        return std::isspace(static_cast<unsigned char>(c)) != 0;
    };

    while (i < s.size()) {
        while (i < s.size() && isSpace(s[i]))
            ++i;

        if (i == s.size())
            break;

        const auto keyBegin = i;
        while (i < s.size() && s[i] != '=' && !isSpace(s[i])) {
            if (s[i] == '"')
                return false;
            ++i;
        }

        if (i == keyBegin || i == s.size() || s[i] != '=')
            return false;

        const auto key = s.substr(keyBegin, i - keyBegin);
        ++i;

        std::size_t valueBegin = i;
        std::size_t valueSize = 0;

        if (i < s.size() && s[i] == '"') {
            valueBegin = ++i;
            const auto closingQuote = s.find('"', i);

            if (closingQuote == std::string_view::npos)
                return false;

            valueSize = closingQuote - valueBegin;
            i = closingQuote + 1;

            if (i < s.size() && !isSpace(s[i]))
                return false;
        } else {
            while (i < s.size() && !isSpace(s[i])) {
                if (s[i] == '"')
                    return false;
                ++i;
            }

            valueSize = i - valueBegin;
        }

        std::string* destination = nullptr;
        if (key == "ts" && !hasTs) {
            hasTs = true;
            destination = &out->ts;
        } else if (key == "type" && !hasType) {
            hasType = true;
            destination = &out->type;
        } else if (key == "pid" && !hasPid) {
            hasPid = true;
            destination = &out->pid;
        } else {
            auto& field = out->fields.emplace_back();
            field.key = key;
            destination = &field.value;
        }
        *destination = s.substr(valueBegin, valueSize);
    }

    return hasTs && hasType && !out->ts.empty() && !out->type.empty();
}
}  // namespace nano_edr
