#include "rule.h"

#include <utility>

namespace nano_edr {

RuleBase::RuleBase(const std::string& id, const Severity severity) : id_(id), severity_(severity) {}

void RuleBase::AddAction(const std::string& action) {
    actions_.push_back(action);
}

MatchRule::MatchRule(const std::string& id, const Severity severity) : RuleBase(id, severity) {}

void MatchRule::AddCondition(std::unique_ptr<ICondition> condition) {
    conditions_.push_back(std::move(condition));
}

bool MatchRule::Check(const Event& event) {
    for (const auto& condition : conditions_) {
        if (!condition->Matches(event))
            return false;
    }
    CountHit();
    return true;
}

SequenceRule::SequenceRule(const std::string& id, const Severity severity, uint64_t window_ms) : RuleBase(id, severity), window_ms_(window_ms) {}

void SequenceRule::SetFirst(std::unique_ptr<ICondition> condition) {}
void SequenceRule::SetSecond(std::unique_ptr<ICondition> condition) {}
bool SequenceRule::Check(const Event& event) {
    return false;
}

}  // namespace nano_edr
