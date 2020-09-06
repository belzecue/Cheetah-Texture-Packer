#ifndef PACKACCESSOR_H
#define PACKACCESSOR_H
#include <fx/gltf.h>
#include "Support/countedsizedarray.hpp"
#include <componenttypeinfo.hpp>
#include <glm/vec4.hpp>
#include <vector>

namespace Sprites
{
struct Document;
};

struct PackMemo
{
typedef fx::gltf::Accessor::ComponentType ComponentType;
typedef fx::gltf::Accessor::Type		  Type;
typedef fx::gltf::BufferView::TargetType  TargetType;


	void PackDocument(Sprites::Document & doc);

	struct BufferPtr
	{
		~BufferPtr() { if(ownsData) free(data); }

		bool operator==(BufferPtr const&) const;
		bool operator!=(BufferPtr const& it) const { return !(*this == it); }

		uint8_t * data{};
		uint32_t size{};
		uint16_t stride{};
		int32_t  bufferViewId{-1};
		bool	 interleaved{};
		bool	 ownsData{};
		TargetType    targetType{};
	};

	struct AccessorMemo
	{
		bool operator==(AccessorMemo const&) const;
		bool operator!=(AccessorMemo const& it) const { return !(*this == it); }

		int32_t		  bufferId{};
		Type		  type{Type::None};
		ComponentType componentType{ComponentType::None};
		uint32_t      byteOffset{};
		uint32_t	  count{};
	};

	struct BufferViewMemo
	{
		std::unique_ptr<uint32_t[]> accessors;
		uint16_t      length{};
		TargetType    targetType{};
		uint32_t	  stride{};
	};

	std::unordered_map<void*, int32_t> mapping;

private:
	void GetMinMax(AccessorMemo const& memo, std::vector<float> & min, std::vector<float> & max);

	std::vector<BufferViewMemo> m_memo;
	std::vector<AccessorMemo>   m_accessors;
	std::vector<BufferPtr>		m_buffers;
};


#endif // PACKACCESSOR_H
