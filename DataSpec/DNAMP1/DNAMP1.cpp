#include <cstdio>

#define NOD_ATHENA 1
#include "DNAMP1.hpp"
#include "STRG.hpp"
#include "SCAN.hpp"
#include "MLVL.hpp"
#include "DataSpec/DNACommon/TXTR.hpp"
#include "DataSpec/DNACommon/PART.hpp"
#include "DataSpec/DNACommon/ELSC.hpp"
#include "DataSpec/DNACommon/SWHC.hpp"
#include "DataSpec/DNACommon/CRSC.hpp"
#include "DataSpec/DNACommon/WPSC.hpp"
#include "DataSpec/DNACommon/DPSC.hpp"
#include "DataSpec/DNACommon/FONT.hpp"
#include "DataSpec/DNACommon/DGRP.hpp"
#include "DataSpec/DNACommon/ATBL.hpp"
#include "HINT.hpp"
#include "CMDL.hpp"
#include "AFSM.hpp"
#include "SAVW.hpp"
#include "ANCS.hpp"
#include "MREA.hpp"
#include "MAPA.hpp"
#include "MAPU.hpp"
#include "FRME.hpp"
#include "AGSC.hpp"
#include "CSNG.hpp"
#include "DCLN.hpp"
#include "PATH.hpp"

#include "DataSpec/DNACommon/Tweaks/TweakWriter.hpp"
#include "Tweaks/CTweakPlayerRes.hpp"
#include "Tweaks/CTweakGunRes.hpp"
#include "Tweaks/CTweakPlayer.hpp"
#include "Tweaks/CTweakCameraBob.hpp"
#include "Tweaks/CTweakSlideShow.hpp"
#include "Tweaks/CTweakGame.hpp"
#include "Tweaks/CTweakTargeting.hpp"
#include "Tweaks/CTweakAutoMapper.hpp"
#include "Tweaks/CTweakGui.hpp"
#include "Tweaks/CTweakPlayerControl.hpp"
#include "Tweaks/CTweakBall.hpp"
#include "Tweaks/CTweakParticle.hpp"
#include "Tweaks/CTweakGuiColors.hpp"
#include "Tweaks/CTweakPlayerGun.hpp"
#include "MazeSeeds.hpp"
#include "SnowForces.hpp"

namespace DataSpec::DNAMP1 {
logvisor::Module Log("urde::DNAMP1");

static bool GetNoShare(std::string_view name) {
  std::string lowerName(name);
  std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), tolower);
  if (!lowerName.compare(0, 7, "metroid"))
    return false;
  return true;
}

PAKBridge::PAKBridge(const nod::Node& node, bool doExtract)
: m_node(node), m_pak(false, GetNoShare(node.getName())), m_doExtract(doExtract) {
  nod::AthenaPartReadStream rs(node.beginReadStream());
  m_pak.read(rs);

  /* Append Level String */
  for (auto& ent : m_pak.m_entries) {
    PAK::Entry& entry = ent.second;
    if (entry.type == FOURCC('MLVL')) {
      PAKEntryReadStream rs = entry.beginReadStream(m_node);
      MLVL mlvl;
      mlvl.read(rs);
      PAK::Entry* nameEnt = (PAK::Entry*)m_pak.lookupEntry(mlvl.worldNameId);
      if (nameEnt) {
        nameEnt->name = entry.name + "_name";
        PAKEntryReadStream rs = nameEnt->beginReadStream(m_node);
        STRG mlvlName;
        mlvlName.read(rs);
        if (m_levelString.size())
          m_levelString += _SYS_STR(", ");
        m_levelString += mlvlName.getSystemString(FOURCC('ENGL'), 0);
      }
    }
  }
}

static hecl::SystemString LayerName(std::string_view name) {
  hecl::SystemString ret(hecl::SystemStringConv(name).sys_str());
  for (auto& ch : ret)
    if (ch == _SYS_STR('/') || ch == _SYS_STR('\\'))
      ch = _SYS_STR('-');
  return ret;
}

