#include "os_source.h"

#include <cstdio>
#include <exception>
#include <print>
#include <stdexcept>

#include "os_handle.h"

namespace nano_edr {

OsSource::OsSource(const std::string& config_path) : handle_(config_path) {}

void OsSource::Run(IEventHandler* handler) {
    handle_.Subscribe(&OsSource::Trampoline, handler);
    handle_.Start();
    os_status status = handle_.Wait(100);
    while (status == OS_TIMEOUT) {
        handler->OnTick();
        status = handle_.Wait(100);
    }

    if (status != OS_OK) {
        throw std::runtime_error("os_wait: " + std::string(os_status_str(status)));
    }
}

void OsSource::Trampoline(const os_event* ev, void* ctx) noexcept {
    auto* handler = static_cast<IEventHandler*>(ctx);

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
        handler->HandleEvent(event);

    } catch (const std::exception& error) {
        handler->OnBroken();
        std::print(stderr, "Событие пропущено: {}\n", error.what());
    } catch (...) {
        handler->OnBroken();
        std::print(stderr, "Событие пропущено: неизвестное исключение\n");
    }
}

}  // namespace nano_edr
