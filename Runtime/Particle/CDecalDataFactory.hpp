#pragma once

#include <memory>

#include "Runtime/IFactory.hpp"
#include "Runtime/IOStreams.hpp"

namespace hecl {
class FourCC;
}

namespace urde {
class CDecalDescription;
class CObjectReference;
class CSimplePool;
class CVParamTransfer;

struct SObjectTag;
struct SQuadDescr;

class CDecalDataFactory {
  static bool CreateDPSM(CDecalDescription* desc, CInputStream& in, CSimplePool* resPool);
  static std::unique_ptr<CDecalDescription> CreateGeneratorDescription(CInputStream& in, CSimplePool* resPool);
  static void GetQuadDecalInfo(CInputStream& in, CSimplePool* resPool, hecl::FourCC clsId, SQuadDescr& quad);

public:
  static std::unique_ptr<CDecalDescription> GetGeneratorDesc(CInputStream& in, CSimplePool* resPool);
};

CFactoryFnReturn FDecalDataFactory(const SObjectTag& tag, CInputStream& in, const CVParamTransfer& vparms,
                                   CObjectReference*);
} // namespace urde
