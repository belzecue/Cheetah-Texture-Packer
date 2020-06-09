#ifndef SPRITESHEET_H
#define SPRITESHEET_H
#include <cstdint>
#include <glm/gtc/type_precision.hpp>
#include "Support/countedsizedarray.hpp"

class GLViewWidget;

class SpriteSheet
{
public:
	SpriteSheet();

	void Prepare(GLViewWidget*, CountedSizedArray<glm::i16vec4> sprites);

	uint32_t GetCenters() const { return m_vbo[a_centerTexBuff]; }

	enum
	{
		a_vertex,
		a_sprite_id,

		a_centerTexBuff,

		VBO_c,
	};

	uint32_t m_vao;
	uint32_t m_vbo[VBO_c];
};

#endif // SPRITESHEET_H
