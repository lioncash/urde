#pragma once

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec::DNAMP1 {
struct ThardusRockProjectile : IScriptObject {
  AT_DECL_DNA_YAML
  AT_DECL_DNAV
  String<-1> name;
  Value<atVec3f> location;
  Value<atVec3f> orientation;
  Value<atVec3f> scale;
  PatternedInfo patternedInfo;
  ActorParameters actorParameters;
  Value<bool> unknown1;
  Value<bool> unknown2;
  Value<float> unknown3;
  UniqueID32 model;
  UniqueID32 stateMachine;

  void addCMDLRigPairs(PAKRouter<PAKBridge>& pakRouter, CharacterAssociations<UniqueID32>& charAssoc) const {
    actorParameters.addCMDLRigPairs(pakRouter, charAssoc, patternedInfo.animationParameters);
  }

  void nameIDs(PAKRouter<PAKBridge>& pakRouter) const {
    if (model.isValid()) {
      PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(model);
      ent->name = name + "_model";
    }
    if (stateMachine.isValid()) {
      PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(stateMachine);
      ent->name = name + "_fsm";
    }
    patternedInfo.nameIDs(pakRouter, name + "_patterned");
    actorParameters.nameIDs(pakRouter, name + "_actp");
  }

  void gatherDependencies(std::vector<hecl::ProjectPath>& pathsOut, std::vector<hecl::ProjectPath>& lazyOut) const {
    g_curSpec->flattenDependencies(model, pathsOut);
    g_curSpec->flattenDependencies(stateMachine, pathsOut);
    patternedInfo.depIDs(pathsOut);
    actorParameters.depIDs(pathsOut, lazyOut);
  }

  void gatherScans(std::vector<Scan>& scansOut) const { actorParameters.scanIDs(scansOut); }
};
} // namespace DataSpec::DNAMP1
