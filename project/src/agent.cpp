#include "agent.h"

#include <cstdio>
#include <exception>
#include <print>

#include "agent_rules.h"
#include "rules.h"

namespace nano_edr {

Agent::Agent(const std::size_t window_size, const bool quiet) : window_(window_size), rules_(BuildAgentRules()), quiet_(quiet) {}

void Agent::HandleEvent(const Event& event) {
    std::size_t detected = 0;
    for (auto& rule : rules_) {
        if (rule.Check(event)) {
            ++detected;
            std::print("[DETECT] {}  {}  ts={} pid={}\n", SeverityName(rule.severity()), rule.id(), event.raw_ts(), event.pid());
        }
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

    for (const auto& [type, count] : events_by_type_)
        std::print("{}: {}\n", type, count);

    std::print("Правила, детектов:\n");
    for (const auto& rule : rules_)
        std::print("  {}  {}\n", rule.id(), rule.hits());
}

void Agent::Trampoline(const os_event* ev, void* ctx) noexcept {
    try {
        EventParts parts;
        parts.type = ev->type;
        if (ev->pid != 0) {
            parts.pid = std::to_string(ev->pid);
        }
        parts.ts = std::to_string(ev->ts);
        for (size_t i = 0; i < ev->field_count; ++i) {
            parts.fields.push_back(Field{.key = ev->fields[i].key, .value = ev->fields[i].value});
        }

        const Event event(parts);
        static_cast<Agent*>(ctx)->HandleEvent(event);

    } catch (const std::exception& error) {
        std::print(stderr, "Событие пропущено: {}\n", error.what());
    } catch (...) {
        std::print(stderr, "Событие пропущено: неизвестное исключение\n");
    }
}

}  // namespace nano_edr
