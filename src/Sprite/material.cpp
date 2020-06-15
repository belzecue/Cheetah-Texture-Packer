#include "material.h"
#include "src/widgets/glviewwidget.h"
#include "src/Support/vectoroperations.hpp"
#include "Shaders/defaultvaos.h"
#include "Shaders/gltfmetallicroughness.h"
#include <glm/gtc/matrix_transform.hpp>
#include "spritesheet.h"

#define UNUSED(x) (void)x;

void Material::Clear(GLViewWidget * gl)
{
	if(m_spriteSheet)
		m_spriteSheet->Clear(gl);
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


void BindCenters(uint32_t active_texture)
{

}

void Material::Prepare(GLViewWidget* gl)
{

#if 0

	if(!m_vao[0])
	{
		_gl glGenVertexArrays(VAOc, &m_vao[0]);
		_gl glGenBuffers(VBOc, &m_vbo[0]);

//sprite sheet
		_gl glBindVertexArray(m_vao[SpriteSheet]);
		glDefaultVAOs::BindSquareIndexVBO(gl);

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[v_SheetVertex]);
		_gl glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, 0, nullptr);

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[v_SheetCenters]);
		_gl glVertexAttribPointer(1, 2, GL_SHORT, GL_FALSE, 0, nullptr);

		_gl glEnableVertexAttribArray(0);
		_gl glEnableVertexAttribArray(1);

		GL_ASSERT;
	}

//upload data
	if(m_dirty == false) return;
	m_dirty = false;

//backdrop



//sprite centers
//

	if(!m_vao[0])
	{
	_gl glGenVertexArrays(VAOc, &m_vao[0]);
	_gl glGenBuffers(VBOc, &m_vbo[0]);

	_gl glBindVertexArray(m_vao[0]);
	glDefaultVAOs::BindSquareIndexVBO(gl);GL_ASSERT;

	_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[SpriteCoords]);
	_gl glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, 0, nullptr);GL_ASSERT;

	_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[Centers]);
	_gl glVertexAttribPointer(1, 2, GL_SHORT, GL_FALSE, 0, nullptr);GL_ASSERT;

	_gl glEnableVertexAttribArray(0);
	_gl glEnableVertexAttribArray(1);GL_ASSERT;

	_gl glBindVertexArray(m_vao[1]);
	glDefaultVAOs::BindSquareIndexVBO(gl);GL_ASSERT;

	_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[CropBoxes]);
	_gl glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, 0, nullptr);GL_ASSERT;

	_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[Centers]);
	_gl glVertexAttribPointer(1, 2, GL_SHORT, GL_FALSE, 0, nullptr);GL_ASSERT;

	_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[TexCoord]);
	_gl glVertexAttribPointer(2, 2, GL_UNSIGNED_SHORT, GL_TRUE, 0, nullptr);GL_ASSERT;

	_gl glEnableVertexAttribArray(0);
	_gl glEnableVertexAttribArray(1);
	_gl glEnableVertexAttribArray(3);GL_ASSERT;

//upload perminant things
	_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[SpriteCoords]);
	UploadTextureData(gl, m_sprites, glm::i16vec4(0, 0, m_size.x, m_size.y), m_size.y);GL_ASSERT;

	std::vector<glm::i16vec2> vec;
	vec.reserve((m_sprites.size()+1));GL_ASSERT;

	vec.push_back({m_size.x/2, m_size.y/2});
	vec.push_back({m_size.x/2, m_size.y/2});
	vec.push_back({m_size.x/2, m_size.y/2});
	vec.push_back({m_size.x/2, m_size.y/2});

	for(uint32_t i = 0; i < m_sprites.size(); ++i)
	{
		vec.push_back({(m_sprites[i].x + m_sprites[i].z)/2, m_size.y - (m_sprites[i].y + m_sprites[i].w)/2});
		vec.push_back({(m_sprites[i].x + m_sprites[i].z)/2, m_size.y - (m_sprites[i].y + m_sprites[i].w)/2});
		vec.push_back({(m_sprites[i].x + m_sprites[i].z)/2, m_size.y - (m_sprites[i].y + m_sprites[i].w)/2});
		vec.push_back({(m_sprites[i].x + m_sprites[i].z)/2, m_size.y - (m_sprites[i].y + m_sprites[i].w)/2});
	}

	_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[Centers]);
	_gl glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(vec[0]), &vec[0], GL_STATIC_DRAW);GL_ASSERT;
}

_gl glBindVertexArray(m_vao[0]);

_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[CropBoxes]);
UploadTextureData(gl, m_cropped, glm::i16vec4(0, 0, m_size.x, m_size.y), m_size.y);GL_ASSERT;

_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[TexCoord]);
UploadTextureData(gl, m_normalized, glm::u16vec4(0, 0, USHRT_MAX, USHRT_MAX));GL_ASSERT;
#endif
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
/*
	BlitShader::Shader.bind(gl, this);
	BlitShader::Shader.bindCenter(gl, db.center);
	BlitShader::Shader.bindMatrix(gl, db.matrix);

	BlitShader::Shader.bindLayer(gl, 4);
	_gl glDrawElements(GL_TRIANGLES, db.fr_elements, GL_UNSIGNED_SHORT, db.fr_offset());
	*/
}

RenderData Material::GetRenderData(int frame)
{
	RenderData r;

	if(frame >= 0) frame %= m_sprites.size();

	r.frame       = frame;
	r.elements    = frame < 0? 6 * m_sprites.size() : 6;
	r.first       = 6 * (1 + frame * (frame > 0));
	r.center      = (frame >= 0);

	r.matrix      = glm::mat4(1);
//	if(!r.center) r.matrix = glm::translate(r.matrix, -glm::vec3(m_sheetSize.x/2.f, m_sheetSize.y/2.f, 0));

	if(!m_spriteIndices.empty())
	{
		r.elements = frame < 0? m_spriteIndices.back().end() : m_spriteIndices.back().length;
		r.first    = frame < 0? 0 : m_spriteIndices[frame].start;
	}

	return r;
}

void Material::RenderSheetBackdrop(GLViewWidget * gl, RenderData const& db)
{
	if(m_spriteSheet == nullptr)
		m_spriteSheet.reset(new SpriteSheet());

	m_spriteSheet->Prepare(gl, m_sprites, m_sheetSize);
	m_spriteSheet->RenderSheet(gl, db);
}
