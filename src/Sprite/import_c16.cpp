#include "import_c16.h"
#include "widgets/glviewwidget.h"
#include <cstring>
#include <cctype>
#include <fstream>
#include <vector>

extern uint8_t PALETTE_DTA[];

typedef std::unique_ptr<FILE, int(*)(FILE*)> File;


SpriteFile::SpriteFile(uint32_t size, uint32_t heap_size, uint32_t internal_format, uint32_t format, uint32_t type) :
	sizes(size),
	pointers(size),
	heap(heap_size),
	count(size),
	internal_format(internal_format),
	format(format),
	type(type)
{
}

SpriteFile SpriteFile::OpenSprite(const char * path)
{
	char     ext[4]{0, 0, 0, 0};
	uint32_t len = strlen(path)-1;

	if(len < 4) return {};

	for(int i = 0; i < 3; ++i)
		ext[i] = tolower(path[len-4+i]);

	if(strncmp(ext, "spr", 3) == 0)
		return ReadSpr(path);
	else if(strncmp(ext, "s16", 3) == 0)
		return ReadS16(path);
	else
		return ReadC16(path);

	return {};
}

static std::ifstream OpenBinary(const char*path)
{
	std::ifstream fp(path, std::ios::binary);

	if(!fp.is_open())
		throw std::system_error(errno, std::system_category(), path);

	fp.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

	return fp;
}

struct Header
{
	uint32_t offset;
	uint16_t width;
	uint16_t height;

	uint32_t end() const { return offset * width * height; }
};

struct C16Header
{
	uint32_t offset;
	uint16_t width;
	uint16_t height;

	CountedSizedArray<uint32_t> row_offsets;
};

