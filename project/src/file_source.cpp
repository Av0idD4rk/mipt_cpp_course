#include "file_source.h"

#include <cstdio>
#include <exception>
#include <print>
#include <stdexcept>

#include "agent.h"
#include "parse.h"

namespace nano_edr {

FileSource::FileSource(const std::string& path) : path_(path), log_(path) {
    if (!log_) {
        throw std::runtime_error("Не удалось открыть журнал: " + path_);
    }
}

void FileSource::Run(Agent* agent) {
    std::string line;

    while (std::getline(log_, line)) {
        try {
            EventParts parts;
            if (!ParseEventParts(line, &parts))
                continue;

            Event event(parts);
            agent->HandleEvent(event);
        } catch (const std::exception& e) {
            std::print(stderr, "Событие пропущено: {}\n", e.what());
        }
    }
    if (log_.bad()) {
        throw std::runtime_error(
            "Ошибка чтения журнала: " + path_);
    }
}

}  // namespace nano_edr
