#include "blitshader.h"
#include "defaultvaos.h"
#include <glm/vec4.hpp>
#include "src/widgets/glviewwidget.h"

#define SHADER(k) "#version 150\n" #k
static const char * kVert();
static const char * kFrag();

BlitShader BlitShader::Shader;

void BlitShader::construct(GLViewWidget* gl)
{
    compile(gl, kVert(), GL_VERTEX_SHADER);
    compile(gl, kFrag(), GL_FRAGMENT_SHADER);
    attribute(gl, 0, "a_vertex");
	attribute(gl, 1, "a_center");
    attribute(gl, 2, "a_uv");
    link(gl);

	uniform(gl, u_texture, "u_texture");
	uniform(gl, u_layer,   "u_layer");
	uniform(gl, u_center,  "u_center");
	uniform(gl, u_object,  "u_object");
	uniform(gl, u_color,   "u_color");
	uniform(gl, u_useColor,"u_useColor");


	glDefaultVAOs::AddRef();
}

void BlitShader::destruct(GLViewWidget* gl)
{
	_gl glAssert();
	glDefaultVAOs::Release(gl);
}

void BlitShader::bind(GLViewWidget* gl, Material * )
{
    if(bindShader(gl))
	{
		 _gl glEnable(GL_DEPTH_TEST);
		 _gl glDepthMask(GL_TRUE);

		_gl glEnable(GL_BLEND);
		_gl glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	_gl glActiveTexture(GL_TEXTURE0);
	_gl glUniform1i(u_texture, 1);
	_gl glUniform1f(u_center, false);
	_gl glUniform1f(u_layer , 0);
	_gl glUniform1f(u_useColor , 0);
	_gl glUniform4f(u_color, 0, 0, 0, 0);

    _gl glAssert();
}

void BlitShader::bindTexture(GLViewWidget* gl, uint32_t texture)
{
	_gl glBindTexture(GL_TEXTURE_2D, texture);
}

void BlitShader::bindColor(GLViewWidget* gl, glm::vec4 color)
{
	_gl glUniform4fv(u_color, 1, &color[0]);
	_gl glUniform1f(u_useColor, 1.f);
}

void BlitShader::clearColor(GLViewWidget* gl)
{
	_gl glUniform1f(u_useColor, 0.f);
}



static const char * kVert()
{
	return SHADER(
		layout(std140) uniform Matrices
		{
			mat4  u_projection;
			mat4  u_modelview;
			ivec4 u_screenSize;
			 vec4 u_cursorColor;
			float u_time;
		};

		uniform mat4  u_object;
		uniform float u_center;
		uniform float u_layer;

		in vec2 a_vertex;
		in vec2 a_center;
		in vec2 a_uv;

		out vec2 v_uv;

		void main()
		{
			vec2 pos = a_vertex - mix(vec2(0, 0), a_center, u_center);
			gl_Position = u_projection * (u_modelview * (u_object * vec4(pos, 0, 1.0)));
			v_uv        = a_uv;
		});
}

static const char * kFrag()
{
	return SHADER(
		uniform sampler2D u_texture;
		uniform vec4      u_color;
		uniform float     u_useColor;

		in vec2 v_uv;

		out vec4 frag_color;

		void main()
		{
			frag_color = mix(texture(u_texture, v_uv), u_color, u_useColor);
		});
}

