#ifndef SPRITEOBJECT_H
#define SPRITEOBJECT_H
#include "Support/countedsizedarray.hpp"
#include "Support/counted_string.h"
#include "material.h"
#include "animation.h"

struct Document;

struct Object
{
	struct Attachment
	{
		counted_string            name;
		std::vector<glm::i16vec4> attachments;
		bool dirty{};
	};

	counted_string                      name;
	std::unique_ptr<Material>           material{new Material()};
	std::vector<counted_ptr<Animation>> animations;
	std::vector<Attachment>             attachments;

	void AddRef() const { ++m_refCount; }
	void Release() { if(--m_refCount == 0) delete this; }

	void RenderAttachments(GLViewWidget *, int attachment = -1);
	void SetAttachment(int selected_attachment, glm::ivec2 position);



private:
	mutable std::atomic<int> m_refCount{1};

	bool attachments_dirty{};

	uint32_t m_attachmentVAO{};
	uint32_t m_attachmentVBO{};
};



#endif // SPRITEOBJECT_H
