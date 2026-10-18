#ifndef NANO_EDR_AGENT_OS_SOURCE_H
#define NANO_EDR_AGENT_OS_SOURCE_H
#include <string>
#include "os_handle.h"

namespace nano_edr {

class Agent;

class OsSource {
public:
    explicit OsSource(const std::string& config_path);
    void Run(Agent* agent);
private:
    OsHandle handle_;
};

}  // namespace nano_edr

#endif  //NANO_EDR_AGENT_OS_SOURCE_H
