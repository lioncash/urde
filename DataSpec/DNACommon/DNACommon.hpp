#pragma once

#include <cstdio>
#include "logvisor/logvisor.hpp"
#include "athena/DNAYaml.hpp"
#include "hecl/Database.hpp"
#include "../SpecBase.hpp"
#include "boo/ThreadLocalPtr.hpp"
#include "zeus/CColor.hpp"

namespace DataSpec {
struct SpecBase;

extern logvisor::Module LogDNACommon;
extern ThreadLocalPtr<SpecBase> g_curSpec;
extern ThreadLocalPtr<class PAKRouterBase> g_PakRouter;
extern ThreadLocalPtr<hecl::blender::Token> g_ThreadBlenderToken;

/* This comes up a great deal */
typedef athena::io::DNA<athena::Big> BigDNA;
typedef athena::io::DNAV<athena::Big> BigDNAV;
typedef athena::io::DNAVYaml<athena::Big> BigDNAVYaml;

/** FourCC with DNA read/write */
using DNAFourCC = hecl::DNAFourCC;

class DNAColor final : public BigDNA, public zeus::CColor {
public:
  DNAColor() = default;
  DNAColor(const zeus::CColor& color) : zeus::CColor(color) {}
  AT_DECL_EXPLICIT_DNA_YAML
};
template <>
inline void DNAColor::Enumerate<BigDNA::Read>(typename Read::StreamT& _r) {
  zeus::CColor::readRGBABig(_r);
}
template <>
inline void DNAColor::Enumerate<BigDNA::Write>(typename Write::StreamT& _w) {
  zeus::CColor::writeRGBABig(_w);
}
template <>
inline void DNAColor::Enumerate<BigDNA::ReadYaml>(typename ReadYaml::StreamT& _r) {
  size_t count;
  if (auto v = _r.enterSubVector(nullptr, count)) {
    zeus::simd_floats f;
    f[0] = (count >= 1) ? _r.readFloat(nullptr) : 0.f;
    f[1] = (count >= 2) ? _r.readFloat(nullptr) : 0.f;
    f[2] = (count >= 3) ? _r.readFloat(nullptr) : 0.f;
    f[3] = (count >= 4) ? _r.readFloat(nullptr) : 0.f;
    mSimd.copy_from(f);
  }
}
template <>
inline void DNAColor::Enumerate<BigDNA::WriteYaml>(typename WriteYaml::StreamT& _w) {
  if (auto v = _w.enterSubVector(nullptr)) {
    zeus::simd_floats f(mSimd);
    _w.writeFloat(nullptr, f[0]);
    _w.writeFloat(nullptr, f[1]);
    _w.writeFloat(nullptr, f[2]);
    _w.writeFloat(nullptr, f[3]);
  }
}
template <>
inline void DNAColor::Enumerate<BigDNA::BinarySize>(typename BinarySize::StreamT& _s) {
  _s += 16;
}

using FourCC = hecl::FourCC;
class UniqueID32;
class UniqueID64;
class UniqueID128;

/** Common virtual interface for runtime ambiguity resolution */
class PAKRouterBase {
protected:
  const SpecBase& m_dataSpec;

public:
  PAKRouterBase(const SpecBase& dataSpec) : m_dataSpec(dataSpec) {}
  hecl::Database::Project& getProject() const { return m_dataSpec.getProject(); }
  virtual hecl::ProjectPath getWorking(const UniqueID32&, bool silenceWarnings = false) const {
    LogDNACommon.report(logvisor::Fatal, fmt("PAKRouter IDType mismatch; expected UniqueID32 specialization"));
    return hecl::ProjectPath();
  }
  virtual hecl::ProjectPath getWorking(const UniqueID64&, bool silenceWarnings = false) const {
    LogDNACommon.report(logvisor::Fatal, fmt("PAKRouter IDType mismatch; expected UniqueID64 specialization"));
    return hecl::ProjectPath();
  }
  virtual hecl::ProjectPath getWorking(const UniqueID128&, bool silenceWarnings = false) const {
    LogDNACommon.report(logvisor::Fatal, fmt("PAKRouter IDType mismatch; expected UniqueID128 specialization"));
    return hecl::ProjectPath();
  }
};

/** Globally-accessed manager allowing UniqueID* classes to directly
 *  lookup destination paths of resources */
class UniqueIDBridge {
  friend class UniqueID32;
  friend class UniqueID64;

