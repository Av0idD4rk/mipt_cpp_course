#include "text.h"

namespace nano_edr {
static std::string ToLower(std::string text) {
    for (char& ch : text) {
        if (ch >= 'A' && ch <= 'Z') {
            ch = static_cast<char>(ch + ('a' - 'A'));
        }
    }
    return text;
}

bool EqualsIgnoreCase(const std::string& text, const std::string& value) {
    return ToLower(text) == ToLower(value);
}

bool ContainsIgnoreCase(const std::string& text, const std::string& needle) {
    return ToLower(text).contains(ToLower(needle));
}

bool EndsWithIgnoreCase(const std::string& text, const std::string& suffix) {
    return ToLower(text).ends_with(ToLower(suffix));
}

}  // namespace nano_edr
