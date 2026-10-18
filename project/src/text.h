#ifndef NANO_EDR_AGENT_TEXT_H
#define NANO_EDR_AGENT_TEXT_H

#include <string>

namespace nano_edr {

bool EqualsIgnoreCase(const std::string& text, const std::string& value);
bool ContainsIgnoreCase(const std::string& text, const std::string& needle);
bool EndsWithIgnoreCase(const std::string& text, const std::string& suffix);

}  // namespace nano_edr

#endif  // NANO_EDR_AGENT_TEXT_H