  static ThreadLocalPtr<hecl::Database::Project> s_Project;
  static ThreadLocalPtr<IDRestorer<UniqueID32>> s_restorer32;
  static ThreadLocalPtr<IDRestorer<UniqueID64>> s_restorer64;
  static ThreadLocalPtr<IDRestorer<UniqueID128>> s_restorer128;

public:
  template <class IDType>
  static hecl::ProjectPath TranslatePakIdToPath(const IDType& id, bool silenceWarnings = false);
  template <class IDType>
  static hecl::ProjectPath MakePathFromString(std::string_view str);
  template <class IDType>
  static void TransformOldHashToNewHash(IDType& id);

  static void SetThreadProject(hecl::Database::Project& project);

  template <class IDType>
  static IDRestorer<IDType>* GetIDRestorer();
  template <class IDType>
  static void SetIDRestorer(IDRestorer<IDType>* restorer);
};

template <>
inline IDRestorer<UniqueID32>* UniqueIDBridge::GetIDRestorer<UniqueID32>() {
  return s_restorer32.get();
}

template <>
inline void UniqueIDBridge::SetIDRestorer<UniqueID32>(IDRestorer<UniqueID32>* restorer) {
  s_restorer32.reset(restorer);
}

template <>
inline IDRestorer<UniqueID64>* UniqueIDBridge::GetIDRestorer<UniqueID64>() {
  return s_restorer64.get();
}

template <>
inline void UniqueIDBridge::SetIDRestorer<UniqueID64>(IDRestorer<UniqueID64>* restorer) {
  s_restorer64.reset(restorer);
}

template <>
inline IDRestorer<UniqueID128>* UniqueIDBridge::GetIDRestorer<UniqueID128>() {
  return s_restorer128.get();
}

template <>
inline void UniqueIDBridge::SetIDRestorer<UniqueID128>(IDRestorer<UniqueID128>* restorer) {
  s_restorer128.reset(restorer);
}

/** PAK 32-bit Unique ID */
class UniqueID32 : public BigDNA {
protected:
  uint32_t m_id = 0xffffffff;

public:
  using value_type = uint32_t;
  static UniqueID32 kInvalidId;
  AT_DECL_EXPLICIT_DNA_YAML
  bool isValid() const { return m_id != 0xffffffff && m_id != 0; }
  void assign(uint32_t id, bool noOriginal = false);

  UniqueID32& operator=(const hecl::ProjectPath& path) {
    assign(path.hash().val32());
    return *this;
  }

  bool operator!=(const UniqueID32& other) const { return m_id != other.m_id; }
  bool operator==(const UniqueID32& other) const { return m_id == other.m_id; }
  bool operator<(const UniqueID32& other) const { return m_id < other.m_id; }
  uint32_t toUint32() const { return m_id; }
  uint64_t toUint64() const { return m_id; }
  std::string toString() const;
  void clear() { m_id = 0xffffffff; }

  UniqueID32() = default;
  UniqueID32(uint32_t idin, bool noOriginal = false) { assign(idin, noOriginal); }
  UniqueID32(athena::io::IStreamReader& reader) { read(reader); }
  UniqueID32(const hecl::ProjectPath& path) { *this = path; }
  UniqueID32(const char* hexStr) {
    char copy[9];
    strncpy(copy, hexStr, 8);
    copy[8] = '\0';
    assign(strtoul(copy, nullptr, 16));
  }
  UniqueID32(const wchar_t* hexStr) {
    wchar_t copy[9];
    wcsncpy(copy, hexStr, 8);
    copy[8] = L'\0';
    assign(wcstoul(copy, nullptr, 16));
  }

  static constexpr size_t BinarySize() { return 4; }
};

/** PAK 32-bit Unique ID - writes zero when invalid */
class UniqueID32Zero : public UniqueID32 {
public:
  AT_DECL_DNA_YAML
  Delete __d2;
  using UniqueID32::UniqueID32;
};

class AuxiliaryID32 : public UniqueID32 {
  const hecl::SystemChar* m_auxStr;
  const hecl::SystemChar* m_addExtension;
  UniqueID32 m_baseId;

public:
  AT_DECL_DNA
  Delete __d2;
  AuxiliaryID32(const hecl::SystemChar* auxStr, const hecl::SystemChar* addExtension = nullptr)
  : m_auxStr(auxStr), m_addExtension(addExtension) {}

  AuxiliaryID32& operator=(const hecl::ProjectPath& path);
  AuxiliaryID32& operator=(const UniqueID32& id);
  const UniqueID32& getBaseId() const { return m_baseId; }
};

/** PAK 64-bit Unique ID */
class UniqueID64 : public BigDNA {
  uint64_t m_id = 0xffffffffffffffff;

public:
  using value_type = uint64_t;
  AT_DECL_EXPLICIT_DNA_YAML
  bool isValid() const { return m_id != 0xffffffffffffffff && m_id != 0; }
  void assign(uint64_t id, bool noOriginal = false);

