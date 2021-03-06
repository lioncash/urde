#include "CMDLMaterials.hpp"
#include "hecl/Blender/Connection.hpp"

using Stream = hecl::blender::PyOutStream;

namespace DataSpec::DNAMP3 {
using Material = MaterialSet::Material;

template <>
void MaterialSet::Material::SectionFactory::Enumerate<BigDNA::Read>(typename Read::StreamT& reader) {
  DNAFourCC type;
  type.read(reader);
  switch (ISection::Type(type.toUint32())) {
  case ISection::Type::PASS:
    section.reset(new struct SectionPASS);
    section->read(reader);
    break;
  case ISection::Type::CLR:
    section.reset(new struct SectionCLR);
    section->read(reader);
    break;
  case ISection::Type::INT:
    section.reset(new struct SectionINT);
    section->read(reader);
    break;
  default:
    section.reset(nullptr);
    break;
  }
}
template <>
void MaterialSet::Material::SectionFactory::Enumerate<BigDNA::Write>(typename Write::StreamT& writer) {
  if (!section)
    return;
  writer.writeUBytes((atUint8*)&section->m_type, 4);
  section->write(writer);
}
template <>
void MaterialSet::Material::SectionFactory::Enumerate<BigDNA::BinarySize>(typename BinarySize::StreamT& s) {
  s += 4;
  section->binarySize(s);
}

template <>
void MaterialSet::Material::Enumerate<BigDNA::Read>(typename Read::StreamT& reader) {
  header.read(reader);
  sections.clear();
  do {
    sections.emplace_back();
    sections.back().read(reader);
  } while (sections.back().section);
  sections.pop_back();
}
template <>
void MaterialSet::Material::Enumerate<BigDNA::Write>(typename Write::StreamT& writer) {
  header.write(writer);
  for (const SectionFactory& section : sections)
    section.write(writer);
  writer.writeUBytes((atUint8*)"END ", 4);
}
template <>
void MaterialSet::Material::Enumerate<BigDNA::BinarySize>(typename BinarySize::StreamT& s) {
  header.binarySize(s);
  for (const SectionFactory& section : sections)
    section.binarySize(s);
  s += 4;
}

void MaterialSet::RegisterMaterialProps(Stream& out) {
  out << "bpy.types.Material.retro_alpha_test = bpy.props.BoolProperty(name='Retro: Punchthrough Alpha')\n"
         "bpy.types.Material.retro_shadow_occluder = bpy.props.BoolProperty(name='Retro: Shadow Occluder')\n"
         "bpy.types.Material.retro_lightmapped = bpy.props.BoolProperty(name='Retro: Lightmapped')\n"
         "bpy.types.Material.retro_opac = bpy.props.IntProperty(name='Retro: OPAC')\n"
         "bpy.types.Material.retro_blod = bpy.props.IntProperty(name='Retro: BLOD')\n"
         "bpy.types.Material.retro_bloi = bpy.props.IntProperty(name='Retro: BLOI')\n"
         "bpy.types.Material.retro_bnif = bpy.props.IntProperty(name='Retro: BNIF')\n"
         "bpy.types.Material.retro_xrbr = bpy.props.IntProperty(name='Retro: XRBR')\n"
         "\n";
}

void MaterialSet::ConstructMaterial(Stream& out, const PAKRouter<PAKBridge>& pakRouter, const PAK::Entry& entry,
                                    const Material& material, unsigned groupIdx, unsigned matIdx) {
  unsigned i;

  out.format(fmt(
      "new_material = bpy.data.materials.new('MAT_{}_{}')\n"
      "new_material.use_shadows = True\n"
      "new_material.use_transparent_shadows = True\n"
      "new_material.diffuse_color = (1.0,1.0,1.0)\n"
      "new_material.use_nodes = True\n"
      "new_nodetree = new_material.node_tree\n"
      "material_node = new_nodetree.nodes['Material']\n"
      "final_node = new_nodetree.nodes['Output']\n"
      "\n"
      "gridder = hecl.Nodegrid(new_nodetree)\n"
      "gridder.place_node(final_node, 3)\n"
      "gridder.place_node(material_node, 0)\n"
      "material_node.material = new_material\n"
      "\n"
      "texture_nodes = []\n"
      "kcolor_nodes = []\n"
      "color_combiner_nodes = []\n"
      "alpha_combiner_nodes = []\n"
      "tex_links = []\n"
      "tev_reg_sockets = [None]*4\n"
      "\n"),
      groupIdx, matIdx);

  /* Material Flags */
  out.format(fmt(
      "new_material.retro_alpha_test = {}\n"
      "new_material.retro_shadow_occluder = {}\n"
      "new_material.diffuse_color = (1, 1, 1, {})\n"),
      material.header.flags.alphaTest() ? "True" : "False",
      material.header.flags.shadowOccluderMesh() ? "True" : "False",
      material.header.flags.shadowOccluderMesh() ? "0" : "1");

  /* Blend factors */
  if (material.header.flags.alphaBlending())
    out << "new_material.blend_method = 'BLEND'\n";
  else if (material.header.flags.additiveBlending())
    out << "new_material.blend_method = 'ADD'\n";

  /* Texmap list */
  out << "tex_maps = []\n"
         "pnode = None\n"
         "anode = None\n"
         "rflv_tex_node = None\n";

  /* Add PASSes */
  i = 0;
  unsigned texMapIdx = 0;
  unsigned texMtxIdx = 0;
  unsigned kColorIdx = 0;
  Material::ISection* prevSection = nullptr;
  for (const Material::SectionFactory& factory : material.sections) {
    factory.section->constructNode(out, pakRouter, entry, prevSection, i++, texMapIdx, texMtxIdx, kColorIdx);
    Material::SectionPASS* pass = Material::SectionPASS::castTo(factory.section.get());
    if (!pass ||
        (pass && Material::SectionPASS::Subtype(pass->subtype.toUint32()) != Material::SectionPASS::Subtype::RFLV))
      prevSection = factory.section.get();
  }

  /* Connect final PASS */
  out << "if pnode:\n"
         "    new_nodetree.links.new(pnode.outputs['Next Color'], final_node.inputs['Color'])\n"
         "else:\n"
         "    new_nodetree.links.new(kcolor_nodes[-1][0].outputs[0], final_node.inputs['Color'])\n"
         "if anode:\n"
         "    new_nodetree.links.new(anode.outputs['Value'], final_node.inputs['Alpha'])\n"
         "elif pnode:\n"
         "    new_nodetree.links.new(pnode.outputs['Next Alpha'], final_node.inputs['Alpha'])\n"
         "else:\n"
         "    new_nodetree.links.new(kcolor_nodes[-1][1].outputs[0], final_node.inputs['Alpha'])\n";
}

void Material::SectionPASS::constructNode(hecl::blender::PyOutStream& out, const PAKRouter<PAKBridge>& pakRouter,
                                          const PAK::Entry& entry, const Material::ISection* prevSection, unsigned idx,
                                          unsigned& texMapIdx, unsigned& texMtxIdx, unsigned& kColorIdx) const {
  /* Add Texture nodes */
  if (txtrId.isValid()) {
    std::string texName = pakRouter.getBestEntryName(txtrId);
    const nod::Node* node;
    const PAK::Entry* texEntry = pakRouter.lookupEntry(txtrId, &node);
    hecl::ProjectPath txtrPath = pakRouter.getWorking(texEntry);
    if (txtrPath.isNone()) {
      txtrPath.makeDirChain(false);
      PAKEntryReadStream rs = texEntry->beginReadStream(*node);
      TXTR::Extract(rs, txtrPath);
    }
    hecl::SystemString resPath = pakRouter.getResourceRelativePath(entry, txtrId);
    hecl::SystemUTF8Conv resPathView(resPath);
    out.format(fmt(
        "if '{}' in bpy.data.textures:\n"
        "    image = bpy.data.images['{}']\n"
        "    texture = bpy.data.textures[image.name]\n"
        "else:\n"
        "    image = bpy.data.images.load('''//{}''')\n"
        "    image.name = '{}'\n"
        "    texture = bpy.data.textures.new(image.name, 'IMAGE')\n"
        "    texture.image = image\n"
        "tex_maps.append(texture)\n"
        "\n"),
        texName, texName, resPathView, texName);
    if (uvAnim.size()) {
      const UVAnimation& uva = uvAnim[0];
      DNAMP1::MaterialSet::Material::AddTexture(out, GX::TexGenSrc(uva.unk1 + (uva.unk1 < 2 ? 0 : 2)), texMtxIdx,
                                                texMapIdx++, false);
      DNAMP1::MaterialSet::Material::AddTextureAnim(out, uva.anim.mode, texMtxIdx++, uva.anim.vals);
    } else
      DNAMP1::MaterialSet::Material::AddTexture(out, GX::TexGenSrc(uvSrc + 4), -1, texMapIdx++, false);
  }

  /* Special case for RFLV (environment UV mask) */
  if (Subtype(subtype.toUint32()) == Subtype::RFLV) {
    if (txtrId.isValid())
      out << "rflv_tex_node = texture_nodes[-1]\n";
    return;
  }

  /* Add PASS node */
  bool linkRAS = false;
  out << "prev_pnode = pnode\n"
         "pnode = new_nodetree.nodes.new('ShaderNodeGroup')\n";
  switch (Subtype(subtype.toUint32())) {
  case Subtype::DIFF: {
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassDIFF']\n";
    if (txtrId.isValid()) {
      out << "new_material.hecl_lightmap = texture.name\n"
          << "texture.image.use_fake_user = True\n";
    }
    linkRAS = true;
    break;
  }
  case Subtype::RIML:
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassRIML']\n";
    if (idx == 0)
      linkRAS = true;
    break;
  case Subtype::BLOL:
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassBLOL']\n";
    if (idx == 0)
      linkRAS = true;
    break;
  case Subtype::BLOD:
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassBLOD']\n";
    if (idx == 0)
      linkRAS = true;
    break;
  case Subtype::CLR:
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassCLR']\n";
    if (idx == 0)
      linkRAS = true;
    break;
  case Subtype::TRAN:
    if (flags.TRANInvert())
      out << "pnode.node_tree = bpy.data.node_groups['RetroPassTRANInv']\n";
    else
      out << "pnode.node_tree = bpy.data.node_groups['RetroPassTRAN']\n";
    break;
  case Subtype::INCA:
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassINCA']\n";
    break;
  case Subtype::RFLV:
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassRFLV']\n";
    break;
  case Subtype::RFLD:
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassRFLD']\n"
           "if rflv_tex_node:\n"
           "    new_nodetree.links.new(rflv_tex_node.outputs['Color'], pnode.inputs['Mask Color'])\n"
           "    new_nodetree.links.new(rflv_tex_node.outputs['Value'], pnode.inputs['Mask Alpha'])\n";
    break;
  case Subtype::LRLD:
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassLRLD']\n";
    break;
  case Subtype::LURD:
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassLURD']\n";
    break;
  case Subtype::BLOI:
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassBLOI']\n";
    break;
  case Subtype::XRAY:
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassXRAY']\n";
    break;
  case Subtype::TOON:
    out << "pnode.node_tree = bpy.data.node_groups['RetroPassTOON']\n";
    break;
  default:
    break;
  }
  out << "gridder.place_node(pnode, 2)\n";

  if (txtrId.isValid()) {
    out << "new_nodetree.links.new(texture_nodes[-1].outputs['Color'], pnode.inputs['Tex Color'])\n"
           "new_nodetree.links.new(texture_nodes[-1].outputs['Value'], pnode.inputs['Tex Alpha'])\n";
  }

  if (linkRAS)
    out << "new_nodetree.links.new(material_node.outputs['Color'], pnode.inputs['Prev Color'])\n"
           "new_nodetree.links.new(material_node.outputs['Alpha'], pnode.inputs['Prev Alpha'])\n";
  else if (prevSection) {
    if (prevSection->m_type == ISection::Type::PASS &&
        Subtype(static_cast<const SectionPASS*>(prevSection)->subtype.toUint32()) != Subtype::RFLV)
      out << "new_nodetree.links.new(prev_pnode.outputs['Next Color'], pnode.inputs['Prev Color'])\n"
             "new_nodetree.links.new(prev_pnode.outputs['Next Alpha'], pnode.inputs['Prev Alpha'])\n";
    else if (prevSection->m_type == ISection::Type::CLR)
      out << "new_nodetree.links.new(kcolor_nodes[-1][0].outputs[0], pnode.inputs['Prev Color'])\n"
             "new_nodetree.links.new(kcolor_nodes[-1][1].outputs[0], pnode.inputs['Prev Alpha'])\n";
  }

  /* Row Break in gridder */
  out << "gridder.row_break(2)\n";
}

void Material::SectionCLR::constructNode(hecl::blender::PyOutStream& out, const PAKRouter<PAKBridge>& pakRouter,
                                         const PAK::Entry& entry, const Material::ISection* prevSection, unsigned idx,
                                         unsigned& texMapIdx, unsigned& texMtxIdx, unsigned& kColorIdx) const {
  DNAMP1::MaterialSet::Material::AddKcolor(out, color, kColorIdx++);
  switch (Subtype(subtype.toUint32())) {
  case Subtype::DIFB:
    out << "kc_node.label += ' DIFB'\n"
           "ka_node.label += ' DIFB'\n";
    break;
  default:
    break;
  }
}

void Material::SectionINT::constructNode(hecl::blender::PyOutStream& out, const PAKRouter<PAKBridge>& pakRouter,
                                         const PAK::Entry& entry, const Material::ISection* prevSection, unsigned idx,
                                         unsigned& texMapIdx, unsigned& texMtxIdx, unsigned& kColorIdx) const {
  switch (Subtype(subtype.toUint32())) {
  case Subtype::OPAC: {
    GX::Color clr(value);
    out.format(fmt(
        "anode = new_nodetree.nodes.new('ShaderNodeValue')\n"
        "anode.outputs['Value'].default_value = {}\n"),
        float(clr[3]) / float(0xff));
    out << "gridder.place_node(anode, 1)\n";
  } break;
  case Subtype::BLOD:
    out.format(fmt("new_material.retro_blod = {}\n"), value);
    break;
  case Subtype::BLOI:
    out.format(fmt("new_material.retro_bloi = {}\n"), value);
    break;
  case Subtype::BNIF:
    out.format(fmt("new_material.retro_bnif = {}\n"), value);
    break;
  case Subtype::XRBR:
    out.format(fmt("new_material.retro_xrbr = {}\n"), value);
    break;
  default:
    break;
  }
}

} // namespace DataSpec::DNAMP3
