#ifndef MATERIAL_H
#define MATERIAL_H
#include "image.h"
#include <fx/gltf.h>
#include <fx/extensions/khr_materials.h>
#include "Support/counted_ptr.hpp"

struct Material : fx::gltf::Material
{
	enum class Tex : int8_t
	{
		None = -1,
		BaseColor,
		Diffuse,
		MetallicRoughness,
		SpecularGlossiness,
		Normal,
		Occlusion,
		Emission,
		Total
	};

	enum
	{
		v_SpriteVertex,
		v_CroppedVertex,
		centerVBO,
		normalizedVBO,
		texCoord0,
		texCoord1,

		renderVBO,
		AttachmentVBO,
		VBOc,

		SpritePadding = 0,
		CroppedSprites,
		AttachmentVAO,

		VAOc,
	};


	KHR::materials::pbrSpecularGlossiness pbrSpecularGlossiness;
	KHR::materials::unlit                 unlit;

	counted_ptr<Image>                    image_slots[(int)Tex::Total];
	Material::Tex                         current_slot{Tex::None};
	int8_t                                tex_coords[(int)Tex::Total];
	bool                                  use_specular{false};

	void RenderObjectSheet(GLViewWidget *, int frame = -1);
	void RenderSpriteSheet(GLViewWidget *, int image_slot, int frame = -1);
	void RenderAttachments(GLViewWidget *, int attachment = -1);

	void RenderSheetBackdrop(GLViewWidget * gl, int frame);
	void Render(GLViewWidget * gl, Material::Tex texture, int frame, int outline);

	std::string IsImageCompatible(Material::Tex, counted_ptr<Image>);
	void SetImage(Material::Tex, counted_ptr<Image>);
	void UpdateCachedArrays();

	inline int & TexCoord(Tex tex)
	{
		static int x{};

		switch(tex)
		{
		case Tex::BaseColor:			return pbrMetallicRoughness.baseColorTexture.texCoord;
		case Tex::Diffuse:				return pbrSpecularGlossiness.diffuseTexture.texCoord;
		case Tex::MetallicRoughness:	return pbrMetallicRoughness.metallicRoughnessTexture.texCoord;
		case Tex::SpecularGlossiness:   return pbrSpecularGlossiness.specularGlossinessTexture.texCoord;
		case Tex::Normal:				return normalTexture.texCoord;
		case Tex::Occlusion:			return occlusionTexture.texCoord;
		case Tex::Emission:				return emissiveTexture.texCoord;
		default:
			throw std::logic_error("Unknown material map value.");
		}

		return x;
	}

private:
	void Prepare(GLViewWidget*);

	struct Pair
	{
		uint16_t start;
		uint16_t length;
	};

	bool                            m_dirty{true};
	CountedSizedArray<glm::i16vec4> m_sprites{};
	CountedSizedArray<glm::i16vec4> m_crop{};
	CountedSizedArray<glm::u16vec4> m_normalizedCrop{};
	CountedSizedArray<glm::u16vec4> m_normalizedSprites{};

	uint32_t                        m_spriteCount{};
	glm::u16vec2                    m_sheetSize{};

	uint32_t     m_vao[VAOc]{};
	uint32_t     m_vbo[VBOc]{};




};

#endif // MATERIAL_H
