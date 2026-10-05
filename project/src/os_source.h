//
// Created by avdk on 05.10.2026.
//

#ifndef NANO_EDR_AGENT_OS_SOURCE_H
#define NANO_EDR_AGENT_OS_SOURCE_H
#include <string>
#include "os_handle.h"

class Agent;

class OsSource {
public:
    explicit OsSource(const std::string& config_path);
    void Run(Agent* agent);
private:
    nano_edr::OsHandle handle_;
};
#endif  //NANO_EDR_AGENT_OS_SOURCE_H
