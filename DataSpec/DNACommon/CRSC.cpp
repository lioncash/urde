#include "CRSC.hpp"

namespace DataSpec::DNAParticle {
static const std::vector<FourCC> GeneratorTypes = {
    SBIG('NODP'), SBIG('DEFS'), SBIG('CRTS'), SBIG('MTLS'), SBIG('GRAS'), SBIG('ICEE'), SBIG('GOOO'), SBIG('WODS'),
    SBIG('WATR'), SBIG('1MUD'), SBIG('1LAV'), SBIG('1SAN'), SBIG('1PRJ'), SBIG('DCHR'), SBIG('DCHS'), SBIG('DCSH'),
    SBIG('DENM'), SBIG('DESP'), SBIG('DESH'), SBIG('BTLE'), SBIG('WASP'), SBIG('TALP'), SBIG('PTGM'), SBIG('SPIR'),
    SBIG('FPIR'), SBIG('FFLE'), SBIG('PARA'), SBIG('BMON'), SBIG('BFLR'), SBIG('PBOS'), SBIG('IBOS'), SBIG('1SVA'),
    SBIG('1RPR'), SBIG('1MTR'), SBIG('1PDS'), SBIG('1FLB'), SBIG('1DRN'), SBIG('1MRE'), SBIG('CHOZ'), SBIG('JZAP'),
    SBIG('1ISE'), SBIG('1BSE'), SBIG('1ATB'), SBIG('1ATA'), SBIG('BTSP'), SBIG('WWSP'), SBIG('TASP'), SBIG('TGSP'),
    SBIG('SPSP'), SBIG('FPSP'), SBIG('FFSP'), SBIG('PSSP'), SBIG('BMSP'), SBIG('BFSP'), SBIG('PBSP'), SBIG('IBSP'),
    SBIG('2SVA'), SBIG('2RPR'), SBIG('2MTR'), SBIG('2PDS'), SBIG('2FLB'), SBIG('2DRN'), SBIG('2MRE'), SBIG('CHSP'),
    SBIG('JZSP'), SBIG('3ISE'), SBIG('3BSE'), SBIG('3ATB'), SBIG('3ATA'), SBIG('BTSH'), SBIG('WWSH'), SBIG('TASH'),
    SBIG('TGSH'), SBIG('SPSH'), SBIG('FPSH'), SBIG('FFSH'), SBIG('PSSH'), SBIG('BMSH'), SBIG('BFSH'), SBIG('PBSH'),
    SBIG('IBSH'), SBIG('3SVA'), SBIG('3RPR'), SBIG('3MTR'), SBIG('3PDS'), SBIG('3FLB'), SBIG('3DRN'), SBIG('3MRE'),
    SBIG('CHSH'), SBIG('JZSH'), SBIG('5ISE'), SBIG('5BSE'), SBIG('5ATB'), SBIG('5ATA')};

static const std::vector<FourCC> SFXTypes = {
    SBIG('DSFX'), SBIG('CSFX'), SBIG('MSFX'), SBIG('GRFX'), SBIG('NSFX'), SBIG('DSFX'), SBIG('CSFX'), SBIG('MSFX'),
    SBIG('GRFX'), SBIG('ICFX'), SBIG('GOFX'), SBIG('WSFX'), SBIG('WTFX'), SBIG('2MUD'), SBIG('2LAV'), SBIG('2SAN'),
    SBIG('2PRJ'), SBIG('DCFX'), SBIG('DSFX'), SBIG('DSHX'), SBIG('DEFX'), SBIG('ESFX'), SBIG('SHFX'), SBIG('BEFX'),
    SBIG('WWFX'), SBIG('TAFX'), SBIG('GTFX'), SBIG('SPFX'), SBIG('FPFX'), SBIG('FFFX'), SBIG('PAFX'), SBIG('BMFX'),
    SBIG('BFFX'), SBIG('PBFX'), SBIG('IBFX'), SBIG('4SVA'), SBIG('4RPR'), SBIG('4MTR'), SBIG('4PDS'), SBIG('4FLB'),
    SBIG('4DRN'), SBIG('4MRE'), SBIG('CZFX'), SBIG('JZAS'), SBIG('2ISE'), SBIG('2BSE'), SBIG('2ATB'), SBIG('2ATA'),
    SBIG('BSFX'), SBIG('WSFX'), SBIG('TSFX'), SBIG('GSFX'), SBIG('SSFX'), SBIG('FSFX'), SBIG('SFFX'), SBIG('PSFX'),
    SBIG('MSFX'), SBIG('SBFX'), SBIG('PBSX'), SBIG('IBSX'), SBIG('5SVA'), SBIG('5RPR'), SBIG('5MTR'), SBIG('5PDS'),
    SBIG('5FLB'), SBIG('5DRN'), SBIG('5MRE'), SBIG('CSFX'), SBIG('JZPS'), SBIG('4ISE'), SBIG('4BSE'), SBIG('4ATB'),
    SBIG('4ATA'), SBIG('BHFX'), SBIG('WHFX'), SBIG('THFX'), SBIG('GHFX'), SBIG('SHFX'), SBIG('FHFX'), SBIG('HFFX'),
    SBIG('PHFX'), SBIG('MHFX'), SBIG('HBFX'), SBIG('PBHX'), SBIG('IBHX'), SBIG('6SVA'), SBIG('6RPR'), SBIG('6MTR'),
    SBIG('6PDS'), SBIG('6FLB'), SBIG('6DRN'), SBIG('6MRE'), SBIG('CHFX'), SBIG('JZHS'), SBIG('6ISE'), SBIG('6BSE'),
    SBIG('6ATB'), SBIG('6ATA'),
};

static const std::vector<FourCC> DecalTypes = {SBIG('NCDL'), SBIG('DDCL'), SBIG('CODL'), SBIG('MEDL'), SBIG('GRDL'),
                                               SBIG('ICDL'), SBIG('GODL'), SBIG('WODL'), SBIG('WTDL'), SBIG('3MUD'),
                                               SBIG('3LAV'), SBIG('3SAN'), SBIG('CHDL'), SBIG('ENDL')};

template <>
const char* CRSM<UniqueID32>::DNAType() {
  return "CRSM<UniqueID32>";
}

template <>
const char* CRSM<UniqueID64>::DNAType() {
  return "CRSM<UniqueID64>";
}

template <class IDType>
void CRSM<IDType>::_read(athena::io::YAMLDocReader& r) {
  for (const auto& elem : r.getCurNode()->m_mapChildren) {
    if (elem.first.size() < 4) {
      LogModule.report(logvisor::Warning, fmt("short FourCC in element '{}'"), elem.first);
      continue;
    }

    if (auto rec = r.enterSubRecord(elem.first.c_str())) {
      FourCC clsId(elem.first.c_str());
      auto gen = std::find_if(GeneratorTypes.begin(), GeneratorTypes.end(),
                              [&clsId](const FourCC& other) -> bool { return clsId == other; });
      if (gen != GeneratorTypes.end()) {
        x0_generators[clsId].read(r);
        continue;
      }

      auto sfx = std::find_if(SFXTypes.begin(), SFXTypes.end(),
                              [&clsId](const FourCC& other) -> bool { return clsId == other; });
      if (sfx != SFXTypes.end()) {
        x10_sfx[clsId] = r.readInt32(clsId.toString().c_str());
        continue;
      }

      auto decal = std::find_if(DecalTypes.begin(), DecalTypes.end(),
                                [&clsId](const FourCC& other) -> bool { return clsId == other; });
      if (decal != DecalTypes.end()) {
        x20_decals[clsId].read(r);
        continue;
      }
      if (clsId == SBIG('RNGE'))
        x30_RNGE = r.readFloat(nullptr);
      else if (clsId == SBIG('FOFF'))
        x34_FOFF = r.readFloat(nullptr);
    }
  }
}

template <class IDType>
void CRSM<IDType>::_write(athena::io::YAMLDocWriter& w) const {
  for (const auto& pair : x0_generators)
    if (pair.second)
      if (auto rec = w.enterSubRecord(pair.first.toString().c_str()))
        pair.second.write(w);

  for (const auto& pair : x10_sfx)
    if (pair.second != UINT32_MAX)
      w.writeUint32(pair.first.toString().c_str(), pair.second);

  for (const auto& pair : x20_decals)
    if (pair.second)
      if (auto rec = w.enterSubRecord(pair.first.toString().c_str()))
        pair.second.write(w);

  if (x30_RNGE != 50.f)
    w.writeFloat("RNGE", x30_RNGE);
  if (x34_FOFF != 0.2f)
    w.writeFloat("FOFF", x34_FOFF);
}

template <class IDType>
void CRSM<IDType>::_binarySize(size_t& __isz) const {
  __isz += 4;
  for (const auto& pair : x0_generators) {
    if (pair.second) {
      __isz += 4;
      pair.second.binarySize(__isz);
    }
  }
  for (const auto& pair : x10_sfx) {
    if (pair.second != UINT32_MAX)
      __isz += 12;
  }

  for (const auto& pair : x20_decals) {
    if (pair.second) {
      __isz += 4;
      pair.second.binarySize(__isz);
    }
  }

  if (x30_RNGE != 50.f)
    __isz += 12;
  if (x34_FOFF != 0.2f)
    __isz += 12;
}

template <class IDType>
void CRSM<IDType>::_read(athena::io::IStreamReader& r) {
  DNAFourCC clsId;
  clsId.read(r);
  if (clsId != SBIG('CRSM')) {
    LogModule.report(logvisor::Warning, fmt("non CRSM provided to CRSM parser"));
    return;
  }

  while (clsId != SBIG('_END')) {
    clsId.read(r);
    auto gen = std::find_if(GeneratorTypes.begin(), GeneratorTypes.end(),
                            [&clsId](const FourCC& other) -> bool { return clsId == other; });
    if (gen != GeneratorTypes.end()) {
      x0_generators[clsId].read(r);
      continue;
    }

    auto sfx = std::find_if(SFXTypes.begin(), SFXTypes.end(),
                            [&clsId](const FourCC& other) -> bool { return clsId == other; });
    if (sfx != SFXTypes.end()) {
      DNAFourCC fcc;
      fcc.read(r);
      if (fcc != SBIG('NONE'))
        x10_sfx[clsId] = r.readInt32Big();
      else
        x10_sfx[clsId] = ~0;
      continue;
    }

    auto decal = std::find_if(DecalTypes.begin(), DecalTypes.end(),
                              [&clsId](const FourCC& other) -> bool { return clsId == other; });
    if (decal != DecalTypes.end()) {
      x20_decals[clsId].read(r);
      continue;
    }
    if (clsId == SBIG('RNGE')) {
      r.readUint32();
      x30_RNGE = r.readFloatBig();
      continue;
    }
    if (clsId == SBIG('FOFF')) {
      r.readUint32();
      x34_FOFF = r.readFloatBig();
      continue;
    }
    if (clsId != SBIG('_END'))
      LogModule.report(logvisor::Fatal, fmt("Unknown CRSM class {} @{}"), clsId, r.position());
  }
}

template <class IDType>
void CRSM<IDType>::_write(athena::io::IStreamWriter& w) const {
  w.writeBytes("CRSM", 4);
  for (const auto& pair : x0_generators) {
    w.writeBytes(pair.first.getChars(), 4);
    pair.second.write(w);
  }

  for (const auto& pair : x10_sfx) {
    w.writeBytes(pair.first.getChars(), 4);
    if (pair.second != UINT32_MAX) {
      w.writeBytes("CNST", 4);
      w.writeUint32Big(pair.second);
    } else {
      w.writeBytes("NONE", 4);
    }
  }

  for (const auto& pair : x20_decals) {
    w.writeBytes(pair.first.getChars(), 4);
    pair.second.write(w);
  }

  if (x30_RNGE != 50.f) {
    w.writeBytes("RNGECNST", 8);
    w.writeFloatBig(x30_RNGE);
  }
  if (x34_FOFF != 0.2f) {
    w.writeBytes("FOFFCNST", 8);
    w.writeFloatBig(x34_FOFF);
  }
  w.writeBytes("_END", 4);
}

AT_SUBSPECIALIZE_DNA_YAML(CRSM<UniqueID32>)
AT_SUBSPECIALIZE_DNA_YAML(CRSM<UniqueID64>)

template <class IDType>
void CRSM<IDType>::gatherDependencies(std::vector<hecl::ProjectPath>& pathsOut) const {
  for (const auto& p : x0_generators)
    g_curSpec->flattenDependencies(p.second.id, pathsOut);
  for (const auto& p : x20_decals)
    g_curSpec->flattenDependencies(p.second.id, pathsOut);
}

template <class IDType>
CRSM<IDType>::CRSM() : x30_RNGE(50.f), x34_FOFF(0.2f) {
  for (const auto& sfx : SFXTypes)
    x10_sfx[sfx] = ~0;
}

template struct CRSM<UniqueID32>;
template struct CRSM<UniqueID64>;

template <class IDType>
bool ExtractCRSM(PAKEntryReadStream& rs, const hecl::ProjectPath& outPath) {
  athena::io::FileWriter writer(outPath.getAbsolutePath());
  if (writer.isOpen()) {
    CRSM<IDType> crsm;
    crsm.read(rs);
    athena::io::ToYAMLStream(crsm, writer);
    return true;
  }
  return false;
}
template bool ExtractCRSM<UniqueID32>(PAKEntryReadStream& rs, const hecl::ProjectPath& outPath);
template bool ExtractCRSM<UniqueID64>(PAKEntryReadStream& rs, const hecl::ProjectPath& outPath);

template <class IDType>
bool WriteCRSM(const CRSM<IDType>& crsm, const hecl::ProjectPath& outPath) {
  athena::io::FileWriter w(outPath.getAbsolutePath(), true, false);
  if (w.hasError())
    return false;
  crsm.write(w);
  int64_t rem = w.position() % 32;
  if (rem)
    for (int64_t i = 0; i < 32 - rem; ++i)
      w.writeUByte(0xff);
  return true;
}
template bool WriteCRSM<UniqueID32>(const CRSM<UniqueID32>& crsm, const hecl::ProjectPath& outPath);
template bool WriteCRSM<UniqueID64>(const CRSM<UniqueID64>& crsm, const hecl::ProjectPath& outPath);
} // namespace DataSpec::DNAParticle