SpriteFile SpriteFile::ReadSpr(const char * path)
{
	auto fp = OpenBinary(path);

	uint16_t no_files{};
	std::vector<Header> header;

	fp.read((char*)&no_files, 2);
	header.resize(no_files);
	fp.read((char*)&header[0], no_files * header.size());

	auto begin = fp.tellg();
	int heap_size = (header.back().end() - begin);

	SpriteFile r(no_files, heap_size*4, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	fp.read((char*)&r.heap[0], heap_size);

	for(uint16_t i = 0; i < no_files; ++i)	r.sizes   [i] = glm::u16vec2(header[i].width, header[i].height);
	for(uint16_t i = 0; i < no_files; ++i)	r.pointers[i] = &r.heap[(header[i].offset - begin)*4];

	for(--heap_size; heap_size >= 0; --heap_size)
	{
		glm::u8vec3 src   = *(glm::u8vec3*)&PALETTE_DTA[r.heap[heap_size]*3];
		glm::u8vec4 & dst = *(glm::u8vec4*)&r.heap[heap_size*4];

		if(src.x == 0 && src.y == 0 && src.z == 0)
			dst = glm::u8vec4(0, 0, 0, 0);
		else
			dst = glm::u8vec4(src*(uint8_t)3, 255);
	}

	return r;
}

static glm::u8vec3 From565(uint16_t r)
{
	return {
		(r & 0xF800) >> 8,
		(r & 0x07E)  >> 3,
		(r & 0x001F) << 3
	};
}

static glm::u8vec3 From555(uint16_t r)
{
	return {
		(r & 0x7C00) >> 7,
		(r & 0x03E)  >> 2,
		(r & 0x001F) << 3
	};
}

SpriteFile SpriteFile::ReadS16(const char * path)
{
	auto fp = OpenBinary(path);

	uint32_t type{};
	uint16_t no_files{};
	std::vector<Header> header;

	fp.read((char*)&type, 4);
	fp.read((char*)&no_files, 2);
	header.resize(no_files);
	fp.read((char*)&header[0], no_files * header.size());

	auto begin = fp.tellg();
	int heap_size = (header.back().end() - begin);

	SpriteFile r(no_files, heap_size*2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	fp.read((char*)&r.heap[0], heap_size);

	for(uint16_t i = 0; i < no_files; ++i)	r.sizes   [i] = glm::u16vec2(header[i].width, header[i].height);
	for(uint16_t i = 0; i < no_files; ++i)	r.pointers[i] = &r.heap[(header[i].offset - begin)*4];

	auto FromUint16 = type? &From565 : &From555;

	for(--heap_size; heap_size >= 0; heap_size -= 2)
	{
		glm::u8vec3  src  = FromUint16(*(uint16_t*)&r.heap[heap_size]);
		glm::u8vec4 & dst = *(glm::u8vec4*)&r.heap[heap_size*2];

		if(src.x == 0 && src.y == 0 && src.z == 0)
			dst = glm::u8vec4(0, 0, 0, 0);
		else
			dst = glm::u8vec4(src, 255);
	}

	return r;
}

SpriteFile SpriteFile::ReadC16(const char * path)
{
	auto fp = OpenBinary(path);

	uint32_t type{};
	uint16_t no_files{};
	std::vector<C16Header> header;

	fp.read((char*)&type, 4);
	fp.read((char*)&no_files, 2);
	header.resize(no_files);

	for(uint32_t i = 0; i < no_files; ++i)
	{
		fp.read((char*)&header[i], sizeof(header[0]));

		if(header[i].height > 1)
		{
			header[i].row_offsets = CountedSizedArray<uint32_t>(header[i].height-1);
			fp.read((char*)&header[i].row_offsets[0], 4 * header[i].height-1);
		}
	}

	fp.read((char*)&header[0], no_files * header.size());

	auto begin = fp.tellg();
	fp.seekg(0, std::ios_base::end);

	std::unique_ptr<uint8_t[]> file_data(new uint8_t[fp.tellg()]);
	fp.seekg(0, std::ios_base::beg);

	fp.read(&file_data[0]);



	SpriteFile r(no_files, heap_size*2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	fp.read((char*)&r.heap[0], heap_size);

	for(uint16_t i = 0; i < no_files; ++i)	r.sizes   [i] = glm::u16vec2(header[i].width, header[i].height);
	for(uint16_t i = 0; i < no_files; ++i)	r.pointers[i] = &r.heap[(header[i].offset - begin)*4];

	auto FromUint16 = type? &From565 : &From555;

	for(--heap_size; heap_size >= 0; heap_size -= 2)
	{
		glm::u8vec3  src  = FromUint16(*(uint16_t*)&r.heap[heap_size]);
		glm::u8vec4 & dst = *(glm::u8vec4*)&r.heap[heap_size*2];

		if(src.x == 0 && src.y == 0 && src.z == 0)
			dst = glm::u8vec4(0, 0, 0, 0);
		else
			dst = glm::u8vec4(src, 255);
	}

	return r;
}

uint8_t PALETTE_DTA[] = {
0x00,0x00,0x00, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F,
0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x04,0x02,0x02,
0x05,0x06,0x0A, 0x06,0x0A,0x04, 0x06,0x09,0x0C, 0x0B,0x04,0x02, 0x0A,0x06,0x09, 0x0D,0x0A,0x04,
0x0C,0x0B,0x0C, 0x06,0x07,0x11, 0x05,0x0D,0x15, 0x06,0x0F,0x18, 0x09,0x07,0x11, 0x0B,0x0D,0x12,
0x0B,0x0E,0x1A, 0x07,0x10,0x07, 0x07,0x10,0x0A, 0x0D,0x12,0x06, 0x0D,0x12,0x0B, 0x0F,0x18,0x06,
0x0F,0x18,0x0A, 0x06,0x10,0x17, 0x07,0x10,0x19, 0x0D,0x11,0x14, 0x0B,0x13,0x1A, 0x0E,0x18,0x13,
0x0F,0x18,0x1C, 0x12,0x06,0x02, 0x12,0x07,0x09, 0x14,0x0B,0x04, 0x12,0x0D,0x0B, 0x1A,0x06,0x03,
0x1B,0x07,0x09, 0x1B,0x0C,0x04, 0x1A,0x0D,0x09, 0x12,0x0E,0x12, 0x12,0x0E,0x1A, 0x1A,0x0D,0x12,
0x1D,0x0D,0x1A, 0x14,0x12,0x05, 0x14,0x12,0x0C, 0x14,0x19,0x06, 0x13,0x1A,0x0B, 0x1C,0x12,0x05,
0x1B,0x13,0x0B, 0x1C,0x19,0x05, 0x1D,0x19,0x0C, 0x13,0x13,0x13, 0x13,0x15,0x1B, 0x15,0x19,0x14,
0x15,0x19,0x1C, 0x1A,0x15,0x13, 0x1A,0x16,0x1A, 0x1C,0x1A,0x14, 0x1B,0x1B,0x1B, 0x0C,0x0F,0x21,
0x0E,0x17,0x24, 0x10,0x0F,0x21, 0x13,0x16,0x23, 0x12,0x16,0x2C, 0x14,0x1A,0x23, 0x12,0x1B,0x2B,
0x19,0x16,0x22, 0x19,0x17,0x2B, 0x1B,0x1C,0x23, 0x1B,0x1D,0x2A, 0x13,0x17,0x31, 0x14,0x1D,0x32,
0x17,0x1C,0x3B, 0x1A,0x1E,0x33, 0x19,0x1E,0x3D, 0x1A,0x23,0x0D, 0x17,0x21,0x13, 0x17,0x20,0x1A,
0x1B,0x23,0x13, 0x1D,0x22,0x1C, 0x1E,0x29,0x13, 0x1E,0x29,0x1A, 0x16,0x20,0x23, 0x17,0x20,0x2E,
0x1C,0x21,0x25, 0x1D,0x22,0x2B, 0x1F,0x29,0x23, 0x1E,0x29,0x2C, 0x16,0x21,0x33, 0x16,0x24,0x39,
0x16,0x29,0x3C, 0x1C,0x22,0x33, 0x1D,0x22,0x3F, 0x1E,0x28,0x36, 0x1C,0x29,0x3B, 0x23,0x06,0x04,
0x24,0x07,0x09, 0x22,0x0D,0x04, 0x23,0x0D,0x0A, 0x2B,0x06,0x04, 0x2B,0x07,0x08, 0x2A,0x0C,0x04,
0x2B,0x0C,0x0A, 0x26,0x0D,0x12, 0x23,0x13,0x05, 0x23,0x14,0x0A, 0x24,0x1A,0x05, 0x24,0x1A,0x0C,
0x2B,0x14,0x05, 0x2A,0x15,0x0A, 0x2C,0x1A,0x05, 0x2B,0x1B,0x0B, 0x22,0x15,0x12, 0x22,0x16,0x1B,
0x23,0x1B,0x13, 0x22,0x1D,0x1B, 0x2B,0x14,0x12, 0x2C,0x15,0x19, 0x2A,0x1D,0x12, 0x2B,0x1D,0x1A,
0x34,0x0B,0x07, 0x35,0x0D,0x12, 0x32,0x15,0x05, 0x32,0x15,0x0A, 0x33,0x1A,0x05, 0x33,0x1C,0x0B,
0x3A,0x14,0x05, 0x3A,0x14,0x0B, 0x3A,0x1D,0x05, 0x3A,0x1D,0x0A, 0x33,0x14,0x12, 0x33,0x15,0x19,
0x33,0x1D,0x12, 0x32,0x1D,0x1A, 0x3A,0x14,0x14, 0x3B,0x16,0x18, 0x3C,0x1C,0x12, 0x3B,0x1C,0x1C,
0x24,0x0F,0x21, 0x23,0x14,0x21, 0x21,0x1E,0x24, 0x21,0x1E,0x2A, 0x2A,0x1E,0x22, 0x29,0x1F,0x29,
0x20,0x1F,0x31, 0x34,0x0C,0x20, 0x36,0x1C,0x22, 0x3B,0x1D,0x33, 0x29,0x22,0x0B, 0x25,0x21,0x14,
0x24,0x22,0x1C, 0x22,0x2B,0x14, 0x23,0x2B,0x1B, 0x2C,0x22,0x14, 0x2B,0x23,0x1B, 0x2D,0x29,0x14,
0x2D,0x2A,0x1C, 0x27,0x31,0x0F, 0x29,0x34,0x17, 0x34,0x22,0x06, 0x34,0x22,0x0C, 0x35,0x2A,0x05,
0x34,0x2A,0x0B, 0x3C,0x23,0x05, 0x3B,0x23,0x0B, 0x3D,0x2B,0x05, 0x3D,0x2B,0x0C, 0x33,0x23,0x13,
0x32,0x25,0x1A, 0x34,0x2A,0x14, 0x34,0x2A,0x1C, 0x3B,0x24,0x12, 0x3B,0x24,0x19, 0x3C,0x2B,0x13,
0x3B,0x2C,0x1B, 0x34,0x31,0x0E, 0x3D,0x33,0x03, 0x3E,0x33,0x0C, 0x3F,0x3C,0x03, 0x3F,0x3B,0x0B,
0x35,0x31,0x14, 0x35,0x31,0x1C, 0x32,0x3D,0x14, 0x33,0x3D,0x1B, 0x3E,0x32,0x13, 0x3D,0x33,0x1B,
0x3E,0x3B,0x13, 0x3F,0x3A,0x1C, 0x23,0x22,0x24, 0x23,0x24,0x2B, 0x24,0x2A,0x24, 0x25,0x2A,0x2D,
0x2A,0x24,0x23, 0x29,0x26,0x2C, 0x2C,0x2A,0x24, 0x2B,0x2A,0x2D, 0x22,0x25,0x33, 0x21,0x26,0x3E,
0x25,0x29,0x34, 0x24,0x2A,0x3F, 0x28,0x27,0x31, 0x2B,0x2B,0x33, 0x29,0x2E,0x3D, 0x2A,0x32,0x2A,
0x26,0x31,0x31, 0x2C,0x30,0x34, 0x2A,0x31,0x3F, 0x2C,0x3A,0x31, 0x2E,0x39,0x3A, 0x33,0x24,0x24,
0x32,0x26,0x29, 0x33,0x2C,0x23, 0x32,0x2C,0x2C, 0x3B,0x24,0x23, 0x3B,0x24,0x29, 0x3A,0x2D,0x22,
0x3A,0x2D,0x2A, 0x31,0x2E,0x32, 0x31,0x2F,0x38, 0x3D,0x2B,0x33, 0x35,0x32,0x24, 0x34,0x32,0x2C,
0x33,0x3C,0x22, 0x33,0x39,0x2C, 0x3C,0x33,0x24, 0x3B,0x34,0x2B, 0x3E,0x3A,0x24, 0x3E,0x3B,0x2C,
0x35,0x32,0x33, 0x32,0x32,0x3A, 0x35,0x39,0x33, 0x36,0x3A,0x39, 0x39,0x35,0x34, 0x38,0x34,0x38,
0x3C,0x3A,0x34, 0x3D,0x3D,0x3B, 0x3F,0x3F,0x3F, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F,
0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F };
