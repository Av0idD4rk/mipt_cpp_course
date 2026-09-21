#include "rules.h"
#include <print>

namespace nano_edr {
size_t CheckRules(const Event& event, const Rule* rules, size_t rule_count) {
    size_t detected = 0;
    for (size_t i = 0; i < rule_count; ++i) {
        const auto& [id, check, severity] = rules[i];
        if (check(event)) {
            detected++;
            std::print("[DETECT] {}  {}  ts={} pid={}\n", SeverityName(severity), id, event.ts, event.pid);
        }
    }
    return detected;
}

const char* SeverityName(Severity severity) {
    switch (severity) {
        case Severity::kCritical: return "critical";
        case Severity::kHigh: return "high";
        case Severity::kMedium: return "medium";
        case Severity::kLow: return "low";
        default: return "?";
    }
};

}  // namespace nano_edr