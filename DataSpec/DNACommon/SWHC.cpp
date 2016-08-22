#include "SWHC.hpp"

namespace DataSpec
{
namespace DNAParticle
{

template <class IDType>
void SWSH<IDType>::read(athena::io::YAMLDocReader& r)
{
    for (const auto& elem : r.getCurNode()->m_mapChildren)
    {
        if (elem.first.size() < 4)
        {
            LogModule.report(logvisor::Warning, "short FourCC in element '%s'", elem.first.c_str());
            continue;
        }

        r.enterSubRecord(elem.first.c_str());
        switch(*reinterpret_cast<const uint32_t*>(elem.first.data()))
        {
        case SBIG('PSLT'):
            x0_PSLT.read(r);
        break;
        case SBIG('TIME'):
            x4_TIME.read(r);
        break;
        case SBIG('LRAD'):
            x8_LRAD.read(r);
        break;
        case SBIG('RRAD'):
            xc_RRAD.read(r);
        break;
        case SBIG('LENG'):
            x10_LENG.read(r);
        break;
        case SBIG('COLR'):
            x14_COLR.read(r);
        break;
        case SBIG('SIDE'):
            x18_SIDE.read(r);
        break;
        case SBIG('IROT'):
            x1c_IROT.read(r);
        break;
        case SBIG('ROTM'):
            x20_ROTM.read(r);
        break;
        case SBIG('POFS'):
            x24_POFS.read(r);
        break;
        case SBIG('IVEL'):
            x28_IVEL.read(r);
        break;
        case SBIG('NPOS'):
            x2c_NPOS.read(r);
        break;
        case SBIG('VELM'):
            x30_VELM.read(r);
        break;
        case SBIG('VLM2'):
            x34_VLM2.read(r);
        break;
        case SBIG('SPLN'):
            x38_SPLN.read(r);
        break;
        case SBIG('TEXR'):
            x3c_TEXR.read(r);
        break;
        case SBIG('TSPN'):
            x40_TSPN.read(r);
        break;
        case SBIG('LLRD'):
            x44_24_LLRD = r.readBool(nullptr);
        break;
        case SBIG('CROS'):
            x44_25_CROS = r.readBool(nullptr);
        break;
        case SBIG('VLS1'):
            x44_26_VLS1 = r.readBool(nullptr);
        break;
        case SBIG('VLS2'):
            x44_27_VLS2 = r.readBool(nullptr);
        break;
        case SBIG('SROT'):
            x44_28_SROT = r.readBool(nullptr);
        break;
        case SBIG('WIRE'):
            x44_29_WIRE = r.readBool(nullptr);
        break;
        case SBIG('TEXW'):
            x44_30_TEXW = r.readBool(nullptr);
        break;
        case SBIG('AALP'):
            x44_31_AALP = r.readBool(nullptr);
        break;
        case SBIG('ZBUF'):
            x45_24_ZBUF = r.readBool(nullptr);
        break;
        case SBIG('ORNT'):
            x45_25_ORNT = r.readBool(nullptr);
        break;
        case SBIG('CRND'):
            x45_26_CRND = r.readBool(nullptr);
        break;
        }

        r.leaveSubRecord();
    }
}

template <class IDType>
void SWSH<IDType>::write(athena::io::YAMLDocWriter& w) const
{
    if (x0_PSLT)
    {
        w.enterSubRecord("PSLT");
        x0_PSLT.write(w);
        w.leaveSubRecord();
    }
    if (x4_TIME)
    {
        w.enterSubRecord("TIME");
        x4_TIME.write(w);
        w.leaveSubRecord();
    }
    if (x8_LRAD)
    {
        w.enterSubRecord("LRAD");
        x8_LRAD.write(w);
        w.leaveSubRecord();
    }
    if (xc_RRAD)
    {
        w.enterSubRecord("RRAD");
        xc_RRAD.write(w);
        w.leaveSubRecord();
    }
    if (x10_LENG)
    {
        w.enterSubRecord("LENG");
        x10_LENG.write(w);
        w.leaveSubRecord();
    }
    if (x14_COLR)
    {
        w.enterSubRecord("COLR");
        x14_COLR.write(w);
        w.leaveSubRecord();
    }
    if (x18_SIDE)
    {
        w.enterSubRecord("SIDE");
        x18_SIDE.write(w);
        w.leaveSubRecord();
    }
    if (x1c_IROT)
    {
        w.enterSubRecord("IROT");
        x1c_IROT.write(w);
        w.leaveSubRecord();
    }
    if (x20_ROTM)
    {
        w.enterSubRecord("ROTM");
        x20_ROTM.write(w);
        w.leaveSubRecord();
    }
    if (x24_POFS)
    {
        w.enterSubRecord("POFS");
        x24_POFS.write(w);
        w.leaveSubRecord();
    }
    if (x28_IVEL)
    {
        w.enterSubRecord("IVEL");
        x28_IVEL.write(w);
        w.leaveSubRecord();
    }
    if (x2c_NPOS)
    {
        w.enterSubRecord("NPOS");
        x2c_NPOS.write(w);
        w.leaveSubRecord();
    }
    if (x30_VELM)
    {
        w.enterSubRecord("VELM");
        x30_VELM.write(w);
        w.leaveSubRecord();
    }
    if (x34_VLM2)
    {
        w.enterSubRecord("VLM2");
        x34_VLM2.write(w);
        w.leaveSubRecord();
    }
    if (x38_SPLN)
    {
        w.enterSubRecord("SPLN");
        x38_SPLN.write(w);
        w.leaveSubRecord();
    }
    if (x3c_TEXR)
    {
        w.enterSubRecord("TEXR");
        x3c_TEXR.write(w);
        w.leaveSubRecord();
    }
    if (x40_TSPN)
    {
        w.enterSubRecord("TSPN");
        x40_TSPN.write(w);
        w.leaveSubRecord();
    }

    if (x44_24_LLRD)
        w.writeBool("LLRD", true);
    if (!x44_25_CROS)
        w.writeBool("CROS", false);
    if (x44_26_VLS1)
        w.writeBool("VLS1", true);
    if (x44_27_VLS2)
        w.writeBool("VLS2", true);
    if (x44_28_SROT)
        w.writeBool("SROT", true);
    if (x44_29_WIRE)
        w.writeBool("WIRE", true);
    if (x44_30_TEXW)
        w.writeBool("TEXW", true);
    if (x44_31_AALP)
        w.writeBool("AALP", true);
    if (x45_24_ZBUF)
        w.writeBool("ZBUF", true);
    if (x45_25_ORNT)
        w.writeBool("ORNT", true);
    if (x45_26_CRND)
        w.writeBool("CRND", true);
}


template <class IDType>
size_t SWSH<IDType>::binarySize(size_t __isz) const
{
    __isz += 4;
    if (x0_PSLT)
        __isz = x0_PSLT.binarySize(__isz + 4);
    if (x4_TIME)
        __isz = x4_TIME.binarySize(__isz + 4);
    if (x8_LRAD)
        __isz = x8_LRAD.binarySize(__isz + 4);
    if (xc_RRAD)
        __isz = xc_RRAD.binarySize(__isz + 4);
    if (x10_LENG)
        __isz = x10_LENG.binarySize(__isz + 4);
    if (x14_COLR)
        __isz = x14_COLR.binarySize(__isz + 4);
    if (x18_SIDE)
        __isz = x18_SIDE.binarySize(__isz + 4);
    if (x1c_IROT)
        __isz = x1c_IROT.binarySize(__isz + 4);
    if (x20_ROTM)
        __isz = x20_ROTM.binarySize(__isz + 4);
    if (x24_POFS)
        __isz = x24_POFS.binarySize(__isz + 4);
    if (x28_IVEL)
        __isz = x28_IVEL.binarySize(__isz + 4);
    if (x2c_NPOS)
        __isz = x2c_NPOS.binarySize(__isz + 4);
    if (x30_VELM)
        __isz = x30_VELM.binarySize(__isz + 4);
    if (x34_VLM2)
        __isz = x34_VLM2.binarySize(__isz + 4);
    if (x38_SPLN)
        __isz = x38_SPLN.binarySize(__isz + 4);
    if (x3c_TEXR)
        __isz = x3c_TEXR.binarySize(__isz + 4);
    if (x40_TSPN)
        __isz = x40_TSPN.binarySize(__isz + 4);
    if (x44_24_LLRD)
        __isz += 9;
    if (!x44_25_CROS)
        __isz += 9;
    if (x44_26_VLS1)
        __isz += 9;
    if (x44_27_VLS2)
        __isz += 9;
    if (x44_28_SROT)
        __isz += 9;
    if (x44_29_WIRE)
        __isz += 9;
    if (x44_30_TEXW)
        __isz += 9;
    if (x44_31_AALP)
        __isz += 9;
    if (x45_24_ZBUF)
        __isz += 9;
    if (x45_25_ORNT)
        __isz += 9;
    if (x45_26_CRND)
        __isz += 9;

    return __isz;
}

template <class IDType>
void SWSH<IDType>::read(athena::io::IStreamReader& r)
{
    uint32_t clsId;
    r.readBytesToBuf(&clsId, 4);
    if (clsId != SBIG('SWSH'))
    {
        LogModule.report(logvisor::Warning, "non SWSH provided to SWSH parser");
        return;
    }

    r.readBytesToBuf(&clsId, 4);
    while (clsId != SBIG('_END'))
    {
        switch(clsId)
        {
        case SBIG('PSLT'):
            x0_PSLT.read(r);
        break;
        case SBIG('TIME'):
            x4_TIME.read(r);
        break;
        case SBIG('LRAD'):
            x8_LRAD.read(r);
        break;
        case SBIG('RRAD'):
            xc_RRAD.read(r);
        break;
        case SBIG('LENG'):
            x10_LENG.read(r);
        break;
        case SBIG('COLR'):
            x14_COLR.read(r);
        break;
        case SBIG('SIDE'):
            x18_SIDE.read(r);
        break;
        case SBIG('IROT'):
            x1c_IROT.read(r);
        break;
        case SBIG('ROTM'):
            x20_ROTM.read(r);
        break;
        case SBIG('POFS'):
            x24_POFS.read(r);
        break;
        case SBIG('IVEL'):
            x28_IVEL.read(r);
        break;
        case SBIG('NPOS'):
            x2c_NPOS.read(r);
        break;
        case SBIG('VELM'):
            x30_VELM.read(r);
        break;
        case SBIG('VLM2'):
            x34_VLM2.read(r);
        break;
        case SBIG('SPLN'):
            x38_SPLN.read(r);
        break;
        case SBIG('TEXR'):
            x3c_TEXR.read(r);
        break;
        case SBIG('TSPN'):
            x40_TSPN.read(r);
        break;
        case SBIG('LLRD'):
            r.readUint32Big();
            x44_24_LLRD = r.readBool();
        break;
        case SBIG('CROS'):
            r.readUint32Big();
            x44_25_CROS = r.readBool();
        break;
        case SBIG('VLS1'):
            r.readUint32Big();
            x44_26_VLS1 = r.readBool();
        break;
        case SBIG('VLS2'):
            r.readUint32Big();
            x44_27_VLS2 = r.readBool();
        break;
        case SBIG('SROT'):
            r.readUint32Big();
            x44_28_SROT = r.readBool();
        break;
        case SBIG('WIRE'):
            r.readUint32Big();
            x44_29_WIRE = r.readBool();
        break;
        case SBIG('TEXW'):
            r.readUint32Big();
            x44_30_TEXW = r.readBool();
        break;
        case SBIG('AALP'):
            r.readUint32Big();
            x44_31_AALP = r.readBool();
        break;
        case SBIG('ZBUF'):
            r.readUint32Big();
            x45_24_ZBUF = r.readBool();
        break;
        case SBIG('ORNT'):
            r.readUint32Big();
            x45_25_ORNT = r.readBool();
        break;
        case SBIG('CRND'):
            r.readUint32Big();
            x45_26_CRND = r.readBool();
        break;
        default:
            LogModule.report(logvisor::Fatal, "Unknown SWSH class %.4s @%" PRIi64, &clsId, r.position());
        break;
        }
        r.readBytesToBuf(&clsId, 4);
    }
}

template <class IDType>
void SWSH<IDType>::write(athena::io::IStreamWriter& w) const
{
    w.writeBytes((atInt8*)"SWSH", 4);
    if (x0_PSLT)
    {
        w.writeBytes((atInt8*)"PSLT", 4);
        x0_PSLT.write(w);
    }
    if (x4_TIME)
    {
        w.writeBytes((atInt8*)"TIME", 4);
        x4_TIME.write(w);
    }
    if (x8_LRAD)
    {
        w.writeBytes((atInt8*)"LRAD", 4);
        x8_LRAD.write(w);
    }
    if (xc_RRAD)
    {
        w.writeBytes((atInt8*)"RRAD", 4);
        xc_RRAD.write(w);
    }
    if (x10_LENG)
    {
        w.writeBytes((atInt8*)"LENG", 4);
        x10_LENG.write(w);
    }
    if (x14_COLR)
    {
        w.writeBytes((atInt8*)"COLR", 4);
        x14_COLR.write(w);
    }
    if (x18_SIDE)
    {
        w.writeBytes((atInt8*)"SIDE", 4);
        x18_SIDE.write(w);
    }
    if (x1c_IROT)
    {
        w.writeBytes((atInt8*)"IROT", 4);
        x1c_IROT.write(w);
    }
    if (x20_ROTM)
    {
        w.writeBytes((atInt8*)"ROTM", 4);
        x20_ROTM.write(w);
    }
    if (x24_POFS)
    {
        w.writeBytes((atInt8*)"POFS", 4);
        x24_POFS.write(w);
    }
    if (x28_IVEL)
    {
        w.writeBytes((atInt8*)"IVEL", 4);
        x28_IVEL.write(w);
    }
    if (x2c_NPOS)
    {
        w.writeBytes((atInt8*)"NPOS", 4);
        x2c_NPOS.write(w);
    }
    if (x30_VELM)
    {
        w.writeBytes((atInt8*)"VELM", 4);
        x30_VELM.write(w);
    }
    if (x34_VLM2)
    {
        w.writeBytes((atInt8*)"VLM2", 4);
        x34_VLM2.write(w);
    }
    if (x38_SPLN)
    {
        w.writeBytes((atInt8*)"SPLN", 4);
        x38_SPLN.write(w);
    }
    if (x3c_TEXR)
    {
        w.writeBytes((atInt8*)"TEXR", 4);
        x3c_TEXR.write(w);
    }
    if (x40_TSPN)
    {
        w.writeBytes((atInt8*)"TSPN", 4);
        x40_TSPN.write(w);
    }

    if (x44_24_LLRD)
        w.writeBytes("LLRDCNST\x01", 9);
    if (!x44_25_CROS)
        w.writeBytes("CROSCNST\x00", 9);
    if (x44_26_VLS1)
        w.writeBytes("VLS1CNST\x01", 9);
    if (x44_27_VLS2)
        w.writeBytes("VLS2CNST\x01", 9);
    if (x44_28_SROT)
        w.writeBytes("SROTCNST\x01", 9);
    if (x44_29_WIRE)
        w.writeBytes("WIRECNST\x01", 9);
    if (x44_30_TEXW)
        w.writeBytes("TEXWCNST\x01", 9);
    if (x44_31_AALP)
        w.writeBytes("AALPCNST\x01", 9);
    if (x45_24_ZBUF)
        w.writeBytes("ZBUFCNST\x01", 9);
    if (x45_25_ORNT)
        w.writeBytes("ORNTCNST\x01", 9);
    if (x45_26_CRND)
        w.writeBytes("CRNDCNST\x01", 9);
    w.writeBytes("_END", 4);
}

template struct SWSH<UniqueID32>;
template struct SWSH<UniqueID64>;

template <class IDType>
bool ExtractSWSH(PAKEntryReadStream& rs, const hecl::ProjectPath& outPath)
{
    athena::io::FileWriter writer(outPath.getAbsolutePath());
    if (writer.isOpen())
    {
        SWSH<IDType> swsh;
        swsh.read(rs);
        swsh.toYAMLStream(writer);
        return true;
    }
    return false;
}
template bool ExtractSWSH<UniqueID32>(PAKEntryReadStream& rs, const hecl::ProjectPath& outPath);
template bool ExtractSWSH<UniqueID64>(PAKEntryReadStream& rs, const hecl::ProjectPath& outPath);

template <class IDType>
bool WriteSWSH(const SWSH<IDType>& swsh, const hecl::ProjectPath& outPath)
{
    athena::io::FileWriter w(outPath.getAbsolutePath(), true, false);
    if (w.hasError())
        return false;
    swsh.write(w);
    int64_t rem = w.position() % 32;
    if (rem)
        for (int64_t i=0 ; i<32-rem ; ++i)
            w.writeBytes((atInt8*)"\xff", 1);
    return true;
}
template bool WriteSWSH<UniqueID32>(const SWSH<UniqueID32>& swsh, const hecl::ProjectPath& outPath);
template bool WriteSWSH<UniqueID64>(const SWSH<UniqueID64>& swsh, const hecl::ProjectPath& outPath);

}
}
