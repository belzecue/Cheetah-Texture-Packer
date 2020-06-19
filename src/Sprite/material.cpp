#include "material.h"
#include "src/widgets/glviewwidget.h"
#include "src/Support/vectoroperations.hpp"
#include "Shaders/defaultvaos.h"
#include "Shaders/gltfmetallicroughness.h"
#include "Shaders/transparencyshader.h"
#include "Shaders/blitshader.h"
#include <glm/gtc/matrix_transform.hpp>
#include "spritesheet.h"

#define UNUSED(x) (void)x;


Material::Material()
{
	pbrMetallicRoughness.baseColorTexture.texCoord = 2 + (int)Tex::BaseColor;
	pbrSpecularGlossiness.diffuseTexture.texCoord  = 2 + (int)Tex::Diffuse;
	pbrMetallicRoughness.metallicRoughnessTexture.texCoord = 2 + (int)Tex::MetallicRoughness;
	pbrSpecularGlossiness.specularGlossinessTexture.texCoord = 2 + (int)Tex::SpecularGlossiness;
	normalTexture.texCoord    = 2 + (int)Tex::Normal;
	occlusionTexture.texCoord = 2 + (int)Tex::Occlusion;
	emissiveTexture.texCoord  = 2 + (int)Tex::Emission;

}

void Material::Clear(GLViewWidget * gl)
{
	if(m_spriteSheet)
		m_spriteSheet->Clear(gl);

	if(m_vao)
	{
		_gl glDeleteBuffers(VBOc, m_vbo);
		_gl glDeleteVertexArrays(1, &m_vao);

		memset(m_vbo, 0, sizeof(m_vbo));
		m_vao = 0;
	}
}

void Material::SetImage(counted_ptr<Image> image, counted_ptr<Image> * slot)
{
	int tex = (slot - &image_slots[0]);

	if(!(0 <= tex && tex < (int)Tex::Total))
	{
		throw std::runtime_error("bad texture pointer...");
	}

	std::string error = IsImageCompatible((Tex)tex, image);

	if(!error.empty())
		throw std::logic_error(error);

	if(image == image_slots[tex])
		return;

	image_slots[tex] = image;
	m_vboFlags |= (1 << tex);

	if((Tex)tex <= current_slot
	|| current_slot == Tex::None)
	{
		current_slot = (Tex)tex;

		m_sprites           = image->m_sprites;
		m_crop              = image->m_cropped;
		m_normalizedCrop    = image->m_normalized;
		m_normalizedSprites = image->m_normalizedPositions;

		m_spriteCount = m_sprites.size();
		m_sheetSize   = image->GetSize();
		m_dirty = true;
	}
	else
	{
	//try to optimize memory
		image->m_sprites.merge(m_sprites);
		image->m_cropped.merge(m_crop);
		image->m_normalized.merge(m_normalizedCrop);
		image->m_normalizedPositions.merge(m_normalizedSprites);
	}
}

std::string Material::IsImageCompatible(Material::Tex tex, counted_ptr<Image> image)
{
	UNUSED(tex);

	if(image == nullptr)
		return {};

	image->LoadFromFile();

//first one so anything goes
	if(current_slot == Tex::None)
		return {};

	if(m_sprites.size() != image->m_sprites.size())
		return "number of sprites in image does not match number in material.";

//check compatibility...
	if(image->m_normalizedPositions.merge(m_normalizedSprites))
	{
		image->m_sprites.merge(m_sprites);
		return {};
	}

	return "sprites in image do not properly align with sprites in material.";
}