void PAKBridge::build() {
  /* First pass: build per-area/per-layer dependency map */
  for (const auto& ent : m_pak.m_entries) {
    const PAK::Entry& entry = ent.second;
    if (entry.type == FOURCC('MLVL')) {
      Level& level = m_levelDeps[entry.id];

      MLVL mlvl;
      {
        PAKEntryReadStream rs = entry.beginReadStream(m_node);
        mlvl.read(rs);
      }
      bool named;
      std::string bestName = m_pak.bestEntryName(m_node, entry, named);
      level.name = hecl::SystemStringConv(bestName).sys_str();
      level.areas.reserve(mlvl.areaCount);
      unsigned layerIdx = 0;

      /* Make MAPW available to lookup MAPAs */
      PAK::Entry* worldMapEnt = (PAK::Entry*)m_pak.lookupEntry(mlvl.worldMap);
      std::vector<UniqueID32> mapw;
      if (worldMapEnt) {
        worldMapEnt->name = entry.name + "_mapw";
        PAKEntryReadStream rs = worldMapEnt->beginReadStream(m_node);
        rs.seek(8, athena::Current);
        atUint32 areaCount = rs.readUint32Big();
        mapw.reserve(areaCount);
        for (atUint32 i = 0; i < areaCount; ++i)
          mapw.emplace_back(rs);
      }

      PAK::Entry* savwEnt = (PAK::Entry*)m_pak.lookupEntry(mlvl.saveWorldId);
      if (savwEnt)
        savwEnt->name = entry.name + "_savw";

      PAK::Entry* skyEnt = (PAK::Entry*)m_pak.lookupEntry(mlvl.worldSkyboxId);
      if (skyEnt)
        skyEnt->name = entry.name + "_skybox";

      /* Index areas */
      unsigned ai = 0;
      for (const MLVL::Area& area : mlvl.areas) {
        Level::Area& areaDeps = level.areas[area.areaMREAId];
        MLVL::LayerFlags& layerFlags = mlvl.layerFlags[ai];
        PAK::Entry* areaNameEnt = (PAK::Entry*)m_pak.lookupEntry(area.areaNameId);
        if (areaNameEnt) {
          STRG areaName;
          {
            PAKEntryReadStream rs = areaNameEnt->beginReadStream(m_node);
            areaName.read(rs);
          }
          areaDeps.name = areaName.getSystemString(FOURCC('ENGL'), 0);
          areaDeps.name = hecl::StringUtils::TrimWhitespace(areaDeps.name);
        }
        if (areaDeps.name.empty()) {
          std::string idStr = area.areaMREAId.toString();
          areaDeps.name = hecl::SystemString(_SYS_STR("MREA_")) + hecl::SystemStringConv(idStr).c_str();
        }
        hecl::SystemString num = fmt::format(fmt(_SYS_STR("{:02d} ")), ai);
        areaDeps.name = num + areaDeps.name;

        std::string lowerName(hecl::SystemUTF8Conv(areaDeps.name).str());
        for (char& ch : lowerName) {
          ch = tolower(ch);
          if (ch == ' ')
            ch = '_';
        }
        if (areaNameEnt)
          areaNameEnt->name = lowerName + "_name";
        PAK::Entry* areaEnt = (PAK::Entry*)m_pak.lookupEntry(area.areaMREAId);
        if (areaEnt)
          areaEnt->name = lowerName;

        areaDeps.layers.reserve(area.depLayerCount - 1);
        unsigned r = 0;
        for (unsigned l = 1; l < area.depLayerCount; ++l) {
          areaDeps.layers.emplace_back();
          Level::Area::Layer& layer = areaDeps.layers.back();
          layer.name = LayerName(mlvl.layerNames[layerIdx++]);
          layer.active = layerFlags.flags >> (l - 1) & 0x1;
          layer.name = hecl::StringUtils::TrimWhitespace(layer.name);

          num = fmt::format(fmt(_SYS_STR("{:02d} ")), l - 1);
          layer.name = num + layer.name;

          layer.resources.reserve(area.depLayers[l] - r);
          for (; r < area.depLayers[l]; ++r)
            layer.resources.emplace(area.deps[r].id);
        }
        areaDeps.resources.reserve(area.depCount - r + 2);
        for (; r < area.depCount; ++r)
          areaDeps.resources.emplace(area.deps[r].id);
        areaDeps.resources.emplace(area.areaMREAId);
        if (mapw.size() > ai)
          areaDeps.resources.emplace(mapw[ai]);
        ++ai;
      }
    }
  }

  /* Second pass: cross-compare uniqueness */
  for (auto& entry : m_pak.m_entries) {
    entry.second.unique.checkEntry(*this, entry.second);
  }
}

