#include "rules.h"

namespace nano_edr {
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