void Material::CreateDefaultArrays(GLViewWidget* gl)
{
	if(!m_normalizedPositions.empty() || m_spriteCount == 0)
		return;

	m_normalizedPositions = CountedSizedArray<glm::vec2>(m_spriteCount * 4);
	m_spriteIndices       = CountedSizedArray<Pair>(m_spriteCount);
	m_spriteVertices      = CountedSizedArray<Pair>(m_spriteCount);

	for(uint32_t i = 0; i < m_spriteCount; ++i)
		m_spriteIndices[i] = {(uint16_t)(i*6), 6};

	for(uint32_t i = 0; i < m_spriteCount; ++i)
		m_spriteVertices[i] = {(uint16_t)(i*4), 4};

	for(uint32_t i = 0; i < m_spriteCount; ++i)
	{
		m_normalizedPositions[i*4+0] = glm::vec2(-1.f,  1.f);
		m_normalizedPositions[i*4+1] = glm::vec2( 1.f,  1.f);
		m_normalizedPositions[i*4+2] = glm::vec2( 1.f, -1.f);
		m_normalizedPositions[i*4+3] = glm::vec2(-1.f, -1.f);
	}

	CreateIdBuffer(gl);

	{
		auto indices = CountedSizedArray<short>(m_spriteCount * 6);

		for(uint32_t i = 0; i < m_spriteCount; ++i)
		{
			indices[i*6+0] = i*4+0;
			indices[i*6+1] = i*4+1;
			indices[i*6+2] = i*4+3;

			indices[i*6+3] = i*4+3;
			indices[i*6+4] = i*4+1;
			indices[i*6+5] = i*4+2;
		}

		_gl glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[v_indices]); DEBUG_GL
		_gl glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_DYNAMIC_DRAW); DEBUG_GL
	}

//create null textures
	_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[v_texCoord]); DEBUG_GL
	_gl glBufferData(GL_ARRAY_BUFFER, m_spriteCount * 4 * sizeof(glm::u16vec4), nullptr, GL_DYNAMIC_DRAW); DEBUG_GL

//create positions

	{
		auto vec = CountedSizedArray<glm::i16vec2>(m_spriteCount * 4);

		for(uint32_t i = 0; i < m_spriteCount; ++i)
		{
			glm::ivec2 center = SpriteSheet::GetCenter(m_sprites[i]);
			glm::ivec4 crop = glm::ivec4(m_crop[i]) - glm::ivec4(center, center);

			vec[i*4+0] = glm::i16vec2(crop.x, crop.y);
			vec[i*4+1] = glm::i16vec2(crop.z, crop.y);
			vec[i*4+2] = glm::i16vec2(crop.z, crop.w);
			vec[i*4+3] = glm::i16vec2(crop.x, crop.w);
		}

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[v_positions]); DEBUG_GL
		_gl glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(vec[0]), &vec[0], GL_DYNAMIC_DRAW); DEBUG_GL
	}
}

void Material::CreateIdBuffer(GLViewWidget* gl)
{
	std::vector<short> array(m_normalizedPositions.size(), 0);

	for(uint32_t i = 0; i < m_spriteCount; ++i)
	{
		auto pair   = m_spriteVertices[i];

		for(uint32_t k = 0; k < pair.length; ++k)
		{
			array[k+pair.start] = i+1;
		}
	}

	_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[v_spriteId]); DEBUG_GL
	_gl glBufferData(GL_ARRAY_BUFFER, array.size() * sizeof(array[0]), &array[0], GL_DYNAMIC_DRAW); DEBUG_GL
}

void Material::Prepare(GLViewWidget* gl)
{
	if(!m_vao)
	{
		_gl glGenVertexArrays(1, &m_vao);
		_gl glGenBuffers(VBOc, &m_vbo[0]);

//sprite sheet
		_gl glBindVertexArray(m_vao);
		_gl glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[v_indices]);

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[v_positions]);
		_gl glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, 0, nullptr);
		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[v_spriteId]);
		_gl glVertexAttribIPointer(1, 1, GL_SHORT, 0, nullptr);

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[v_texCoord]);
		_gl glVertexAttribPointer(2, 2, GL_UNSIGNED_SHORT, GL_TRUE, 8, nullptr);
		_gl glVertexAttribPointer(3, 2, GL_UNSIGNED_SHORT, GL_TRUE, 8, (void*)4);

		for(int i = 0; i < (int)Tex::Total; ++i)
		{
			_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[v_sheetCoordBegin+i]);
			_gl glVertexAttribPointer(4+i, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		}

		for(int i = 0; i < v_indices; ++i)
			_gl glEnableVertexAttribArray(i);

		GL_ASSERT;
	}

