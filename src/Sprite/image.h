#ifndef IMAGE_H
#define IMAGE_H
#include "Support/counted_string.h"
#include "Support/countedsizedarray.hpp"
#include <glm/gtc/type_precision.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include <mutex>
#include <map>
#include <memory>

class GLViewWidget;
#define HAVE_CHROMA_KEY 0

class Image
{
public:
	class ImageManager
	{
	public:
		ImageManager(GLViewWidget *gl) : gl(gl) {}
		~ImageManager() = default;

		GLViewWidget * GetGL() const { return gl; }

	private:
	friend class Image;
		std::map<counted_string, Image*> loadedImages;
		GLViewWidget                  *  gl;
	};

	enum
	{
		SpriteCoords,
		CropBoxes,
		Centers,
		TexCoord,

		VBOc = 4,

		SpritePadding = 0,
		CroppedSprites,

		VAOc = 2,
	};

	static counted_ptr<Image> Factory(ImageManager * manager, std::string const& path);

	void LoadFromFile();
	void Clear();

	CountedSizedArray<glm::i16vec4> m_sprites;
	CountedSizedArray<glm::i16vec4> m_cropped;
	CountedSizedArray<glm::u16vec4> m_normalized;
	CountedSizedArray<glm::u16vec4> m_normalizedPositions;

	std::unique_ptr<uint8_t[]> LoadFileAsArray(uint32_t & size) const;

	bool isLoaded() const { return m_isLoaded; }
	bool hasAlpha() const { return m_hasAlpha; }

	uint32_t GetTexture() const { return m_texture; };
	glm::u16vec2 GetSize() const { return m_size; }

	std::string getFilename() const;
	std::string getDirectory() const;
	std::string getMimeType() const;

#if HAVE_CHROMA_KEY
	void UpdateChromaKey(uint32_t color_mask, uint32_t color_key);
#endif

protected:
friend class counted_ptr<Image>;
	void AddRef() const { ++m_refCount; }
	void Release() { if(--m_refCount == 0) { Clear(); delete this; } }

private:
	Image(ImageManager * manager, std::string const& path);
	~Image();

	static std::mutex                       g_mutex;

	ImageManager                  * m_manager{nullptr};
	std::string		                m_path;

	glm::i16vec2                    m_size{0, 0};
	uint8_t                         m_channels{0};

	bool                            m_hasAlpha{0};
	bool                            m_isLoaded{0};
	bool                            m_ownsTexture{0};

	uint32_t                        m_texture{0};
	mutable std::atomic<int>        m_refCount{1};

#if HAVE_CHROMA_KEY
	struct CropHistory
	{
		CountedSizedArray<glm::i16vec4> cropped;
		CountedSizedArray<glm::u16vec4> normalized;

		uint32_t color_mask;
		uint32_t color_key;
	};

	std::vector<CropHistory>       m_cropHistory;
#endif
};

#endif // IMAGE_H