  UniqueID64& operator=(const hecl::ProjectPath& path) {
    assign(path.hash().val64());
    return *this;
  }

  bool operator!=(const UniqueID64& other) const { return m_id != other.m_id; }
  bool operator==(const UniqueID64& other) const { return m_id == other.m_id; }
  bool operator<(const UniqueID64& other) const { return m_id < other.m_id; }
  uint64_t toUint64() const { return m_id; }
  std::string toString() const;
  void clear() { m_id = 0xffffffffffffffff; }

  UniqueID64() = default;
  UniqueID64(uint64_t idin, bool noOriginal = false) { assign(idin, noOriginal); }
  UniqueID64(athena::io::IStreamReader& reader) { read(reader); }
  UniqueID64(const hecl::ProjectPath& path) { *this = path; }
  UniqueID64(const char* hexStr) {
    char copy[17];
    strncpy(copy, hexStr, 16);
    copy[16] = '\0';
#if _WIN32
    assign(_strtoui64(copy, nullptr, 16));
#else
    assign(strtouq(copy, nullptr, 16));
#endif
  }
  UniqueID64(const wchar_t* hexStr) {
    wchar_t copy[17];
    wcsncpy(copy, hexStr, 16);
    copy[16] = L'\0';
#if _WIN32
    assign(_wcstoui64(copy, nullptr, 16));
#else
    assign(wcstoull(copy, nullptr, 16));
#endif
  }

  static constexpr size_t BinarySize() { return 8; }
};

/** PAK 128-bit Unique ID */
class UniqueID128 : public BigDNA {
public:
  union Value {
    uint64_t id[2];
#if __SSE__
    __m128i id128;
#endif
  };

private:
  Value m_id;

public:
  using value_type = uint64_t;
  AT_DECL_EXPLICIT_DNA_YAML
  UniqueID128() {
    m_id.id[0] = 0xffffffffffffffff;
    m_id.id[1] = 0xffffffffffffffff;
  }
  UniqueID128(uint64_t idin, bool noOriginal = false) {
    m_id.id[0] = idin;
    m_id.id[1] = 0;
  }
  bool isValid() const {
    return m_id.id[0] != 0xffffffffffffffff && m_id.id[0] != 0 && m_id.id[1] != 0xffffffffffffffff && m_id.id[1] != 0;
  }

  UniqueID128& operator=(const hecl::ProjectPath& path) {
    m_id.id[0] = path.hash().val64();
    m_id.id[1] = 0;
    return *this;
  }
  UniqueID128(const hecl::ProjectPath& path) { *this = path; }

  bool operator!=(const UniqueID128& other) const {
#if __SSE__
    __m128i vcmp = _mm_cmpeq_epi32(m_id.id128, other.m_id.id128);
    int vmask = _mm_movemask_epi8(vcmp);
    return vmask != 0xffff;
#else
    return (m_id.id[0] != other.m_id.id[0]) || (m_id.id[1] != other.m_id.id[1]);
#endif
  }
  bool operator==(const UniqueID128& other) const {
#if __SSE__
    __m128i vcmp = _mm_cmpeq_epi32(m_id.id128, other.m_id.id128);
    int vmask = _mm_movemask_epi8(vcmp);
    return vmask == 0xffff;
#else
    return (m_id.id[0] == other.m_id.id[0]) && (m_id.id[1] == other.m_id.id[1]);
#endif
  }
  bool operator<(const UniqueID128& other) const {
    return m_id.id[0] < other.m_id.id[0] || (m_id.id[0] == other.m_id.id[0] && m_id.id[1] < other.m_id.id[1]);
  }
  void clear() {
    m_id.id[0] = 0xffffffffffffffff;
    m_id.id[1] = 0xffffffffffffffff;
  }
  uint64_t toUint64() const { return m_id.id[0]; }
  uint64_t toHighUint64() const { return m_id.id[0]; }
  uint64_t toLowUint64() const { return m_id.id[1]; }
  std::string toString() const;

