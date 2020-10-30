#include "defaultvaos.h"
#include "src/widgets/glviewwidget.h"
#include <glm/gtc/type_precision.hpp>
#include <atomic>
#include <vector>
#include <mutex>

namespace glDefaultVAOs
{


struct DefaultVAOs
{
	GLuint vao{};
	GLuint vbo{};
	GLuint index_vbo{};

	mutable std::mutex       mutex;
	mutable std::atomic<int> refCount{0};
};

static DefaultVAOs s_BlitStruct;

void AddRef()
{
	++s_BlitStruct.refCount;
}

void Release(GLViewWidget * gl)
{
	if(!--s_BlitStruct.refCount && s_BlitStruct.vao)
	{
		std::lock_guard<std::mutex> lock(s_BlitStruct.mutex);

		if(s_BlitStruct.refCount == 0 && s_BlitStruct.vao)
		{
            _gl glDeleteVertexArrays(1, &s_BlitStruct.vao);
            _gl glDeleteBuffers(1, &s_BlitStruct.vbo);

			s_BlitStruct.vao = 0;
			s_BlitStruct.vbo = 0;
		}
	}
}

void RenderPoint(GLViewWidget * gl)
{
    _gl glDrawArrays(GL_POINTS, 0, 1);
}

void RenderSquare(GLViewWidget * gl)
{
    _gl glDrawArrays(GL_TRIANGLES, 1, 6);
}

void RenderCube(GLViewWidget * gl)
{
    _gl glDrawArrays(GL_TRIANGLES, 7, 36);
}

struct gltfVertex
{
	glm::i8vec4 position;
	glm::i8vec4 normal;
	glm::i8vec4 texCoord0;
};

void BindVAO(GLViewWidget * gl)
{
	assert(s_BlitStruct.refCount != 0);

	if(s_BlitStruct.vao)
	{
        _gl glBindVertexArray(s_BlitStruct.vao);
		return;
	}

	std::lock_guard<std::mutex> lock(s_BlitStruct.mutex);
	assert(s_BlitStruct.refCount != 0);

	const glm::i8vec4 positions[] =
	{
		{ -1,-1, 0, 1 },  {  1,-1, 0, 1 },  { -1, 1, 0, 1 },
		{ -1, 1, 0, 1 },  {  1,-1, 0, 1 },  {  1, 1, 0, 1 },

		{ -1,-1,-1, 1 },  { -1,-1, 1, 1 },  { -1, 1, 1, 1 },
		{  1, 1,-1, 1 },  { -1,-1,-1, 1 },  { -1, 1,-1, 1 },

		{  1,-1, 1, 1 },  { -1,-1,-1, 1 },  {  1,-1,-1, 1 },
		{  1, 1,-1, 1 },  {  1,-1,-1, 1 },  { -1,-1,-1, 1 },

		{ -1,-1,-1, 1 },  { -1, 1, 1, 1 },  { -1, 1,-1, 1 },
		{  1,-1, 1, 1 },  { -1,-1, 1, 1 },  { -1,-1,-1, 1 },

		{ -1, 1, 1, 1 },  { -1,-1, 1, 1 },  {  1,-1, 1, 1 },
		{  1, 1, 1, 1 },  {  1,-1,-1, 1 },  {  1, 1,-1, 1 },

		{  1,-1,-1, 1 },  {  1, 1, 1, 1 },  {  1,-1, 1, 1 },
		{  1, 1, 1, 1 },  {  1, 1,-1, 1 },  { -1, 1,-1, 1 },

		{  1, 1, 1, 1 },  { -1, 1,-1, 1 },  { -1, 1, 1, 1 },
		{  1, 1, 1, 1 },  { -1, 1, 1, 1 },  {  1,-1, 1, 1 },
	};

	std::vector<gltfVertex> vert_array;
	vert_array.resize(43);
	vert_array[0].normal = glm::i8vec4(0, 0, 127, 0);

	auto verts = &vert_array[1];
	for(int tri = 0; tri < 14; ++tri)
	{
		glm::ivec4 p0 = positions[tri*3+0];
		glm::ivec4 p1 = positions[tri*3+1];
		glm::ivec4 p2 = positions[tri*3+2];

		glm::ivec4 u = p1 - p0;
		glm::ivec4 v = p2 - p0;

		glm::ivec4 normal(
			u.y*v.z - u.z*v.y,
			u.z*v.x - u.x*v.z,
			u.x*v.y - u.y*v.x,
			0);

		for(int i = 0; i < 3; ++i)
		{
			verts[tri*3+i].position = positions[tri*3+i];
			verts[tri*3+i].normal   = normal;
			verts[tri*3+i].texCoord0= (positions[tri*3+i] + (int8_t)1) / (int8_t)2;
		}
	}

    _gl glGenVertexArrays(1, &s_BlitStruct.vao);
    _gl glGenBuffers(1, &s_BlitStruct.vbo);

    _gl glBindVertexArray(s_BlitStruct.vao);
    _gl glBindBuffer(GL_ARRAY_BUFFER, s_BlitStruct.vbo);

    _gl glBufferData(GL_ARRAY_BUFFER, vert_array.size() * sizeof(gltfVertex), &vert_array[0], GL_STATIC_DRAW);

	//position
    _gl glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(gltfVertex), (void*) offsetof(gltfVertex, position));
	//normal
    _gl glVertexAttribPointer(1, 3, GL_BYTE, GL_FALSE, sizeof(gltfVertex), (void*) offsetof(gltfVertex, normal));
//	glVertexAttribPointer(2, 4, GL_BYTE, GL_FALSE, sizeof(gltfVertex), (void*) 8);
	//texcoord0
    _gl glVertexAttribPointer(2, 2, GL_BYTE, GL_FALSE, sizeof(gltfVertex), (void*) offsetof(gltfVertex, texCoord0));

    _gl glEnableVertexAttribArray(0);
    _gl glEnableVertexAttribArray(1);
    _gl glEnableVertexAttribArray(3);
}


void BindSquareIndexVBO(GLViewWidget * gl)
{
	if(s_BlitStruct.index_vbo != 0)
	{
		_gl glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_BlitStruct.index_vbo);
		return;
	}

	std::vector<uint16_t> buffer;
	buffer.resize(65536);

	for(uint32_t i = 0, j = 0; i+6 < buffer.size(); i += 6, j += 4)
	{
		buffer[i+0] = j+0;
		buffer[i+1] = j+1;
		buffer[i+2] = j+3;
		buffer[i+3] = j+3;
		buffer[i+4] = j+1;
		buffer[i+5] = j+2;
	}

	_gl glGenBuffers(1, &s_BlitStruct.index_vbo);
	_gl glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_BlitStruct.index_vbo);
	_gl glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.size() * sizeof(buffer[0]), &buffer[0], GL_STATIC_DRAW);
}

}
