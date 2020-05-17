#ifndef SPRITEOBJECT_H
#define SPRITEOBJECT_H
#include "document.h"


struct Object
{
	struct Attachment
	{
		counted_string            name;
		std::vector<glm::i16vec4> attachments;
		bool dirty{};
	};

	counted_string                      name;
	Material                            material;
	std::vector<counted_ptr<Animation>> animations;
	std::vector<Attachment>             attachments;

	enum
	{
		spriteVBO,
		cropVBO,
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

	void RenderSheetBackdrop(GLViewWidget * gl, int frame);
	void Render(GLViewWidget * gl, Material::Tex texture, int frame, int outline);

	void AddRef() const { ++m_refCount; }
	void Release() { if(--m_refCount == 0) delete this; }

	std::string IsImageCompatible(counted_ptr<Image>, Material::Tex);
	void UpdateImages(Document*);
	void UpdateCachedArrays();

	uint32_t                        m_spriteCount{};
	glm::u16vec2                    m_sheetSize{};
	glm::u16vec2                    m_spriteSize{};

	CountedSizedArray<glm::i16vec4> m_sprites{};
	CountedSizedArray<glm::i16vec4> m_crop{};
	CountedSizedArray<glm::u16vec4> m_normalizedCrop{};
	CountedSizedArray<glm::u16vec4> m_normalizedSprites{};

	uint32_t                        m_textures[10];

	uint32_t     m_vao[VAOc]{};
	uint32_t     m_vbo[VBOc]{};

private:
	mutable std::atomic<int> m_refCount{1};

	bool attachments_dirty{};


};



#endif // SPRITEOBJECT_H
