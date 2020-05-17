#ifndef TRANSPARENCYSHADER_H
#define TRANSPARENCYSHADER_H
#include "glprogram.h"
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <atomic>

class TransparencyShader : public glProgram
{
public:
static TransparencyShader Shader;
    void bind(GLViewWidget* gl);

	void bindCenter(GLViewWidget* gl, bool);
	void bindLayer(GLViewWidget* gl, int);
	void bindMatrix(GLViewWidget* gl, glm::mat4x4 const&);

private:
	std::atomic<int> refCount{0};

    void construct(GLViewWidget* gl);
	void destruct(GLViewWidget* gl);

	int32_t u_spriteSize;
	int32_t u_center;
	int32_t u_layer;
	int32_t u_object;
};

#endif // TRANSPARENCYSHADER_H
