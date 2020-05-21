#ifndef MATERIAL_H
#define MATERIAL_H
#include "image.h"
#include <fx/gltf.h>
#include <fx/extensions/khr_materials.h>
#include "Support/counted_ptr.hpp"

struct Material : fx::gltf::Material
{
	enum class Tex
	{
		None = -1,
		Normal,
		Emission,
		Occlusion,
		BaseColor,
		MetallicRoughness,
		Diffuse,
		SpecularGlossiness,
		Total
	};

	KHR::materials::pbrSpecularGlossiness pbrSpecularGlossiness;
	KHR::materials::unlit                 unlit;

	counted_ptr<Image>                    image_slots[(int)Tex::Total];
	int8_t                                tex_coords[(int)Tex::Total];
	bool                                  use_specular{false};

	inline int & TexCoord(Tex tex)
	{
		static int x{};

		switch(tex)
		{
		case Tex::Normal:				return normalTexture.texCoord;
		case Tex::Emission:				return emissiveTexture.texCoord;
		case Tex::Occlusion:			return occlusionTexture.texCoord;
		case Tex::BaseColor:			return pbrMetallicRoughness.baseColorTexture.texCoord;
		case Tex::MetallicRoughness:	return pbrMetallicRoughness.metallicRoughnessTexture.texCoord;
		case Tex::Diffuse:				return pbrSpecularGlossiness.diffuseTexture.texCoord;
		case Tex::SpecularGlossiness:   return pbrSpecularGlossiness.specularGlossinessTexture.texCoord;
		default:
			throw std::logic_error("Unknown material map value.");
		}

		return x;
	}
};

#endif // MATERIAL_H
