#ifndef IMPORT_C16_H
#define IMPORT_C16_H
#include <memory>
#include <cstdint>
#include "Support/countedsizedarray.hpp"
#include <glm/gtc/type_precision.hpp>

class SpriteFile;
typedef std::unique_ptr<SpriteFile> SpritePtr;

class SpriteFile
{
public:
	static SpriteFile OpenSprite(const char * path);
	static SpriteFile ReadSpr(const char * path);
	static SpriteFile ReadC16(const char * path);
	static SpriteFile ReadS16(const char * path);

	SpriteFile() = default;
	SpriteFile(SpriteFile const& file) :
		sizes(file.sizes),
		pointers(file.pointers),
		heap(file.heap),
		count(file.count),
		internal_format(file.internal_format),
		format(file.format),
		type(file.type)
	{
	}
	SpriteFile(uint32_t size, uint32_t heap_size, uint32_t internal_format, uint32_t format, uint32_t type);

	auto size() const { return count; }
	bool empty() const { return !count; }

	CountedSizedArray<glm::u16vec2> sizes;
	CountedSizedArray<void*>        pointers;
	CountedSizedArray<uint8_t>      heap;

	uint16_t                        count{};
	uint32_t                        internal_format{};
	uint32_t                        format{};
	uint32_t                        type{};
};


#endif // IMPORT_C16_H
