#ifndef PACKSPRITESHEET_H
#define PACKSPRITESHEET_H
#include <glm/vec2.hpp>
#include <glm/gtc/type_precision.hpp>
#include "Support/countedsizedarray.hpp"
#include <vector>

struct PackSpriteSheet
{
	PackSpriteSheet(CountedSizedArray<glm::u16vec2> sizes);

	glm::u16vec2 size{0, 0};
	glm::u8vec4  border_color{0, 0, 0, 0};

	CountedSizedArray<glm::u16vec2> positions;
	CountedSizedArray<glm::u16vec2> sizes;



private:
	struct SheetMemo
	{
		glm::u16vec2 size;
		std::vector<int> row_heights;
	};

	static uint32_t SumArea(glm::u16vec2 const* sprites, uint32_t no_sprites, bool padding);
	static SheetMemo CreateMemo(glm::u16vec2 const* sprites, uint32_t first_cut, uint32_t no_sprites);


};

#endif // PACKSPRITESHEET_H
