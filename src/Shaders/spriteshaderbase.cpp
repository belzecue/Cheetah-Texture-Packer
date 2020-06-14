#include "spriteshaderbase.h"
#include "src/widgets/glviewwidget.h"
#include "src/Sprite/document.h"
#include "defaulttextures.h"

void SpriteShaderBase::bindLayer(GLViewWidget* gl, int layer)
{
	float l = layer / (float) UCHAR_MAX;
	_gl glUniform1f(u_layer, l);
}

void SpriteShaderBase::bindMatrix(GLViewWidget* gl, glm::mat4x4 const& matrix)
{
	_gl glUniformMatrix4fv(u_object, 1, GL_FALSE, &matrix[0][0]);
}

void SpriteShaderBase::bindTextures(GLViewWidget* gl, Material * material)
{
	typedef Material::Tex Tex;

	if(!material)
	{
		_gl glActiveTexture(GL_TEXTURE0);
		_gl glBindTexture(GL_TEXTURE_2D, 0);

		_gl glActiveTexture(GL_TEXTURE1);
		_gl glBindTexture(GL_TEXTURE_2D, 0);

		_gl glActiveTexture(GL_TEXTURE2);
		_gl glBindTexture(GL_TEXTURE_2D, 0);

		_gl glActiveTexture(GL_TEXTURE3);
		_gl glBindTexture(GL_TEXTURE_2D, 0);
		return;
	}


	bool use_specular = !material->pbrSpecularGlossiness.is_empty;

	_gl glActiveTexture(GL_TEXTURE0);
	if(use_specular && material->image_slots[(int)Tex::Diffuse])
		_gl glBindTexture(GL_TEXTURE_2D, material->image_slots[(int)Tex::Diffuse]->GetTexture());
	else if(!use_specular && material->image_slots[(int)Tex::BaseColor])
		_gl glBindTexture(GL_TEXTURE_2D, material->image_slots[(int)Tex::BaseColor]->GetTexture());
	else
		_gl glBindTexture(GL_TEXTURE_2D, DefaultTextures::Get().GetWhiteTexture(gl));

	_gl glActiveTexture(GL_TEXTURE1);
	if(material->image_slots[(int)Tex::Normal])
		_gl glBindTexture(GL_TEXTURE_2D, material->image_slots[(int)Tex::Normal]->GetTexture());
	else
		_gl glBindTexture(GL_TEXTURE_2D, DefaultTextures::Get().GetNormalTexture(gl));

	_gl glActiveTexture(GL_TEXTURE2);
	if(use_specular && material->image_slots[(int)Tex::SpecularGlossiness])
		_gl glBindTexture(GL_TEXTURE_2D, material->image_slots[(int)Tex::SpecularGlossiness]->GetTexture());
	else if(!use_specular && material->image_slots[(int)Tex::MetallicRoughness])
		_gl glBindTexture(GL_TEXTURE_2D, material->image_slots[(int)Tex::MetallicRoughness]->GetTexture());
	else
		_gl glBindTexture(GL_TEXTURE_2D, DefaultTextures::Get().GetWhiteTexture(gl));

	_gl glActiveTexture(GL_TEXTURE3);
	if(material->image_slots[(int)Tex::Occlusion])
		_gl glBindTexture(GL_TEXTURE_2D, material->image_slots[(int)Tex::Occlusion]->GetTexture());
	else
		_gl glBindTexture(GL_TEXTURE_2D, DefaultTextures::Get().GetWhiteTexture(gl));
}
