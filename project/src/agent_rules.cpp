#include "agent_rules.h"

namespace nano_edr {

bool ScriptHostFromTemp(const Event& event) {
    return true;
}
bool LolbinDownload(const Event& event) {
    return true;
}
bool HiddenPowershell(const Event& event) {
    return true;
}
bool AutostartWrite(const Event& event) {
    return true;
}
bool RansomExtension(const Event& event) {
    return true;
}

constexpr Rule kRules[] = {
    {.id = "script_host_from_temp", .check = ScriptHostFromTemp, .severity = Severity::kHigh},
    {.id = "lolbin_download", .check = LolbinDownload, .severity = Severity::kHigh},
    {.id = "hidden_powershell", .check = HiddenPowershell, .severity = Severity::kMedium},
    {.id = "autostart_write", .check = AutostartWrite, .severity = Severity::kHigh},
    {.id = "ransom_extension", .check = RansomExtension, .severity = Severity::kCritical},
};

const Rule* AgentRules() {
    return kRules;
}

size_t AgentRuleCount() {
    return std::size(kRules);
}

}  // namespace nano_edr