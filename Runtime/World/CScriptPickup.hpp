#pragma once

#include "CPhysicsActor.hpp"
#include "CPlayerState.hpp"

namespace urde {
class CScriptPickup : public CPhysicsActor {
  CPlayerState::EItemType x258_itemType;
  s32 x25c_amount;
  s32 x260_capacity;
  float x264_possibility;
  float x268_fadeInTime;
  float x26c_lifeTime;
  float x270_curTime = 0.f;
  float x274_tractorTime;
  float x278_delayTimer;
  TLockedToken<CGenDescription> x27c_pickupParticleDesc;

  bool x28c_24_generated : 1;
  bool x28c_25_inTractor : 1;
  bool x28c_26_enableTractorTest : 1;

public:
  CScriptPickup(TUniqueId uid, std::string_view name, const CEntityInfo& info, const zeus::CTransform& xf,
                CModelData&& mData, const CActorParameters& aParams, const zeus::CAABox& aabb,
                CPlayerState::EItemType itemType, s32 amount, s32 capacity, CAssetId pickupEffect,
                float possibility, float lifeTime, float fadeInTime, float startDelay, bool active);

  void Accept(IVisitor& visitor);
  void Think(float, CStateManager&);
  void Touch(CActor&, CStateManager&);
  std::optional<zeus::CAABox> GetTouchBounds() const { return CPhysicsActor::GetBoundingBox(); }
  float GetPossibility() const { return x264_possibility; }
  CPlayerState::EItemType GetItem() const { return x258_itemType; }
  void SetGenerated() { x28c_24_generated = true; }
};
} // namespace urde
