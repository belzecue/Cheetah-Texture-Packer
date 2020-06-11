#include "spritesheet.h"
#include "src/widgets/glviewwidget.h"

SpriteSheet::SpriteSheet()
{

}

void SpriteSheet::Prepare(GLViewWidget*, CountedSizedArray<glm::i16vec4> sprites)
{
	if(m_sprites == sprites.data())
		return;

	m_sprites = sprites.data();

	if(!m_vao)
	{


	}


}
