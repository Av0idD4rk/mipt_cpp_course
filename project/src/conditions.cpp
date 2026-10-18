#include "conditions.h"

#include "fields.h"
#include "text.h"

namespace nano_edr {

bool FieldEquals::operator()(const Event& event) const {
    const std::string* field = FindField(event, key_);
    return field && EqualsIgnoreCase(*field, value_);
}

bool FieldContains::operator()(const Event& event) const {
    const std::string* field = FindField(event, key_);
    return field && ContainsIgnoreCase(*field, needle_);
}

bool FieldEndsWith::operator()(const Event& event) const {
    const std::string* field = FindField(event, key_);
    return field && EndsWithIgnoreCase(*field, suffix_);
}

bool FieldInList::operator()(const Event& event) const {
    const std::string* field = FindField(event, key_);
    if (!field)
        return false;

    for (const auto& value : values_) {
        if (EqualsIgnoreCase(*field, value))
            return true;
    }
    return false;
}

Condition Condition::Equals(const std::string& key, const std::vector<std::string>& values) {
    return {Match::kEquals, key, values};
}

Condition Condition::Contains(const std::string& key, const std::vector<std::string>& values) {
    return {Match::kContains, key, values};
}

Condition Condition::EndsWith(const std::string& key, const std::vector<std::string>& values) {
    return {Match::kEndsWith, key, values};
}

bool Condition::operator()(const Event& event) const {
    const std::string* field = FindField(event, key_);
    if (!field)
        return false;

    for (const auto& value : values_) {
        switch (match_) {
            case Match::kEquals:
                if (EqualsIgnoreCase(*field, value))
                    return true;
                break;
            case Match::kContains:
                if (ContainsIgnoreCase(*field, value))
                    return true;
                break;
            case Match::kEndsWith:
                if (EndsWithIgnoreCase(*field, value))
                    return true;
                break;
        }
    }
    return false;
}

}  // namespace nano_edr
