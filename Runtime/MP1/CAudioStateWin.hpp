#pragma once

#include "../CIOWin.hpp"

namespace urde::MP1 {
class CAudioStateWin : public CIOWin {
public:
  CAudioStateWin() : CIOWin("CAudioStateWin") {}
  CIOWin::EMessageReturn OnMessage(const CArchitectureMessage& msg, CArchitectureQueue& queue);
};

} // namespace urde::MP1
