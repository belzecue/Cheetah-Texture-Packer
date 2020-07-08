#include "packspritesheet.h"
#include <climits>


PackSpriteSheet::PackSpriteSheet(CountedSizedArray<glm::u16vec2> _sizes) :
	sizes(std::move(_sizes))
{
	uint32_t  optimal_area = SumArea(&sizes[0], sizes.size(), true);
	SheetMemo best         = CreateMemo(&sizes[0], 1, sizes.size());
	float     max         = optimal_area / (float) (best.size.x * best.size.y);


	for(uint32_t i = 2; i <= sizes.size(); ++i)
	{
		SheetMemo eax = CreateMemo(&sizes[0], i, sizes.size());
		float     value = optimal_area / (float) (eax.size.x * eax.size.y);




	}


	positions = CountedSizedArray<glm::u16vec2>(sizes.size());
	glm::u16vec2 row  {4, 4};

	for(uint32_t i = 0, j = 0; i < sizes.size(); ++i)
	{
		glm::u16vec2 rounded = (sizes[i] + glm::u16vec2(7, 7)) & glm::u16vec2(0xFFFC, 0xFFFC);

		if(row.x + rounded.x > best.size.x)
			row = glm::u16vec2(4, best.row_heights[++j]);

		positions[i] = row;
		row.x += rounded.x;
	}
}



uint32_t PackSpriteSheet::SumArea(glm::u16vec2 const* sprites, uint32_t no_sprites, bool padding)
{
	uint32_t r{};

	for(uint32_t i = 0; i < no_sprites; ++i)
	{
		if(padding) 	r += (sprites[i].x+4) * (sprites[i].y+4);
		else			r += sprites[i].x * sprites[i].y;
	}

	return r;
}


PackSpriteSheet::SheetMemo PackSpriteSheet::CreateMemo(glm::u16vec2 const* sprites, uint32_t first_cut, uint32_t no_sprites)
{
	uint16_t max_width{0};

	glm::u16vec2 row  {4, 4};
	glm::u16vec2 sheet{4, 4};

	for(uint32_t i = 0; i < first_cut; ++i)
	{
		glm::u16vec2 rounded = (sprites[i] + glm::u16vec2(7, 7)) & glm::u16vec2(0xFFFC, 0xFFFC);

		row.y = std::max<int>(rounded.y+4, row.y);
		row.x += rounded.x;
	}

	std::vector<int> row_start(1, 0);
	row_start.push_back(row.y);

	sheet = row;
	row = glm::u16vec2(4, 4);

	if(first_cut < no_sprites) row.x += sprites[first_cut].x;
	max_width = row.x;

	for(uint32_t i = first_cut; i < no_sprites; ++i)
	{
		glm::u16vec2 rounded = (sprites[i] + glm::u16vec2(7, 7)) & glm::u16vec2(0xFFFC, 0xFFFC);

		if(row.x + rounded.x > max_width)
		{
			row_start.push_back(row.y);

			sheet.x = std::max<int>(sheet.x, row.x+4);
			row = glm::u16vec2(0, 0);
		}

		row.y  = std::max<int>(rounded.y+4, row.y);
		row.x += rounded.x+4;
	}

	return { glm::u16vec2(sheet.x, sheet.y), std::move(row_start) };
}
