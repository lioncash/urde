set(PARTICLE_SOURCES
    IElement.hpp
    CGenDescription.hpp
    CRealElement.hpp CRealElement.cpp
    CIntElement.hpp CIntElement.cpp
    CVectorElement.hpp CVectorElement.cpp
    CModVectorElement.hpp CModVectorElement.cpp
    CColorElement.hpp CColorElement.cpp
    CUVElement.hpp CUVElement.cpp
    CEmitterElement.hpp CEmitterElement.cpp
    CParticleDataFactory.hpp CParticleDataFactory.cpp
    CSwooshDescription.hpp
    CElectricDescription.hpp
    CDecalDescription.hpp
    CWeaponDescription.hpp
    CDecalDataFactory.hpp CDecalDataFactory.cpp
    CElementGen.hpp CElementGen.cpp
    CParticleSwooshDataFactory.hpp CParticleSwooshDataFactory.cpp
    CParticleSwoosh.hpp CParticleSwoosh.cpp
    CParticleElectricDataFactory.hpp CParticleElectricDataFactory.cpp
    CParticleElectric.hpp CParticleElectric.cpp
    CParticleGen.hpp CParticleGen.cpp
    CProjectileWeaponDataFactory.hpp CProjectileWeaponDataFactory.cpp
    CDecal.hpp CDecal.cpp
    CDecalManager.hpp CDecalManager.cpp
    CSpawnSystemKeyframeData.hpp CSpawnSystemKeyframeData.cpp
    CWarp.hpp
    CFlameWarp.hpp CFlameWarp.cpp
    CParticleGlobals.hpp CParticleGlobals.cpp
    ${PLAT_SRCS})

runtime_add_list(Particle PARTICLE_SOURCES)
