#ifndef BLITSHADER_H
#define BLITSHADER_H
#include "spriteshaderbase.h"
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <atomic>

class UnlitShader : public SpriteShaderBase
{
public:
static UnlitShader Shader;
    void bind(GLViewWidget* gl, Material * material) override;

	void bindTexture(GLViewWidget* gl, uint32_t);
	void bindColor(GLViewWidget* gl, glm::vec4);
	void clearColor(GLViewWidget * gl);

private:
    void construct(GLViewWidget* gl) override;
	void destruct(GLViewWidget* gl) override;

	int32_t u_texture;
	int32_t u_color;
	int32_t u_useColor;
};

#endif //BLITSHADER_H
