#ifndef SPRITESHADERBASE_H
#define SPRITESHADERBASE_H
#include "glprogram.h"
#include <glm/mat4x4.hpp>

class Material;
class GLViewWidget;
class Tex;

#define SHADER(k) "#version 150\n" #k

class SpriteShaderBase : public glProgram
{
public:
	virtual ~SpriteShaderBase() = default;

	static const char * GenericVert();

	virtual void bind(GLViewWidget* gl, Material * material) = 0;

	static void bindTextures(GLViewWidget* gl, Material * material);
	void bindTexture(GLViewWidget* gl, Tex);

	void bindLayer(GLViewWidget* gl, int);
	void bindMatrix(GLViewWidget* gl, glm::mat4x4 const&);

	void GenericLink(GLViewWidget * gl);

protected:
	int32_t u_layer;
	int32_t u_object;
	int32_t u_texCoords;
	int32_t u_boundingBoxes;
};

#endif // SPRITESHADERBASE_H
