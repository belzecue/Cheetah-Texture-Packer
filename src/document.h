#ifndef DOCUMENT_H
#define DOCUMENT_H
#include <fx/gltf.h>
#include <fx/extensions/khr_materials.h>
#include "commandinterface.hpp"
#include "packersettings.h"
#include "Support/countedsizedarray.hpp"
#include "Support/counted_string.h"
#include "Support/counted_ptr.hpp"
#include "image.h"
#include <chrono>
#include <algorithm>
#include <memory>
#include <atomic>

class CommandInterface;
class MainWindow;
class GLViewWidget;
class Image;
struct Document;

struct Material : fx::gltf::Material
{
	enum class Tex
	{
		None = -1,
		Normal,
		Emission,
		Occlusion,
		BaseColor,
		MetallicRoughness,
		Diffuse,
		SpecularGlossiness,
		Total
	};

	KHR::materials::pbrSpecularGlossiness pbrSpecularGlossiness;
	KHR::materials::unlit                 unlit;

	counted_ptr<Image>                    image_slots[(int)Tex::Total];
	int8_t                                tex_coords[(int)Tex::Total];
	bool                                  use_specular{false};

	inline int & TexCoord(Tex tex)
	{
		static int x{};

		switch(tex)
		{
		case Tex::Normal:				return normalTexture.texCoord;
		case Tex::Emission:				return emissiveTexture.texCoord;
		case Tex::Occlusion:			return occlusionTexture.texCoord;
		case Tex::BaseColor:			return pbrMetallicRoughness.baseColorTexture.texCoord;
		case Tex::MetallicRoughness:	return pbrMetallicRoughness.metallicRoughnessTexture.texCoord;
		case Tex::Diffuse:				return pbrSpecularGlossiness.diffuseTexture.texCoord;
		case Tex::SpecularGlossiness:   return pbrSpecularGlossiness.specularGlossinessTexture.texCoord;
		default:
			throw std::logic_error("Unknown material map value.");
		}

		return x;
	}
};

struct Animation
{
	Animation() = default;
	Animation(Animation const& it) { *this = it; }

	counted_string              name;
	CountedSizedArray<uint16_t> frames;
	float                       fps{20};

	Animation& operator=(Animation const& it)
	{
		name   = it.name;
		frames = it.frames;
		fps    = it.fps;

		return *this;
	}

	void AddRef() const { ++m_refCount; }
	void Release() { if(--m_refCount == 0) delete this; }

private:
	mutable std::atomic<int> m_refCount{1};
};

struct Object;

struct Document
{
	Document(GLViewWidget*gl) : imageManager(gl) {}

	Image::ImageManager   imageManager;
	PackerSettings        settings;

	std::vector<counted_ptr<Object>>   objects;
	MainWindow *          window{};

	void AddRef() const { ++m_refCount; }
	void Release() { if(--m_refCount == 0) delete this; }

	template<typename T, typename...Args>
	void addCommand(Args&&... args)
	{
		std::unique_ptr<T> r;
		try
		{
			r = std::make_unique<T>(this, std::move(args)...);
		}
		catch(std::exception& e)
		{
			OnError(e.what());
		}

		addCommand(std::move(r));
		return;
	}

	void addCommand(std::unique_ptr<CommandInterface> );

	void editUndo();
	void editRedo();

	bool CanUndo() const { return commandId > 0; }
	bool CanRedo() const { return commandId < commandList.size(); }

	void RenderObjectSheet(GLViewWidget *, Object*, int frame = -1);
	void RenderSpriteSheet(GLViewWidget *, Image *, int frame = -1);
	void RenderAnimation(GLViewWidget *,  Object*, int);
	void RenderAttachments(GLViewWidget *, Object*, int);
	void RenderPackedTextures(GLViewWidget *, int) {}

	void ClearAnimation() { animation = nullptr; }
	void OnError(std::string const& what);

private:
typedef std::chrono::steady_clock::time_point time_point;
	mutable std::atomic<int> m_refCount{1};

	std::vector<std::unique_ptr<CommandInterface> > commandList;
	uint32_t                                        commandId{0};

	void     * animation{};
	time_point animation_start{};

//image management
};

#endif // DOCUMENT_H
