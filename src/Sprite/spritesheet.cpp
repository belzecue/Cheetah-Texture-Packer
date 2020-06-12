#include "spritesheet.h"
#include "src/widgets/glviewwidget.h"
#include "Shaders/defaultvaos.h"
#include "Support/vectoroperations.hpp"
#include <cassert>

#define UNUSED(x) (void)x;


SpriteSheet::~SpriteSheet()
{
	assert(m_vao == 0);
}

void SpriteSheet::Clear(GLViewWidget* gl)
{
	if(m_vao == 0)
		return;

	_gl glDeleteVertexArrays(1, &m_vao);
	_gl glDeleteBuffers(VBOc, m_vbo);
	_gl glDeleteTextures(TEXc, m_texture);

	memset(m_vbo, 0, sizeof(m_vbo));
	memset(m_texture, 0, sizeof(m_texture));
	m_vao     = 0;
	m_sprites = 0;
	m_size    = glm::i16vec2(0, 0);
}

struct vertex
{
	glm::i16vec2 position;
	glm::i8vec2  texCoord0;
	uint32_t     sprite_id;
};

inline void PushSprite(std::vector<vertex> & vec, glm::i16vec4 item, uint32_t id)
{
	glm::ivec2 center((item.x + item.z) / 2, (item.y + item.w) / 2);

	vec.push_back({{item.x  - center.x, item.y -  center.y}, {0, 0}, id});
	vec.push_back({{item.z  - center.x, item.y -  center.y}, {0, 0}, id});
	vec.push_back({{item.z  - center.x, item.w -  center.y}, {0, 0}, id});
	vec.push_back({{item.x  - center.x, item.w -  center.y}, {0, 0}, id});
}


void SpriteSheet::Prepare(GLViewWidget* gl, CountedSizedArray<glm::i16vec4> sprites, glm::i16vec2 sheet_size)
{
	GL_ASSERT;

	if(m_sprites == sprites.data()
	&& m_size    == sheet_size)
		return;

	m_sprites = sprites.data();
	m_size    = sheet_size;

	if(m_sprites == nullptr)
		return Clear(gl);


	if(!m_vao)
	{
		_gl glGenVertexArrays(1, &m_vao);
		_gl glGenBuffers(VBOc, m_vbo);
		_gl glGenTextures(1,   m_texture);

		_gl glBindVertexArray(m_vao);
		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[a_vertex]);
		glDefaultVAOs::BindSquareIndexVBO(gl);

		_gl glVertexAttribPointer(0, 3, GL_SHORT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, position));
	    _gl glVertexAttribPointer(2, 2, GL_BYTE, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, texCoord0));
		_gl glVertexAttribIPointer(3, 2, GL_INT, sizeof(vertex), (void*) offsetof(vertex, sprite_id));

		_gl glEnableVertexAttribArray(0);
		_gl glEnableVertexAttribArray(2);

		_gl glBindTexture(GL_TEXTURE_BUFFER, m_texture[t_centers]);
		_gl glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA16UI, m_vbo[a_centers]);

		_gl glTexParameteri( GL_TEXTURE_BUFFER, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		_gl glTexParameteri( GL_TEXTURE_BUFFER, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		_gl glTexParameteri( GL_TEXTURE_BUFFER, GL_TEXTURE_WRAP_S, GL_CLAMP );
		_gl glTexParameteri( GL_TEXTURE_BUFFER, GL_TEXTURE_WRAP_T, GL_CLAMP );

		_gl glBindTexture(GL_TEXTURE_BUFFER, m_texture[t_bounds]);
		_gl glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA16UI, m_vbo[a_bounds]);

		_gl glTexParameteri( GL_TEXTURE_BUFFER, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		_gl glTexParameteri( GL_TEXTURE_BUFFER, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		_gl glTexParameteri( GL_TEXTURE_BUFFER, GL_TEXTURE_WRAP_S, GL_CLAMP );
		_gl glTexParameteri( GL_TEXTURE_BUFFER, GL_TEXTURE_WRAP_T, GL_CLAMP );

		_gl glBindTexture(GL_TEXTURE_BUFFER, 0);

		GL_ASSERT;
	}


	_gl glBindVertexArray(m_vao);

	{
		std::vector<vertex> vec;
		vec.reserve((sprites.size() + 1)*4);

		PushSprite(vec, {0, 0, m_size.x, m_size.y}, 0);

		for(uint32_t i = 0; i < sprites.size(); ++i)
			PushSprite(vec, sprites[i], i+1);

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[a_vertex]);
		_gl glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(vec[0]), &vec[0], GL_STATIC_DRAW);
	}

//make textures
	{
		std::vector<glm::i16vec4> vec;
		vec.resize(sprites.size()+1);

		vec[0] = glm::i16vec4(sheet_size.x/2, sheet_size.y/2);

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[a_bounds]);
		_gl glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(vec[0]), &vec[0], GL_STATIC_DRAW);
	}

	{
		std::vector<glm::i16vec2> vec;
		vec.reserve(sprites.size()+1);

		vec.push_back({sheet_size.x/2, sheet_size.y/2});

		for(uint32_t i = 0; i < sprites.size(); ++i)
			vec.push_back({(sprites[i].x + sprites[i].z) / 2, (sprites[i].y + sprites[i].w) / 2});

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[a_centers]);
		_gl glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(vec[0]), &vec[0], GL_STATIC_DRAW);
	}

	GL_ASSERT;
}

void SpriteSheet::BindCenters      (GLViewWidget* gl, uint32_t active_texture)
{
	_gl glActiveTexture(active_texture);
	_gl glBindTexture(GL_TEXTURE_BUFFER, m_texture[t_centers]);
}

void SpriteSheet::BindBoundingBoxes(GLViewWidget* gl, uint32_t active_texture)
{
	_gl glActiveTexture(active_texture);
	_gl glBindTexture(GL_TEXTURE_BUFFER, m_texture[t_bounds]);
}

