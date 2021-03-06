#pragma once

#include "World/CPatterned.hpp"
#include "Character/CBoneTracking.hpp"
#include "Weapon/CProjectileInfo.hpp"

namespace urde::MP1 {
class CEyeball : public CPatterned {
  static constexpr std::string_view skEyeLocator = "Laser_LCTR"sv;
  float x568_attackDelay;
  float x56c_attackStartTime;
  CBoneTracking x570_boneTracking;
  zeus::CVector3f x5a8_targetPosition;
  CProjectileInfo x5b4_projectileInfo;
  CAssetId x5dc_beamContactFxId;
  CAssetId x5e0_beamPulseFxId;
  CAssetId x5e4_beamTextureId;
  CAssetId x5e8_beamGlowTextureId;
  TUniqueId x5ec_projectileId = kInvalidUniqueId;
  u32 x5f0_currentAnim = 0;
  s32 x5f4_animIdxs[4];
  u16 x604_beamSfxId;
  CSfxHandle x608_beamSfx = 0;
  bool x60c_24_canAttack : 1;
  bool x60c_25_playerInRange : 1;
  bool x60c_26_alert : 1;
  bool x60c_27_attackDisabled : 1;
  bool x60c_28_firingBeam : 1;

  void CreateBeam(CStateManager&);
  void FireBeam(CStateManager&, const zeus::CTransform&);
  void TryFlinch(CStateManager&, int);
  void UpdateAnimation();
  void ResetBeamState(CStateManager&);

public:
  DEFINE_PATTERNED(EyeBall)

  CEyeball(TUniqueId uid, std::string_view name, CPatterned::EFlavorType flavor, const CEntityInfo& info,
           const zeus::CTransform& xf, CModelData&& mData, const CPatternedInfo& pInfo, float attackDelay,
           float attackStartTime, CAssetId wpscId, const CDamageInfo& dInfo, CAssetId beamContactFxId,
           CAssetId beamPulseFxId, CAssetId beamTextureId, CAssetId beamGlowTextureId, u32 anim0, u32 anim1, u32 anim2,
           u32 anim3, u32 beamSfx, bool attackDisabled, const CActorParameters& actParms);

  void Accept(IVisitor& visitor);
  void PreRender(CStateManager&, const zeus::CFrustum&);
  void Touch(CActor&, CStateManager&){};
  void Death(CStateManager&, const zeus::CVector3f&, EScriptObjectState);

  void AcceptScriptMsg(EScriptObjectMessage msg, TUniqueId uid, CStateManager& mgr);
  void DoUserAnimEvent(CStateManager&, const CInt32POINode&, EUserEventType, float);
  void Think(float, CStateManager&);
  void Flinch(CStateManager&, EStateMsg, float);
  void Active(CStateManager&, EStateMsg, float);
  void InActive(CStateManager&, EStateMsg, float);

  void Cover(CStateManager&, EStateMsg, float);

  bool ShouldAttack(CStateManager&, float) { return x60c_26_alert; }
  bool ShouldFire(CStateManager&, float) { return !x60c_27_attackDisabled; }
};
} // namespace urde::MP1