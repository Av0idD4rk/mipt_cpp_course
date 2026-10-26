#ifndef NANO_EDR_AGENT_AGENT_H
#define NANO_EDR_AGENT_AGENT_H
#include <cstddef>
#include <map>
#include <string>

#include "event.h"
#include "event_list.h"
#include "event_source.h"
#include "rule_engine.h"

namespace nano_edr {

class Agent : public IEventHandler {
 public:
    Agent(std::size_t window_size, bool quiet);
    void HandleEvent(const Event& event) override;
    void PrintSummary() const;
    void OnSkipped() override;
    void OnBroken() override;
    void OnTick() override;

 private:
    static void PrintContext(const EventNode* first);

    EventList window_;
    RuleEngine rule_engine_;
    bool quiet_;
    std::size_t events_ = 0;
    std::map<std::string, std::size_t> events_by_type_;
    const EventNode* context_ = nullptr;
    std::size_t skipped_ = 0;
    std::size_t broken_ = 0;
};

}  // namespace nano_edr

#endif  //NANO_EDR_AGENT_AGENT_H