void PAKBridge::addCMDLRigPairs(PAKRouter<PAKBridge>& pakRouter, CharacterAssociations<UniqueID32>& charAssoc) const {
  for (const auto& [uniqueID, entry] : m_pak.m_entries) {
    if (entry.type == FOURCC('ANCS')) {
      PAKEntryReadStream rs = entry.beginReadStream(m_node);
      ANCS ancs;
      ancs.read(rs);
      for (const ANCS::CharacterSet::CharacterInfo& ci : ancs.characterSet.characters) {
        charAssoc.m_cmdlRigs[ci.cmdl] = std::make_pair(ci.cskr, ci.cinf);
        charAssoc.m_cskrCinfToCharacter[ci.cskr] = std::make_pair(entry.id, fmt::format(fmt("{}.CSKR"), ci.name));
        charAssoc.m_cskrCinfToCharacter[ci.cinf] = std::make_pair(entry.id, fmt::format(fmt("CINF_{}.CINF"), ci.cinf));
        PAK::Entry* cmdlEnt = (PAK::Entry*)m_pak.lookupEntry(ci.cmdl);
        PAK::Entry* cskrEnt = (PAK::Entry*)m_pak.lookupEntry(ci.cskr);
        PAK::Entry* cinfEnt = (PAK::Entry*)m_pak.lookupEntry(ci.cinf);
        cmdlEnt->name = fmt::format(fmt("ANCS_{}_{}_model"), uniqueID, ci.name);
        cskrEnt->name = fmt::format(fmt("ANCS_{}_{}_skin"), uniqueID, ci.name);
        cinfEnt->name = fmt::format(fmt("ANCS_{}_{}_skel"), uniqueID, ci.name);
        if (ci.cmdlIce.isValid() && ci.cskrIce.isValid()) {
          charAssoc.m_cmdlRigs[ci.cmdlIce] = std::make_pair(ci.cskrIce, ci.cinf);
          charAssoc.m_cskrCinfToCharacter[ci.cskrIce] =
              std::make_pair(entry.id, fmt::format(fmt("{}.ICE.CSKR"), ci.name));
          PAK::Entry* cmdlEnt = (PAK::Entry*)m_pak.lookupEntry(ci.cmdlIce);
          PAK::Entry* cskrEnt = (PAK::Entry*)m_pak.lookupEntry(ci.cskrIce);
          cmdlEnt->name = fmt::format(fmt("ANCS_{}_{}_icemodel"), uniqueID, ci.name);
          cskrEnt->name = fmt::format(fmt("ANCS_{}_{}_iceskin"), uniqueID, ci.name);
        }
      }
      std::map<atUint32, DNAANCS::AnimationResInfo<UniqueID32>> animInfo;
      ancs.getAnimationResInfo(&pakRouter, animInfo);
      for (auto& ae : animInfo) {
        PAK::Entry* animEnt = (PAK::Entry*)m_pak.lookupEntry(ae.second.animId);
        animEnt->name = fmt::format(fmt("ANCS_{}_{}"), uniqueID, ae.second.name);
        charAssoc.m_cskrCinfToCharacter[ae.second.animId] =
            std::make_pair(entry.id, fmt::format(fmt("{}.ANIM"), ae.second.name));
        if (ae.second.evntId.isValid()) {
          PAK::Entry* evntEnt = (PAK::Entry*)m_pak.lookupEntry(ae.second.evntId);
          evntEnt->name = fmt::format(fmt("ANCS_{}_{}_evnt"), uniqueID, ae.second.name);
          charAssoc.m_cskrCinfToCharacter[ae.second.evntId] =
              std::make_pair(entry.id, fmt::format(fmt("{}.evnt.yaml"), ae.second.name));
        }
      }
    } else if (entry.type == FOURCC('MREA')) {
      PAKEntryReadStream rs = entry.beginReadStream(m_node);
      MREA::AddCMDLRigPairs(rs, pakRouter, charAssoc);
    }
  }
}

void PAKBridge::addPATHToMREA(PAKRouter<PAKBridge>& pakRouter,
                              std::unordered_map<UniqueID32, UniqueID32>& pathToMrea) const {
  for (const auto& [uniqueID, entry] : m_pak.m_entries) {
    if (entry.type == FOURCC('MREA')) {
      PAKEntryReadStream rs = entry.beginReadStream(m_node);
      UniqueID32 pathID = MREA::GetPATHId(rs);
      if (pathID.isValid())
        pathToMrea.insert_or_assign(pathID, uniqueID);
    }
  }
}

