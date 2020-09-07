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

	int32_t PackBufferView(uint8_t * ptr, uint32_t size, bool take_memory);
	void PackDocument(Sprites::Document & doc);
	int32_t PackIndices(uint16_t * ptr, uint32_t size, bool take_memory);
	void InterlaceBuffers(std::unique_ptr<uint32_t[]> array, uint32_t size);

	template<int S, typename T, glm::qualifier Q>
	int32_t PackAccessor(glm::vec<S, T, Q> * array, uint32_t length, bool normalize, bool take_memory)
	{
		static_assert(ComponentTypeId<T>::ComponentType != 0, "unidentified component type");

		return PackAccessor(
			(void*)array, length,
			(ComponentType)ComponentTypeId<T>::ComponentType,
			(Type)S,
			normalize, take_memory);
	}

	int32_t PackAccessor(void * array, uint32_t length, ComponentType, Type, bool normalize, bool take_memory);

	struct BufferPtr
	{
		BufferPtr() = default;
		BufferPtr(BufferPtr const&) = delete;
		BufferPtr(BufferPtr &&);
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
		bool		  normalize{};
		ComponentType componentType{ComponentType::None};
		uint32_t      byteOffset{};
		uint32_t	  count{};
	};

	struct BufferViewMemo
	{
		std::unique_ptr<uint32_t[]> accessors;
		uint16_t      length{};
	};

	std::unordered_map<void*, int32_t> mapping;

private:
	void GetMinMax(AccessorMemo const& memo, std::vector<float> & min, std::vector<float> & max);

	std::vector<BufferViewMemo> m_memo;
	std::vector<AccessorMemo>   m_accessors;
	std::vector<BufferPtr>		m_buffers;
};


#endif // PACKACCESSOR_H
