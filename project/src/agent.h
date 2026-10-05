//
// Created by avdk on 05.10.2026.
//

#ifndef NANO_EDR_AGENT_AGENT_H
#define NANO_EDR_AGENT_AGENT_H
#include <cstddef>
#include <map>
#include <string>

#include "event.h"
#include "event_list.h"
#include "os.h"

class Agent {
 public:
    Agent(std::size_t window_size, bool quiet);
    void HandleEvent(const nano_edr::Event& event);
    void PrintSummary() const;

    static void Trampoline(const os_event* ev, void* ctx) noexcept;

 private:
    static void PrintContext(const nano_edr::EventNode* first);

    nano_edr::EventList window_;
    bool quiet_;
    std::size_t events_ = 0;
    std::map<std::string, std::size_t> events_by_type_;
    const nano_edr::EventNode* context_ = nullptr;
};

#endif  //NANO_EDR_AGENT_AGENT_H
