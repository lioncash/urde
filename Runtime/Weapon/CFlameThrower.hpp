#pragma once

#include "Weapon/CGameProjectile.hpp"
#include "Particle/CFlameWarp.hpp"

namespace urde {
class CFlameInfo;
class CElementGen;
class CFlameThrower : public CGameProjectile {
public:
  enum class EFlameState {
    Default,
    FireStart,
    FireActive,
    FireStopTimer,
    FireWaitForParticlesDone
  };
private:
  static const zeus::CVector3f kLightOffset;
  zeus::CTransform x2e8_flameXf;
  zeus::CAABox x318_flameBounds = zeus::skNullBox;
  float x330_particleWaitDelayTimer = 0.f;
  float x334_fireStopTimer = 0.f;
  float x338_;
  TToken<CGenDescription> x33c_flameDesc;
  std::unique_ptr<CElementGen> x348_flameGen;
  CFlameWarp x34c_flameWarp;
  EFlameState x3f0_flameState = EFlameState::Default;
  CAssetId x3f4_playerSteamTxtr;
  s16 x3f8_playerHitSfx;
  CAssetId x3fc_playerIceTxtr;

  union {
    struct {
      bool x400_24_active : 1;
      bool x400_25_particlesActive : 1;
      bool x400_26_ : 1;
      bool x400_27_coneCollision : 1; /* Z-sort and finer collision detection */
    };
    u32 _dummy = 0;
  };

  void CreateFlameParticles(CStateManager&);
  void SetFlameLightActive(CStateManager&, bool);
  void UpdateFlameState(float, CStateManager&);
  CRayCastResult DoCollisionCheck(TUniqueId& idOut, const zeus::CAABox& aabb, CStateManager& mgr);
  void ApplyDamageToActor(CStateManager& mgr, TUniqueId id, float dt);
public:
  CFlameThrower(const TToken<CWeaponDescription>& wDesc, std::string_view name, EWeaponType wType,
                const CFlameInfo& flameInfo, const zeus::CTransform& xf, EMaterialTypes matType,
                const CDamageInfo& dInfo, TUniqueId uid, TAreaId aId, TUniqueId owner, EProjectileAttrib attribs,
                CAssetId playerSteamTxtr, s16 playerHitSfx, CAssetId playerIceTxtr);

  void Accept(IVisitor& visitor);
  void AcceptScriptMsg(EScriptObjectMessage, TUniqueId, CStateManager&);
  void Think(float, CStateManager&);
  void AddToRenderer(const zeus::CFrustum&, const CStateManager&) const;
  void SetTransform(const zeus::CTransform& xf, float);
  void Reset(CStateManager&, bool);
  void Fire(const zeus::CTransform&, CStateManager&, bool);
  bool GetParticlesActive() const { return x400_25_particlesActive; }
};
} // namespace urde
