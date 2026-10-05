#include "os_handle.h"

#include <stdexcept>
#include <string>

namespace nano_edr {
OsHandle::OsHandle(const std::string& config_path) {
    const os_status status = os_init(config_path.c_str(), &handle_);
    if (status != OS_OK) {
        throw std::runtime_error("os_init: " + std::string(os_status_str(status)) + " in " + config_path);
    }

}

OsHandle::~OsHandle() {
    os_free(handle_);
}

void OsHandle::Subscribe(const os_event_cb callback, void* context) {
    const os_status status = os_event_subscribe(handle_, callback, context);
    if (status != OS_OK) {
        throw std::runtime_error("os_event_subscribe: " + std::string(os_status_str(status)));
    }
}

void OsHandle::Start() {
    const os_status status = os_start(handle_);
    if (status != OS_OK) {
        throw std::runtime_error("os_start: " + std::string(os_status_str(status)));
    }
}

os_status OsHandle::Wait(const uint32_t timeout_ms) {
    return os_wait(handle_, timeout_ms);

}
void OsHandle::Stop() {
    os_stop(handle_);
}

}  // namespace nano_edr