#include "os_source.h"

#include <stdexcept>

#include "agent.h"
#include "os_handle.h"

OsSource::OsSource(const std::string& config_path) : handle_(config_path) {}

void OsSource::Run(Agent* agent) {
    handle_.Subscribe(Agent::Trampoline, agent);
    handle_.Start();
    os_status status = handle_.Wait(100);
    while (status == OS_TIMEOUT) {
        status = handle_.Wait(100);
    }

    if (status != OS_OK) {
        throw std::runtime_error("os_wait: " + std::string(os_status_str(status)));
    }
}