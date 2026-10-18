#include "agent_rules.h"

namespace nano_edr {

std::vector<MatchRule> BuildAgentRules() {
    MatchRule script_host("script_host_from_temp", Severity::kHigh);
    script_host.AddCondition(Condition::Equals("type", {"process_start"}));
    script_host.AddCondition(Condition::EndsWith("image", {"wscript.exe", "cscript.exe"}));
    script_host.AddCondition(Condition::Contains("cmdline", {
        R"(\appdata\local\temp\)", "/appdata/local/temp/",
        R"(\windows\temp\)", "/windows/temp/",
        "%temp%", "%tmp%"}));
    script_host.AddAction("kill_process");

    MatchRule lolbin("lolbin_download", Severity::kHigh);
    lolbin.AddCondition(Condition::Equals("type", {"process_start"}));
    lolbin.AddCondition(Condition::EndsWith("image", {"certutil.exe", "bitsadmin.exe"}));
    lolbin.AddCondition(Condition::Contains("cmdline", {"urlcache", "transfer", "http:", "https:"}));
    lolbin.AddAction("kill_process");

    MatchRule powershell("hidden_powershell", Severity::kMedium);
    powershell.AddCondition(Condition::Equals("type", {"process_start"}));
    powershell.AddCondition(Condition::EndsWith("image", {"powershell.exe", "pwsh.exe"}));
    powershell.AddCondition(Condition::Contains("cmdline", {"-w hidden", "-windowstyle hidden", "-enc", "-encodedcommand"}));
    powershell.AddAction("kill_process");

    MatchRule autostart_path("autostart_write", Severity::kHigh);
    autostart_path.AddCondition(Condition::Equals("type", {"file_create", "file_write"}));
    autostart_path.AddCondition(Condition::Contains("path", {
        R"(\start menu\programs\startup\)", "/start menu/programs/startup/"}));
    autostart_path.AddAction("quarantine_file");

    MatchRule autostart_to("autostart_write", Severity::kHigh);
    autostart_to.AddCondition(Condition::Equals("type", {"file_move"}));
    autostart_to.AddCondition(Condition::Contains("to", {
        R"(\start menu\programs\startup\)", "/start menu/programs/startup/"}));
    autostart_to.AddAction("quarantine_file");

    MatchRule ransom_path("ransom_extension", Severity::kCritical);
    ransom_path.AddCondition(Condition::Equals("type", {"file_create", "file_write"}));
    ransom_path.AddCondition(Condition::EndsWith("path", {".locked"}));
    ransom_path.AddAction("quarantine_file");

    MatchRule ransom_to("ransom_extension", Severity::kCritical);
    ransom_to.AddCondition(Condition::Equals("type", {"file_move"}));
    ransom_to.AddCondition(Condition::EndsWith("to", {".locked"}));
    ransom_to.AddAction("quarantine_file");

    return {script_host, lolbin, powershell, autostart_path, autostart_to, ransom_path, ransom_to};
}

}  // namespace nano_edr