  static constexpr size_t BinarySize() { return 16; }
};

/** Casts ID type to its null-zero equivalent */
template <class T>
using CastIDToZero = typename std::conditional_t<std::is_same_v<T, UniqueID32>, UniqueID32Zero, T>;

/** Word Bitmap reader/writer */
class WordBitmap {
  std::vector<atUint32> m_words;
  size_t m_bitCount = 0;

public:
  void read(athena::io::IStreamReader& reader, size_t bitCount);
  void write(athena::io::IStreamWriter& writer) const;
  void reserve(size_t bitCount) { m_words.reserve((bitCount + 31) / 32); }
  void binarySize(size_t& __isz) const;
  size_t getBitCount() const { return m_bitCount; }
  bool getBit(size_t idx) const {
    size_t wordIdx = idx / 32;
    if (wordIdx >= m_words.size())
      return false;
    size_t wordCur = idx % 32;
    return (m_words[wordIdx] >> wordCur) & 0x1;
  }
  void setBit(size_t idx) {
    size_t wordIdx = idx / 32;
    while (wordIdx >= m_words.size())
      m_words.push_back(0);
    size_t wordCur = idx % 32;
    m_words[wordIdx] |= (1 << wordCur);
    m_bitCount = std::max(m_bitCount, idx + 1);
  }
  void unsetBit(size_t idx) {
    size_t wordIdx = idx / 32;
    while (wordIdx >= m_words.size())
      m_words.push_back(0);
    size_t wordCur = idx % 32;
    m_words[wordIdx] &= ~(1 << wordCur);
    m_bitCount = std::max(m_bitCount, idx + 1);
  }
  void clear() {
    m_words.clear();
    m_bitCount = 0;
  }

  class Iterator {
    friend class WordBitmap;
    const WordBitmap& m_bmp;
    size_t m_idx = 0;
    Iterator(const WordBitmap& bmp, size_t idx) : m_bmp(bmp), m_idx(idx) {}

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = bool;
    using difference_type = std::ptrdiff_t;
    using pointer = bool*;
    using reference = bool&;

    Iterator& operator++() {
      ++m_idx;
      return *this;
    }
    bool operator*() { return m_bmp.getBit(m_idx); }
    bool operator!=(const Iterator& other) const { return m_idx != other.m_idx; }
  };
  Iterator begin() const { return Iterator(*this, 0); }
  Iterator end() const { return Iterator(*this, m_bitCount); }
};

/** Resource cooker function */
typedef std::function<bool(const hecl::ProjectPath&, const hecl::ProjectPath&)> ResCooker;

/** Mappings of resources involved in extracting characters */
template <class IDType>
struct CharacterAssociations {
  using RigPair = std::pair<IDType, IDType>;
  /* CMDL -> (CSKR, CINF) */
  std::unordered_map<IDType, RigPair> m_cmdlRigs;
  /* (CSKR, CINF) -> ANCS */
  std::unordered_map<IDType, std::pair<IDType, std::string>> m_cskrCinfToCharacter;
  /* ANCS -> (CINF, CMDL) */
  std::unordered_multimap<IDType, std::pair<RigPair, std::string>> m_characterToAttachmentRigs;
  using MultimapIteratorPair =
      std::pair<typename std::unordered_multimap<IDType, std::pair<RigPair, std::string>>::const_iterator,
                typename std::unordered_multimap<IDType, std::pair<RigPair, std::string>>::const_iterator>;
  void addAttachmentRig(IDType character, IDType cinf, IDType cmdl, const char* name) {
    auto range = m_characterToAttachmentRigs.equal_range(character);
    for (auto it = range.first; it != range.second; ++it)
      if (it->second.second == name)
        return;
    m_characterToAttachmentRigs.insert(std::make_pair(character, std::make_pair(std::make_pair(cinf, cmdl), name)));
  }
};

} // namespace DataSpec

/* Hash template-specializations for UniqueID types */
namespace std {
template <>
struct hash<DataSpec::DNAFourCC> {
  size_t operator()(const DataSpec::DNAFourCC& fcc) const { return fcc.toUint32(); }
};

template <>
struct hash<DataSpec::UniqueID32> {
  size_t operator()(const DataSpec::UniqueID32& id) const { return id.toUint32(); }
};

template <>
struct hash<DataSpec::UniqueID64> {
  size_t operator()(const DataSpec::UniqueID64& id) const { return id.toUint64(); }
};

template <>
struct hash<DataSpec::UniqueID128> {
  size_t operator()(const DataSpec::UniqueID128& id) const { return id.toHighUint64() ^ id.toLowUint64(); }
};
} // namespace std

FMT_CUSTOM_FORMATTER(DataSpec::UniqueID32, fmt("{:08X}"), obj.toUint32())
FMT_CUSTOM_FORMATTER(DataSpec::UniqueID64, fmt("{:016X}"), obj.toUint64())
FMT_CUSTOM_FORMATTER(DataSpec::UniqueID128, fmt("{:016X}{:016X}"), obj.toHighUint64(), obj.toLowUint64())
