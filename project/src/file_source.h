//
// Created by avdk on 05.10.2026.
//

#ifndef NANO_EDR_AGENT_FILE_SOURCE_H
#define NANO_EDR_AGENT_FILE_SOURCE_H
#include <string>

class Agent;

class FileSource {
public:
    explicit FileSource(const std::string& path);
    void Run(Agent* agent);

private:
    std::string path_;
};

#endif  //NANO_EDR_AGENT_FILE_SOURCE_H
