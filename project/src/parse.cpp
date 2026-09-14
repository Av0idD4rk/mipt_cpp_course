#include "parse.h"

namespace nano_edr {
    bool ParseFields(const std::string *text, std::vector<Field> *out) {
        if (!text || !out)
            return false;

        const std::string &s = *text;
        std::vector<Field> parsed_fields;
        std::size_t i = 0;

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

            std::string key = s.substr(keyBegin, i - keyBegin);
            ++i;

            std::string value;

            if (i < s.size() && s[i] == '"') {
                const auto valueBegin = ++i;
                const auto closingQuote = s.find('"', i);

                if (closingQuote == std::string::npos)
                    return false;

                value = s.substr(valueBegin, closingQuote - valueBegin);
                i = closingQuote + 1;

                if (i < s.size() && !isSpace(s[i]))
                    return false;
            } else {
                const auto valueBegin = i;

                while (i < s.size() && !isSpace(s[i])) {
                    if (s[i] == '"')
                        return false;
                    ++i;
                }

                value = s.substr(valueBegin, i - valueBegin);
            }

            parsed_fields.push_back({.key = std::move(key), .value = std::move(value)});
        }

        if (!parsed_fields.empty())
            out->insert(
                out->end(),
                std::make_move_iterator(parsed_fields.begin()),
                std::make_move_iterator(parsed_fields.end())
            );
        return true;
    }

    bool IsBlankOrComment(const std::string* line) {
        if (!line)
            return true;
        for (char c : *line) {
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

        std::vector<Field> pairs;
        if (!ParseFields(line, &pairs))
            return false;

        Event event;
        bool hasTs = false;
        bool hasType = false;
        bool hasPid = false;

        for (auto& field : pairs) {
            if (field.key == "ts" && !hasTs) {
                if (hasTs || field.value.empty())
                    return false;

                hasTs = true;
                event.ts = std::move(field.value);
            } else if (field.key == "type" && !hasType) {
                if (hasType || field.value.empty())
                    return false;

                hasType = true;
                event.type = std::move(field.value);
            } else if (field.key == "pid" && !hasPid) {
                if (hasPid)
                    return false;
                hasPid = true;
                event.pid = std::move(field.value);
            } else {
                event.fields.push_back(std::move(field));
            }
        }

        if (!hasTs || !hasType || event.ts.empty() || event.type.empty())
            return false;

        *out = std::move(event);
        return true;
    }

}