static const atVec4f BottomRow = {{0.f, 0.f, 0.f, 1.f}};

void PAKBridge::addMAPATransforms(PAKRouter<PAKBridge>& pakRouter,
                                  std::unordered_map<UniqueID32, zeus::CMatrix4f>& addTo,
                                  std::unordered_map<UniqueID32, hecl::ProjectPath>& pathOverrides) const {
  for (const auto& entry : m_pak.m_entries) {
    if (entry.second.type == FOURCC('MLVL')) {
      MLVL mlvl;
      {
        PAKEntryReadStream rs = entry.second.beginReadStream(m_node);
        mlvl.read(rs);
      }
      hecl::ProjectPath mlvlDirPath = pakRouter.getWorking(&entry.second).getParentPath();

      if (mlvl.worldNameId.isValid())
        pathOverrides[mlvl.worldNameId] = hecl::ProjectPath(mlvlDirPath, _SYS_STR("!name.yaml"));

      for (const MLVL::Area& area : mlvl.areas) {
        {
          /* Get PATH transform */
          const nod::Node* areaNode;
          const PAK::Entry* areaEntry = pakRouter.lookupEntry(area.areaMREAId, &areaNode);
          PAKEntryReadStream rs = areaEntry->beginReadStream(*areaNode);
          UniqueID32 pathId = MREA::GetPATHId(rs);
          if (pathId.isValid())
            addTo[pathId] = zeus::CMatrix4f(area.transformMtx[0], area.transformMtx[1], area.transformMtx[2], BottomRow)
                                .transposed();
        }

        hecl::ProjectPath areaDirPath = pakRouter.getWorking(area.areaMREAId).getParentPath();
        if (area.areaNameId.isValid())
          pathOverrides[area.areaNameId] = hecl::ProjectPath(areaDirPath, _SYS_STR("!name.yaml"));
      }

      if (mlvl.worldMap.isValid()) {
        const nod::Node* mapNode;
        const PAK::Entry* mapEntry = pakRouter.lookupEntry(mlvl.worldMap, &mapNode);
        if (mapEntry) {
          PAKEntryReadStream rs = mapEntry->beginReadStream(*mapNode);
          u32 magic = rs.readUint32Big();
          if (magic == 0xDEADF00D) {
            rs.readUint32Big();
            u32 count = rs.readUint32Big();
            for (u32 i = 0; i < count && i < mlvl.areas.size(); ++i) {
              MLVL::Area& areaData = mlvl.areas[i];
              UniqueID32 mapaId;
              mapaId.read(rs);
              addTo[mapaId] = zeus::CMatrix4f(areaData.transformMtx[0], areaData.transformMtx[1],
                                              areaData.transformMtx[2], BottomRow)
                                  .transposed();
            }
          }
        }
      }
    }
  }
}