//upload data
	if(m_dirty != false)
	{
		m_dirty = false;
	}

	CreateDefaultArrays(gl);

	if(m_vboFlags)
	{
		auto flags = m_vboFlags;
		m_vboFlags = 0;

		for(int i = 0; i < (int)Tex::Total; ++i)
		{
			if(!(flags >> i)) continue;

			if(image_slots[i] == nullptr)
			{
				_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[v_sheetCoordBegin+i]);
				_gl glBufferData(GL_ARRAY_BUFFER, m_normalizedPositions.size() * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
			}

			std::vector<glm::vec2> coords(m_normalizedPositions.size());

			for(uint32_t j = 0; j < m_normalizedPositions.size(); ++j)
			{
				glm::vec2 pos = m_normalizedPositions[j];
				pos.y *= -1;
				coords[j] = (pos + 1.f) / 2.f;
			}

			for(uint32_t j = 0; j < m_spriteCount; ++j)
			{
				auto square = image_slots[i]->m_sprites[j];
				auto pair   = m_spriteVertices[j];

				for(uint32_t k = 0; k < pair.length; ++k)
				{
					auto & v = coords[k+pair.start];
					v = glm::mix(glm::vec2(square.x, square.y), glm::vec2(square.z, square.w), v);
				}
			}

			_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[v_sheetCoordBegin+i]);
			_gl glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(coords[0]), &coords[0], GL_DYNAMIC_DRAW);
		}
	}

	DEBUG_GL
}

void Material::RenderObjectSheet(GLViewWidget * gl, int frame)
{
	if(isUnlit())
	{
		for(int i = 0; i < (int)Tex::Total; ++i)
		{
			if(image_slots[i])
			{
				RenderSpriteSheet(gl, (Tex)i, frame);
				return;
			}
		}

		return;
	}

	Prepare(gl);
	auto db = GetRenderData(frame);
	RenderSheetBackdrop(gl, db);

	/*
	//draw sprites
	_gl glDisable(GL_DEPTH_TEST);
	gltfMetallicRoughness::Shader.bind(gl, this);
	gltfMetallicRoughness::Shader.bindCenter(gl, db.center);
	gltfMetallicRoughness::Shader.bindMatrix(gl, db.matrix);

	gltfMetallicRoughness::Shader.bindLayer(gl, 4);
	_gl glDrawElements(GL_TRIANGLES, db.fr_elements, GL_UNSIGNED_SHORT, db.fr_offset());
*/

	GL_ASSERT;
}


void Material::RenderSpriteSheet(GLViewWidget * gl, Material::Tex image_slot, int frame)
{
	if(image_slots[(int)image_slot] == nullptr) return;

	Prepare(gl);
	auto db = GetRenderData(frame);
	RenderSheetBackdrop(gl, db);

	_gl glBindVertexArray(m_vao);

	BlitShader::Shader.bind(gl, this);
	if(!db.center) m_spriteSheet->BindCenters(gl, GL_TEXTURE10);
	BlitShader::Shader.bindMatrix(gl, db.matrix);

	BlitShader::Shader.bindLayer(gl, 8);
	BlitShader::Shader.bindColor(gl, glm::vec4(1, 1, 1, 1)); DEBUG_GL
	_gl glDisable(GL_BLEND);
	_gl glDrawElements(GL_TRIANGLES, db.elements, GL_UNSIGNED_SHORT, db.offset());

}

RenderData Material::GetRenderData(int frame)
{
	RenderData r;

	if(frame >= 0) frame %= m_sprites.size();

	r.frame       = frame;
	r.elements    = 0;
	r.first       = 0;
	r.center      = (frame >= 0);

	r.matrix      = glm::mat4(1);
//	if(!r.center) r.matrix = glm::translate(r.matrix, -glm::vec3(m_sheetSize.x/2.f, m_sheetSize.y/2.f, 0));

	if(!m_spriteIndices.empty())
	{
		r.elements = frame >= 0? m_spriteIndices[frame].length : m_spriteIndices.back().end();
		r.first    = frame >= 0? m_spriteIndices[frame].start  : 0;
	}

	return r;
}

void Material::RenderSheetBackdrop(GLViewWidget * gl, RenderData const& db)
{
	if(m_spriteSheet == nullptr)
		m_spriteSheet.reset(new SpriteSheet());

	m_spriteSheet->Prepare(gl, m_sprites, m_sheetSize);
//	m_spriteSheet->RenderSheet(gl, db);
}
