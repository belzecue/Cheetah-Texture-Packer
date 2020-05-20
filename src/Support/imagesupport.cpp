#include "widgets/glviewwidget.h"

#include "Support/glm_iostream.hpp"

#include "basisu_enc.h"
#include "imagesupport.h"
#include "lf_math.h"
#include "lodepng.h"
#include "apg_bmp.h"
#include "jpgd.h"

#include <iostream>
#include <glm/glm.hpp>
#include <cctype>
#include <cstring>


const char * g_ReadMimeTypes[] =
{
"image/jpg",
"image/png",
"image/bmp"
""
};

const char * g_WriteMimeTypes[] =
{
"image/png",
"image/bmp"
""
};


IO::Image IO::LoadImage(const char * path)
{
	IO::Image image;

	switch(GetFormat(path))
	{
	case IO::Format::Unknown:
		throw std::runtime_error(std::string(path) + "\n Unkown Image Format");
	case IO::Format::None:
		return image;
	case IO::Format::BMP:
	{
		uint32_t compz{};
		int width{}, height{};
		auto data = apg_bmp_read(path, &width, &height, &compz);

		image.image.reset(data);
		image.size = glm::ivec2(width, height);
		image.channels = compz;
		return image;
	}
	case IO::Format::JPEG:
	{
		int compz, width, height;
		auto data = jpgd::decompress_jpeg_image_from_file(path, &width, &height, &compz, 4, 0);

		image.image.reset(data);
		image.size = glm::ivec2(width, height);
		image.channels = compz;
		break;
	}
	case IO::Format::PNG:
	{
		uint32_t x, y;
		uint8_t * data{nullptr};
		auto r = lodepng_decode32_file(&data, &x, &y, path);

		if(r != 0)
			throw std::runtime_error("problem reading png");

		image.image.reset(data);
		image.size = glm::ivec2(x, y);
		image.channels = 4;
		break;
	} break;
	default:
		throw std::logic_error("unhandled image format");
	}

	return image;
}

void IO::SaveImage(const char * path, uint8_t * data, glm::i16vec2 size, int channels)
{
	switch(GetFormat(path))
	{
	case IO::Format::Unknown:
		throw std::runtime_error("Problem saving: " + std::string(path) + "\n Unknown image format.");
	case IO::Format::None:
		return;
	case IO::Format::BMP:
		if(!apg_bmp_write(path, data, size.x, size.y, channels))
			throw std::runtime_error("Problem saving bitmap: " + std::string(path));
		break;
	case IO::Format::JPEG:
	{
		throw std::runtime_error("Problem saving: " + std::string(path) + "\n Saving jpegs is unsupported at this time.");
		break;
	}
	case IO::Format::PNG:
	{
		LodePNGColorType types[] = { LCT_GREY, LCT_GREY_ALPHA, LCT_RGB, LCT_RGBA };
		if(lodepng_encode_file(path, data, size.x, size.y, types[channels % 4], 8))
			throw std::runtime_error("Problem saving png: " + std::string(path));
	} break;
	default:
		throw std::logic_error("unhandled image format");
	}
}

IO::Format IO::GetFormat(const char * path, int length)
{
	struct FormatTable
	{
		const char * extension;
		uint8_t        length;
		IO::Format format;
	};

	static FormatTable table[] =
	{
	{ ".bmp",   4, IO::Format::BMP },
	{ ".jpg",   4, IO::Format::JPEG },
	{ ".jpeg",  5, IO::Format::JPEG },
	{ ".png",   4, IO::Format::PNG },
	//{ ".tga", 4, IO::Format::TGA },
	{ nullptr,  0, IO::Format::None }
	};

	if(path == nullptr)
		return Format::None;

	for(FormatTable * fmt = table; fmt->extension; ++fmt)
	{
//		std::cerr << "checking format: " << fmt->extension << std::endl;

		auto ext = fmt->extension + fmt->length;
		auto pth = path + (length >= 0? length : strlen(path));

		for(;ext >= fmt->extension && pth >= path; --pth, --ext)
		{
			if(*ext != tolower(*pth))
				break;

			if(*ext == '.')
				return fmt->format;
		}
	}

	return Format::Unknown;
}