ResExtractor<PAKBridge> PAKBridge::LookupExtractor(const nod::Node& pakNode, const PAK& pak, const PAK::Entry& entry) {
  switch (entry.type.toUint32()) {
  case SBIG('STRG'):
    return {STRG::Extract, {_SYS_STR(".yaml")}};
  case SBIG('SCAN'):
    return {SCAN::Extract, {_SYS_STR(".yaml")}, 0, SCAN::Name};
  case SBIG('HINT'):
    return {HINT::Extract, {_SYS_STR(".yaml")}};
  case SBIG('TXTR'):
    return {TXTR::Extract, {_SYS_STR(".png")}};
  case SBIG('AFSM'):
    return {AFSM::Extract, {_SYS_STR(".yaml")}};
  case SBIG('FRME'):
    return {FRME::Extract, {_SYS_STR(".blend")}, 2};
  case SBIG('CMDL'):
    return {CMDL::Extract, {_SYS_STR(".blend")}, 1, CMDL::Name};
  case SBIG('DCLN'):
    return {DCLN::Extract, {_SYS_STR(".blend")}};
  case SBIG('ANCS'):
    return {ANCS::Extract, {_SYS_STR(".yaml"), _SYS_STR(".blend")}, 2};
  case SBIG('MLVL'):
    return {MLVL::Extract, {_SYS_STR(".yaml"), _SYS_STR(".blend")}, 3};
  case SBIG('MREA'):
    return {MREA::Extract, {_SYS_STR(".blend")}, 4, MREA::Name};
  case SBIG('MAPA'):
    return {MAPA::Extract, {_SYS_STR(".blend")}, 4};
  case SBIG('MAPU'):
    return {MAPU::Extract, {_SYS_STR(".blend")}, 5};
  case SBIG('PATH'):
    return {PATH::Extract, {_SYS_STR(".blend")}, 5};
  case SBIG('PART'):
    return {DNAParticle::ExtractGPSM<UniqueID32>, {_SYS_STR(".gpsm.yaml")}};
  case SBIG('ELSC'):
    return {DNAParticle::ExtractELSM<UniqueID32>, {_SYS_STR(".elsm.yaml")}};
  case SBIG('SWHC'):
    return {DNAParticle::ExtractSWSH<UniqueID32>, {_SYS_STR(".swsh.yaml")}};
  case SBIG('CRSC'):
    return {DNAParticle::ExtractCRSM<UniqueID32>, {_SYS_STR(".crsm.yaml")}};
  case SBIG('WPSC'):
    return {DNAParticle::ExtractWPSM<UniqueID32>, {_SYS_STR(".wpsm.yaml")}};
  case SBIG('DPSC'):
    return {DNAParticle::ExtractDPSM<UniqueID32>, {_SYS_STR(".dpsm.yaml")}};
  case SBIG('FONT'):
    return {DNAFont::ExtractFONT<UniqueID32>, {_SYS_STR(".yaml")}};
  case SBIG('DGRP'):
    return {DNADGRP::ExtractDGRP<UniqueID32>, {_SYS_STR(".yaml")}};
  case SBIG('AGSC'):
    return {AGSC::Extract, {}};
  case SBIG('CSNG'):
    return {CSNG::Extract, {_SYS_STR(".mid"), _SYS_STR(".yaml")}};
  case SBIG('ATBL'):
    return {DNAAudio::ATBL::Extract, {_SYS_STR(".yaml")}};
  case SBIG('CTWK'):
  case SBIG('DUMB'): {
    bool named;
    std::string name = pak.bestEntryName(pakNode, entry, named);
    if (named) {
      if (!name.compare("PlayerRes"))
        return {ExtractTweak<CTweakPlayerRes>, {_SYS_STR(".yaml")}};
      if (!name.compare("GunRes"))
        return {ExtractTweak<CTweakGunRes>, {_SYS_STR(".yaml")}};
      if (!name.compare("Player"))
        return {ExtractTweak<CTweakPlayer>, {_SYS_STR(".yaml")}};
      if (!name.compare("CameraBob"))
        return {ExtractTweak<CTweakCameraBob>, {_SYS_STR(".yaml")}};
      if (!name.compare("SlideShow"))
        return {ExtractTweak<CTweakSlideShow>, {_SYS_STR(".yaml")}};
      if (!name.compare("Game"))
        return {ExtractTweak<CTweakGame>, {_SYS_STR(".yaml")}};
      if (!name.compare("Targeting"))
        return {ExtractTweak<CTweakTargeting>, {_SYS_STR(".yaml")}};
      if (!name.compare("Gui"))
        return {ExtractTweak<CTweakGui>, {_SYS_STR(".yaml")}};
      if (!name.compare("AutoMapper"))
        return {ExtractTweak<CTweakAutoMapper>, {_SYS_STR(".yaml")}};
      if (!name.compare("PlayerControls") || !name.compare("PlayerControls2"))
        return {ExtractTweak<CTweakPlayerControl>, {_SYS_STR(".yaml")}};
      if (!name.compare("Ball"))
        return {ExtractTweak<CTweakBall>, {_SYS_STR(".yaml")}};
      if (!name.compare("Particle"))
        return {ExtractTweak<CTweakParticle>, {_SYS_STR(".yaml")}};
      if (!name.compare("GuiColors"))
        return {ExtractTweak<CTweakGuiColors>, {_SYS_STR(".yaml")}};
      if (!name.compare("PlayerGun"))
        return {ExtractTweak<CTweakPlayerGun>, {_SYS_STR(".yaml")}};
      if (!name.compare("DUMB_MazeSeeds"))
        return {ExtractTweak<MazeSeeds>, {_SYS_STR(".yaml")}};
      if (!name.compare("DUMB_SnowForces"))
        return {ExtractTweak<SnowForces>, {_SYS_STR(".yaml")}};
    }
    break;
  }
  }
  return {};
}

} // namespace DataSpec::DNAMP1
