#include "material.h"
#include "src/widgets/glviewwidget.h"
#incldue "src/support/vectoroperations.hpp"

#define UNUSED(x) (void)x;

void Material::SetImage(Material::Tex tex, counted_ptr<Image> image)
{
	std::string error = IsImageCompatible(tex, image);

	if(!error.empty())
		throw std::logic_error(error);

	image_slots[(int)tex] = image;

	if(tex <= current_slot
	|| current_slot == Tex::None)
	{
		current_slot = tex;

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


void Material::Prepare(GLViewWidget* gl)
{
	if(m_dirty == false)
		return;

	m_dirty = false;

	if(!m_vao[0])
	{
		_gl glGenVertexArrays(VAOc, &m_vao[0]);
		_gl glGenBuffers(VBOc, &m_vbo[0]);

//sprite padding
		_gl glBindVertexArray(m_vao[0]);
		glDefaultVAOs::BindSquareIndexVBO(gl);GL_ASSERT;
	}



}
