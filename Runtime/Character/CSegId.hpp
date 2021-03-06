#pragma once

#include "RetroTypes.hpp"
#include "IOStreams.hpp"
#include "zeus/CVector3f.hpp"

namespace urde {

class CSegId {
  u8 x0_segId = 0xff;

public:
  CSegId() = default;
  CSegId(u8 id) : x0_segId(id) {}
  CSegId(CInputStream& in) : x0_segId(in.readUint32Big()) {}
  CSegId& operator++() {
    ++x0_segId;
    return *this;
  }
  CSegId& operator--() {
    --x0_segId;
    return *this;
  }
  operator u8() const { return x0_segId; }
};

} // namespace urde
