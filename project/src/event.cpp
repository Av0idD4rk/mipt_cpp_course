#include "event.h"

#include <charconv>
#include <stdexcept>

namespace nano_edr {
Event::Event(const EventParts& parts) : raw_ts_(parts.ts),
                                        type_(parts.type),
                                        event_type_(EventTypeFromString(parts.type)),
                                        pid_(parts.pid),
                                        fields_(parts.fields) {
    if (type_.empty()) {
        throw std::invalid_argument("Пустой тип события: ''");
    }

    const char* begin = raw_ts_.data();
    const char* end = begin + raw_ts_.size();

    const auto [ptr, error] = std::from_chars(begin, end, ts_.ms);

    if (error != std::errc{} || ptr != end) {
        throw std::invalid_argument(
            "Неверное время события: " + raw_ts_);
    }
}

std::string ToString(const Event& event) {
    std::string result =
        "ts=" + event.raw_ts() +
        " type=" + event.type() +
        " pid=" + event.pid();

    for (const auto& field : event.fields()) {
        result.append(" " + field.key + "=" + field.value);
    }
    return result;
}

std::ostream& operator<<(std::ostream& out, const Event& event) {
    return out << ToString(event);
}

EventType EventTypeFromString(const std::string& type) {
    if (type == "process_start") return EventType::kProcessStart;
    if (type == "process_end") return EventType::kProcessEnd;
    if (type == "file_create") return EventType::kFileCreate;
    if (type == "file_write") return EventType::kFileWrite;
    if (type == "file_delete") return EventType::kFileDelete;
    if (type == "file_move") return EventType::kFileMove;
    if (type == "net_connect") return EventType::kNetConnect;

    return EventType::kOther;
}

}  // namespace nano_edr