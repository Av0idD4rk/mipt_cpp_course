#include "agent.h"

#include <print>

#include "agent_rules.h"
#include "rules.h"

Agent::Agent(const std::size_t window_size, const bool quiet) : window_(window_size), quiet_(quiet) {}

void Agent::HandleEvent(const nano_edr::Event& event) {
    const auto detected = nano_edr::CheckRules(
        event,
        nano_edr::AgentRules(),
        nano_edr::AgentRuleCount());

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

void Agent::PrintContext(const nano_edr::EventNode* first) {
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

    for (const auto& [type, count] : events_by_type_)
        std::print("{}: {}\n", type, count);
}

void Agent::Trampoline(const os_event* ev, void* ctx) noexcept {
    try {
        nano_edr::EventParts parts;
        parts.type = ev->type;
        if (ev->pid != 0) {
            parts.pid = std::to_string(ev->pid);
        }
        parts.ts = std::to_string(ev->ts);
        for (size_t i = 0; i < ev->field_count; ++i) {
            parts.fields.push_back(nano_edr::Field{.key = ev->fields[i].key, .value = ev->fields[i].value});
        }

        const nano_edr::Event event(parts);
        static_cast<Agent*>(ctx)->HandleEvent(event);

    } catch (...) {
    }
}