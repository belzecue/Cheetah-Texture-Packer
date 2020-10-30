#include "animation.h"
#include "spritejson.h"

Sprites::Animation Animation::PackDocument() const
{
	Sprites::Animation r;

	r.name = name.toStdString();
	r.frames.insert(r.frames.end(), frames.begin(), frames.end());
	r.fps = fps;
	r.loop_start = loop_start;
	r.loop_end   = loop_end;

	return r;
}
