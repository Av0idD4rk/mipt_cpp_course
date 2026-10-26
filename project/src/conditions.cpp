#include "conditions.h"

#include "fields.h"
#include "text.h"

#include <utility>

namespace nano_edr {

bool FieldEquals::Matches(const Event& event) const {
    const std::string* field = FindField(event, key_);
    return field && EqualsIgnoreCase(*field, value_);
}

bool FieldContains::Matches(const Event& event) const {
    const std::string* field = FindField(event, key_);
    return field && ContainsIgnoreCase(*field, needle_);
}

bool FieldEndsWith::Matches(const Event& event) const {
    const std::string* field = FindField(event, key_);
    return field && EndsWithIgnoreCase(*field, suffix_);
}

bool FieldInList::Matches(const Event& event) const {
    const std::string* field = FindField(event, key_);
    if (!field)
        return false;

    for (const auto& value : values_) {
        if (EqualsIgnoreCase(*field, value))
            return true;
    }
    return false;
}

bool EventTypeIs::Matches(const Event& event) const {
    for (const auto& type : types_) {
        if (event.event_type() == type)
            return true;
    }
    return false;
}

void AllOf::Add(std::unique_ptr<ICondition> condition) {
    parts_.push_back(std::move(condition));
}

bool AllOf::Matches(const Event& event) const {
    for (const auto& condition : parts_) {
        if (!condition->Matches(event))
            return false;
    }
    return true;
}

void AnyOf::Add(std::unique_ptr<ICondition> condition) {
    parts_.push_back(std::move(condition));
}

bool AnyOf::Matches(const Event& event) const {
    for (const auto& condition : parts_) {
        if (condition->Matches(event))
            return true;
    }
    return false;
}

}  // namespace nano_edr
