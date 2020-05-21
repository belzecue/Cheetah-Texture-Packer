#ifndef ANIMATION_H
#define ANIMATION_H
#include "Support/countedsizedarray.hpp"
#include "Support/counted_string.h"
#include <atomic>

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

#endif // ANIMATION_H
