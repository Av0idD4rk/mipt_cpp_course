#include "file_source.h"

#include <cstdio>
#include <exception>
#include <print>
#include <stdexcept>

#include "parse.h"

namespace nano_edr {

FileSource::FileSource(const std::string& path) : path_(path), log_(path) {
    if (!log_) {
        throw std::runtime_error("Не удалось открыть журнал: " + path_);
    }
}

void FileSource::Run(IEventHandler* handler) {
    std::string line;

    while (std::getline(log_, line)) {
        try {
            if (IsBlankOrComment(line)) {
                handler->OnSkipped();
                continue;
            }
        
            EventParts parts;
            if (!ParseEventParts(line, &parts)) {
                handler->OnBroken();
                continue;
            }

            Event event(parts);
            handler->HandleEvent(event);
        } catch (const std::exception& error) {
            handler->OnBroken();
            std::print(stderr, "Событие пропущено: {}\n", error.what());
        }
    }

    if (log_.bad()) {
        throw std::runtime_error("Ошибка чтения журнала: " + path_);
    }
}

}  // namespace nano_edr
