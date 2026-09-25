#include "fields.h"
#include <string_view>
#include <charconv>
#include <stdexcept>

namespace nano_edr {
static std::string ToLower(std::string text) {
    for (char& ch : text) {
        ch = static_cast<char>(
            std::tolower(static_cast<unsigned char>(ch))
        );
    }
    return text;
}
const std::string* FindField(const Event& event, const std::string& key) {
    for (auto& [field_key, value] : event.fields) {
        if (field_key == key) {
            return &value;
        }
    }
    return nullptr;
}
const std::string& GetRequiredField(const Event& event, const std::string& key) {
    if (const std::string* value = FindField(event, key)) {
        return *value;
    }
    throw std::invalid_argument("Обязательное поле не найдено: " + key);
}

bool GetIntField(const Event& event, const std::string& key, uint64_t* out) {
    const std::string* value = FindField(event, key);
    if (value == nullptr || out == nullptr) {
        return false;
    }
    uint64_t parsed;
    const char* begin = value->data();
    const char* end = value->data() + value->size();

    auto [ptr, err] = std::from_chars(begin, end, parsed);

    if (err != std::errc() || ptr != end) {
        return false;
    }
    * out = parsed;
    return true;
}

uint64_t GetIntField(const Event& event, const std::string& key,
                     uint64_t fallback) {
    uint64_t result;
    if (GetIntField(event, key, &result)) {
        return result;
    }
    return fallback;
}

bool IsProcessStart(const Event& event) {
    return event.type == "process_start";
}

bool IsFileWrite(const Event& event) {
    return event.type == "file_write";
}

bool IsNetConnect(const Event& event) {
    return event.type == "net_connect";
}

bool PathEndsWith(const Event& event, const std::string& suffix) {
    if (const std::string* path = FindField(event, "path")) {
        return ToLower(*path).ends_with(ToLower(suffix));
    }
    return false;
}

bool CommandLineContains(const Event& event, const std::string& needle) {
    if (const std::string* command = FindField(event, "cmdline")) {
        return ToLower(*command).contains(ToLower(needle));
    }
    return false;
}



std::string NormalizePath(const std::string& path) {
    std::string normalized = ToLower(path);

    static constexpr std::string_view replacement = R"(\appdata\local\temp)";

    auto replace_all = [&](const std::string& token) {
        std::size_t pos = 0;

        while ((pos = normalized.find(token, pos)) != std::string::npos) {
            normalized.replace(pos, token.size(), replacement);
            pos += replacement.size();
        }
    };

    replace_all("%temp%");
    replace_all("%tmp%");

    std::string result;
    result.reserve(normalized.size());

    for (char ch : normalized) {
        if (ch == '/') {
            ch = '\\';
        }

        if (ch == '\\' && !result.empty() && result.back() == '\\') {
            continue;
        }

        result.push_back(ch);
    }

    return result;
}
}  // namespace nano_edr