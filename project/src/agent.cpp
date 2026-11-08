#include "agent.h"

#include <print>
#include <vector>

#include "agent_rules.h"
#include "rules.h"

namespace nano_edr {

Agent::Agent(const bool quiet) : rule_engine_(BuildAgentRules()), quiet_(quiet) {}

void Agent::HandleEvent(const Event& event) {
    std::vector<const IRule*> fired;
    const auto detected = rule_engine_.ProcessEvent(event, &fired);

    if (detected != 0 && !quiet_)
        PrintContext();

    for (const auto* rule : fired) {
        std::print(
            "[DETECT] {}  {}  ts={} pid={}\n",
            SeverityName(rule->severity()),
            rule->id(),
            event.raw_ts(),
            event.pid());
    }

    window_.PushBack(WindowEntry{
        .ts = event.ts(),
        .type = event.type(),
        .pid = event.pid()
    });

    ++events_;
    ++events_by_type_[event.type()];
}

void Agent::PrintContext() const {
    if (window_.empty())
        return;

    if (window_.size() >= 2) {
        const auto& entry = window_[window_.size() - 2];
        std::print(
            "[CTX] -2: {} {} pid={}\n",
            entry.ts.ms,
            entry.type,
            entry.pid);
    }

    const auto& entry = window_.back();
    std::print(
        "[CTX] -1: {} {} pid={}\n",
        entry.ts.ms,
        entry.type,
        entry.pid);
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
