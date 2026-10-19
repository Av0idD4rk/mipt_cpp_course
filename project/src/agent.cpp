#include "agent.h"

#include <cstdio>
#include <exception>
#include <print>

#include "agent_rules.h"
#include "rules.h"

namespace nano_edr {

Agent::Agent(const std::size_t window_size, const bool quiet) : window_(window_size), rule_engine_(BuildAgentRules()), quiet_(quiet) {}

void Agent::HandleEvent(const Event& event) {
    std::vector<const IRule*> fired;
    const auto detected = rule_engine_.ProcessEvent(event, &fired);
    for (const auto* rule : fired) {
        std::print(
            "[DETECT] {}  {}  ts={} pid={}\n",
            SeverityName(rule->severity()),
            rule->id(),
            event.raw_ts(),
            event.pid());
    }

    if (detected != 0 && !quiet_)
        PrintContext(context_);

    window_.PushBack(event);

    if (window_.size() <= 2)
        context_ = window_.head();
    else
        context_ = context_->next;

    ++events_;
    ++events_by_type_[event.type()];
}

void Agent::PrintContext(const EventNode* first) {
    if (!first)
        return;

    if (first->next) {
        std::print(
            "[CTX] -2: ts={} type={} pid={}\n",
            first->event.raw_ts(),
            first->event.type(),
            first->event.pid());
        first = first->next;
    }
    std::print(
        "[CTX] -1: ts={} type={} pid={}\n",
        first->event.raw_ts(),
        first->event.type(),
        first->event.pid());
}

void Agent::PrintSummary() const {
    if (quiet_) return;
    std::print("--------------------------------------------\n");
    std::print("Всего событий: {}\n", events_);
    std::print("Пропущено строк: {}\n", skipped_);
    std::print("Испорчено событий: {}\n", broken_);

    for (const auto& [type, count] : events_by_type_)
        std::print("{}: {}\n", type, count);

    std::print("Правила, детектов:\n");
    for (std::size_t i = 0; i < rule_engine_.size(); ++i) {
        const auto& rule = rule_engine_.rule(i);
        std::print("  {}  {}\n", rule.id(), rule.hits());
    }
}

void Agent::OnBroken() {
    ++broken_;
}

void Agent::OnSkipped() {
    ++skipped_;
}
void Agent::OnTick() {}

}  // namespace nano_edr