void IO::UploadImage(GLViewWidget * gl, uint32_t * texture, uint8_t * data, glm::i16vec2 size, int channels)
{
	GL_ASSERT;

	(void)gl;

	if(texture == nullptr)
		return;

	if(*texture == 0)
		_gl glGenTextures(1, texture);

	_gl glBindTexture(GL_TEXTURE_2D, *texture);

	_gl glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	_gl glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	_gl glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	_gl glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	_gl glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	GLenum type[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };

	_gl glTexImage2D(GL_TEXTURE_2D,
			0,
			type[channels-1],
			size.x,
			size.y,
			0,
			type[channels-1],
			GL_UNSIGNED_BYTE,
			&data[0]);

	GL_ASSERT;
}

CountedSizedArray<glm::i16vec4> IO::GetSprites(uint8_t * data, glm::i16vec2 size, int channels)
{
	std::vector<glm::i16vec4> r;

	uint32_t color;
	memcpy(&color, &data[0], channels);

	auto ComparePx = [=](int x, int y)
	{
		return !memcmp(&color, &data[(y * size.x + x)*channels], channels);
	};

	for(int y = 0; y < size.y; ++y)
		for(int x = 0; x < size.x; ++x)
		{
			if(ComparePx(x, y))
				continue;

//top left corner i guess?
			if(y == 0 || ComparePx(x, y-1))
			{
				r.push_back(GetSprite(data, size, channels, glm::i16vec2(x, y), color));
				x = r.back().z;
				continue;
			}

			for(auto j = r.rbegin(); j != r.rend(); ++j)
			{
				if(math::contains(*j, glm::i16vec2(x, y)))
				{
					x = j->z;
					goto end;
				}
				else if(j->w < y)
					break;
			}

//			throw std::logic_error("missed a sprite?");
end:
			(void)0;
		}

	std::cerr << "total sprites found: " << r.size() << std::endl;

	return CountedSizedArray<glm::i16vec4>::FromArray(&r[0], r.size());
}

CountedSizedArray<glm::i16vec4> IO::GetCrop(uint8_t * data, glm::i16vec2 size, int channels, const CountedSizedArray<glm::i16vec4> sprites)
{
	CountedSizedArray<glm::i16vec4> r(sprites.size());

	uint32_t mask = channels == 4? 0x000000FF : 0xFFFFFF00;

	for(uint32_t i = 0; i < sprites.size(); ++i)
	{
		r[i] = GetCrop(data, size, channels, sprites[i], mask, 0);
	}

	return r;
}

CountedSizedArray<glm::u16vec4> IO::NormalizeCrop(CountedSizedArray<glm::i16vec4> sprites, glm::i16vec2 size)
{
	CountedSizedArray<glm::u16vec4> r(sprites.size());

	size = glm::max(size, glm::i16vec2(1, 1));

	for(uint32_t i = 0; i < sprites.size(); ++i)
	{
		glm::dvec4 uv(
			sprites[i].x / (double) size.x,
			sprites[i].y / (double) size.y,
			sprites[i].z / (double) size.x,
			sprites[i].w / (double) size.y);

		uv = glm::clamp(uv, glm::dvec4(0), glm::dvec4(1));
		r[i] = uv * (double) USHRT_MAX;
	}

	return r;
}


