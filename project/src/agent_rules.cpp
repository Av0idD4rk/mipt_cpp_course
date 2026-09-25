#include "agent_rules.h"

#include "fields.h"

namespace nano_edr {

namespace {

bool ImageIs(const Event& event, const std::string& name) {
    const std::string path =
        NormalizePath(GetRequiredField(event, "image"));

    return path == name || path.ends_with("\\" + name);
}

bool IsFileChange(const Event& event) {
    return event.type == "file_create" ||
           IsFileWrite(event) ||
           event.type == "file_move";
}

}  // namespace

static bool ScriptHostFromTemp(const Event& event) {
    if (!IsProcessStart(event) ||
        !(ImageIs(event, "wscript.exe") ||
          ImageIs(event, "cscript.exe"))) {
        return false;
    }

    const std::string* command = FindField(event, "cmdline");
    if (command == nullptr) {
        return false;
    }

    const std::string normalized = NormalizePath(*command);
    return normalized.contains(R"(\appdata\local\temp\)") ||
           normalized.contains(R"(\windows\temp\)");
}
static bool LolbinDownload(const Event& event) {
    return IsProcessStart(event) &&
           (ImageIs(event, "certutil.exe") ||
            ImageIs(event, "bitsadmin.exe")) &&
           (CommandLineContains(event, "urlcache") ||
            CommandLineContains(event, "transfer") ||
            CommandLineContains(event, "http:") ||
            CommandLineContains(event, "https:"));
}
static bool HiddenPowershell(const Event& event) {
    return IsProcessStart(event) &&
           (ImageIs(event, "powershell.exe") ||
            ImageIs(event, "pwsh.exe")) &&
           (CommandLineContains(event, "-w hidden") ||
            CommandLineContains(event, "-windowstyle hidden") ||
            CommandLineContains(event, "-enc") ||
            CommandLineContains(event, "-encodedcommand"));
}
static bool AutostartWrite(const Event& event) {
    if (!IsFileChange(event)) {
        return false;
    }

    const std::string* path = FindField(
        event, event.type == "file_move" ? "to" : "path");

    return path && NormalizePath(*path).contains(
                       R"(\start menu\programs\startup\)");
}

static bool RansomExtension(const Event& event) {
    if (!IsFileChange(event)) {
        return false;
    }

    if (event.type == "file_move") {
        const std::string* to = FindField(event, "to");
        return to && NormalizePath(*to).ends_with(".locked");
    }

    return PathEndsWith(event, ".locked");
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