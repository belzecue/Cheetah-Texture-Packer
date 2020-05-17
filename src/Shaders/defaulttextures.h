#ifndef DEFAULTTEXTURES_H
#define DEFAULTTEXTURES_H
#include <atomic>

class GLViewWidget;

class DefaultTextures
{
public:
	static DefaultTextures & Get()
	{
		static DefaultTextures defaults;
		return defaults;
	}

	~DefaultTextures();

	inline void AddRef() { ++refCount; }
	inline void Release(GLViewWidget * gl) { if(--refCount == 0) destroyTextures(gl); }

	uint32_t GetWhiteTexture(GLViewWidget * gl);
	uint32_t GetNormalTexture(GLViewWidget * gl);

private:
	enum
	{
		WhiteTexture,
		NormalTexture,
		TotalTextures
	};

	std::atomic<int> refCount;
	uint32_t textures[TotalTextures];

	DefaultTextures();

	void createTextures(GLViewWidget * gl);
	void destroyTextures(GLViewWidget * gl);
};

#endif // DEFAULTTEXTURES_H
