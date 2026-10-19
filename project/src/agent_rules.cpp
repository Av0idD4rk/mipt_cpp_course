#include "agent_rules.h"

#include <initializer_list>
#include <memory>
#include <utility>
#include <vector>

namespace nano_edr {

RuleEngine BuildAgentRules() {
    RuleEngine engine;

    auto script_host = std::make_unique<MatchRule>(
        "script_host_from_temp", Severity::kHigh);
    script_host->AddCondition(std::make_unique<EventTypeIs>(
        std::vector<EventType>{EventType::kProcessStart}));

    auto script_images = std::make_unique<AnyOf>();
    for (const char* image : {"wscript.exe", "cscript.exe"}) {
        script_images->Add(
            std::make_unique<FieldEndsWith>("image", image));
    }
    script_host->AddCondition(std::move(script_images));

    auto temp = std::make_unique<AnyOf>();
    for (const char* path : {
             R"(\appdata\local\temp\)", "/appdata/local/temp/",
             R"(\windows\temp\)", "/windows/temp/",
             "%temp%", "%tmp%"}) {
        temp->Add(std::make_unique<FieldContains>("cmdline", path));
    }
    script_host->AddCondition(std::move(temp));
    script_host->AddAction("kill_process");
    engine.AddRule(std::move(script_host));

    auto lolbin = std::make_unique<MatchRule>(
        "lolbin_download", Severity::kHigh);
    lolbin->AddCondition(std::make_unique<EventTypeIs>(
        std::vector<EventType>{EventType::kProcessStart}));

    auto lolbin_images = std::make_unique<AnyOf>();
    for (const char* image : {"certutil.exe", "bitsadmin.exe"}) {
        lolbin_images->Add(
            std::make_unique<FieldEndsWith>("image", image));
    }
    lolbin->AddCondition(std::move(lolbin_images));

    auto download = std::make_unique<AnyOf>();
    for (const char* value : {"urlcache", "transfer", "http:", "https:"}) {
        download->Add(std::make_unique<FieldContains>("cmdline", value));
    }
    lolbin->AddCondition(std::move(download));
    lolbin->AddAction("kill_process");
    engine.AddRule(std::move(lolbin));

    auto powershell = std::make_unique<MatchRule>(
        "hidden_powershell", Severity::kMedium);
    powershell->AddCondition(std::make_unique<EventTypeIs>(
        std::vector<EventType>{EventType::kProcessStart}));

    auto powershell_images = std::make_unique<AnyOf>();
    for (const char* image : {"powershell.exe", "pwsh.exe"}) {
        powershell_images->Add(
            std::make_unique<FieldEndsWith>("image", image));
    }
    powershell->AddCondition(std::move(powershell_images));

    auto hidden = std::make_unique<AnyOf>();
    for (const char* value : {
             "-w hidden", "-windowstyle hidden",
             "-enc", "-encodedcommand"}) {
        hidden->Add(std::make_unique<FieldContains>("cmdline", value));
    }
    powershell->AddCondition(std::move(hidden));
    powershell->AddAction("kill_process");
    engine.AddRule(std::move(powershell));

    auto autostart = std::make_unique<MatchRule>(
        "autostart_write", Severity::kHigh);
    autostart->AddCondition(std::make_unique<EventTypeIs>(
        std::vector<EventType>{
            EventType::kFileCreate,
            EventType::kFileWrite,
            EventType::kFileMove}));

    auto startup = std::make_unique<AnyOf>();
    for (const char* key : {"path", "to"}) {
        for (const char* path : {
                 R"(\start menu\programs\startup\)",
                 "/start menu/programs/startup/"}) {
            startup->Add(std::make_unique<FieldContains>(key, path));
        }
    }
    autostart->AddCondition(std::move(startup));
    autostart->AddAction("quarantine_file");
    engine.AddRule(std::move(autostart));

    auto ransom = std::make_unique<MatchRule>(
        "ransom_extension", Severity::kCritical);
    ransom->AddCondition(std::make_unique<EventTypeIs>(
        std::vector<EventType>{
            EventType::kFileCreate,
            EventType::kFileWrite,
            EventType::kFileMove}));

    auto paths = std::make_unique<AnyOf>();
    paths->Add(std::make_unique<FieldEndsWith>("path", ".locked"));
    paths->Add(std::make_unique<FieldEndsWith>("to", ".locked"));
    ransom->AddCondition(std::move(paths));
    ransom->AddAction("quarantine_file");
    engine.AddRule(std::move(ransom));

    return engine;
}

}  // namespace nano_edr