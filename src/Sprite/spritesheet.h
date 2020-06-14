#ifndef SPRITESHEET_H
#define SPRITESHEET_H
#include <cstdint>
#include <glm/gtc/type_precision.hpp>
#include "Support/countedsizedarray.hpp"

class GLViewWidget;
class RenderData;

class SpriteSheet
{
public:
	SpriteSheet() = default;
	~SpriteSheet();

	void Clear(GLViewWidget*);

	void Prepare(GLViewWidget*, CountedSizedArray<glm::i16vec4> & sprites, glm::i16vec2 sheet_size);
	void RenderSheet(GLViewWidget * gl, RenderData db);

	void BindCenters      (GLViewWidget*, uint32_t active_texture);
	void BindBoundingBoxes(GLViewWidget*, uint32_t active_texture);

	bool         empty()  const { return !m_vao; }
	glm::i16vec2 size()   const { return m_size; }
	uint32_t     length() const { return m_length; }

	enum
	{
		a_vertex,

		a_centers,
		a_bounds,

		VBOc,

		t_bounds = 0,
		t_centers,

		TEXc,

		v_Squares = 0,
		v_Outlines,

		VAOc,
	};

private:
	uint32_t    m_vao[VAOc]{};
	uint32_t    m_vbo[VBOc]{};
	uint32_t    m_texture[TEXc]{};
	void const* m_sprites{};
	glm::i16vec2 m_size{0, 0};
	uint32_t     m_length{};
};

#endif // SPRITESHEET_H
