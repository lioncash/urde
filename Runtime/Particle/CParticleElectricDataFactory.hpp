#pragma once

#include "RetroTypes.hpp"
#include "CFactoryMgr.hpp"
#include "IObj.hpp"
#include "CToken.hpp"
#include "IOStreams.hpp"

namespace urde {
class CElectricDescription;
class CSimplePool;
class CParticleElectricDataFactory {
  static CElectricDescription* CreateElectricDescription(CInputStream& in, CSimplePool* resPool);
  static bool CreateELSM(CElectricDescription* desc, CInputStream& in, CSimplePool* resPool);
  static void LoadELSMTokens(CElectricDescription* desc);

public:
  static CElectricDescription* GetGeneratorDesc(CInputStream& in, CSimplePool* resPool);
};

CFactoryFnReturn FParticleElectricDataFactory(const SObjectTag& tag, CInputStream& in, const CVParamTransfer& vparms,
                                              CObjectReference*);
} // namespace urde