glm::i16vec4 IO::GetSprite(uint8_t * data, glm::i16vec2 size, int channels, glm::i16vec2 tl, uint32_t color)
{
	glm::i16vec2 i  = tl;
	glm::i16vec2 br = size;

	auto ComparePx = [=](int x, int y)
	{
		return !memcmp(&color, &data[(y * size.x + x)*channels], channels);
	};

//scan top
	for(; i.x < size.x; ++i.x)
	{
		if(i.y > 0 && !ComparePx(i.x, i.y-1))
			throw std::runtime_error("sprite beginning at " + std::to_string(tl) + " does not have a straight upper edge." );

		if(ComparePx(i.x, i.y))
		{
			br.x = i.x;
			break;
		}
	}

//scan left
	for(i = tl; i.y < size.y; ++i.y)
	{
		if(i.x > 0 && !ComparePx(i.x-1, i.y))
			throw std::runtime_error("sprite beginning at " + std::to_string(tl) + " does not have a straight left edge." );

		if(ComparePx(i.x, i.y))
		{
			br.y = i.y;
			break;
		}
	}

	if(br.x < size.x && br.y+1 < size.y
	&& !ComparePx(br.x, br.y))
		throw std::runtime_error("sprite beginning at " + std::to_string(tl) + " is not proper square." );


//verify bottom
	for(i = glm::ivec2(tl.x, br.y); i.x < br.x; ++i.x)
	{
		if((i.y+1 < size.y && !ComparePx(i.x, i.y))
		|| ComparePx(i.x, i.y-1))
			throw std::runtime_error("sprite beginning at " + std::to_string(tl) + " does not have a straight bottom edge." );
	}

//verify right
	for(i = glm::ivec2(br.x, tl.y); i.y < br.y; ++i.y)
	{
		if((i.x+1 < size.x && !ComparePx(i.x, i.y))
		|| ComparePx(i.x-1, i.y))
			throw std::runtime_error("sprite beginning at " + std::to_string(tl) + " does not have a straight right edge." );
	}

	std::cerr << "found sprite: " << glm::i16vec4(tl.x, tl.y, br.x, br.y) << std::endl;

	return glm::i16vec4(tl.x, tl.y, br.x, br.y);
}

glm::i16vec4 IO::GetCrop  (uint8_t const* data, glm::i16vec2 size, int channels, glm::i16vec4 aabb, uint32_t greenMask, uint32_t greenScreen)
{
	glm::i16vec2 min{aabb.z - aabb.x, aabb.w - aabb.y};
	glm::i16vec2 max{0, 0};
	glm::i16vec2 N = min;

	uint32_t px   = 0x000000FF;

	for(short y = 0; y < N.y; ++y)
	{
		auto row = &data[((y+aabb.y) *size.x + aabb.x)*channels];

		for(short x = 0; x < N.x; ++x)
		{
			memcpy(&px, &row[x*channels], channels);

			if((px & greenMask) != greenScreen)
			{
				min = glm::min(min, glm::i16vec2(x, y));
				max = glm::max(max, glm::i16vec2(x, y));
			}
		}
	}

	return glm::i16vec4(glm::min(min, max) + glm::i16vec2(aabb.x, aabb.y), max + glm::i16vec2(aabb.x, aabb.y));
}

void IO::DownloadImage(GLViewWidget * gl, IO::Image & image,  uint32_t texture)
{
	GL_ASSERT;
	(void)gl;

	if(texture == 0)
		return;

	int width, height;
	_gl glBindTexture(GL_TEXTURE_2D, texture);
	_gl glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH , &width);
	_gl glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	image.size = glm::ivec2(width, height);
	image.image.reset(new uint8_t[width*height*4]);

	_gl glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, &image.image[0]);
	GL_ASSERT;
}

void IO::DownloadImage(GLViewWidget * gl, basisu::image & image,  uint32_t texture)
{
	GL_ASSERT;
	(void)gl;

	if(texture == 0)
		return;

	int width, height;
	_gl glBindTexture(GL_TEXTURE_2D, texture);
	_gl glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH , &width);
	_gl glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	image.resize(width, height);

	_gl glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, image.get_ptr());
	GL_ASSERT;
}
