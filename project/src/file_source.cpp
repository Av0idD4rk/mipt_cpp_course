#include "file_source.h"

#include <cstdio>
#include <exception>
#include <fstream>
#include <print>
#include <stdexcept>

#include "agent.h"
#include "parse.h"

FileSource::FileSource(const std::string& path) : path_(path) {}

void FileSource::Run(Agent* agent) {
    std::ifstream log(path_);

    if (!log) {
        throw std::runtime_error("Не удалось открыть журнал: " + path_);
    }

    std::string line;

    while (std::getline(log, line)) {
        nano_edr::EventParts parts;
        if (!nano_edr::ParseEventParts(line, &parts))
            continue;

        try {
            nano_edr::Event event(parts);
            agent->HandleEvent(event);
        } catch (const std::exception& e) {
            std::print(stderr, "Событие пропущено: {}\n", e.what());
        }
    }
    if (log.bad()) {
        throw std::runtime_error(
            "Ошибка чтения журнала: " + path_);
    }
}