#include "transparencyshader.h"
#include "defaultvaos.h"
#include "src/widgets/glviewwidget.h"

#define SHADER(k) "#version 150\n" #k
static const char * kVert();
static const char * kFrag();

TransparencyShader TransparencyShader::Shader;

void TransparencyShader::construct(GLViewWidget* gl)
{
    compile(gl, kVert(), GL_VERTEX_SHADER);
    compile(gl, kFrag(), GL_FRAGMENT_SHADER);
    attribute(gl, 0, "a_vertex");
	attribute(gl, 1, "a_center");
    attribute(gl, 2, "a_uv");
    link(gl);
	uniform(gl, u_layer,   "u_layer");
	uniform(gl, u_center,  "u_center");
	uniform(gl, u_object,  "u_object");

	glDefaultVAOs::AddRef();
}

void TransparencyShader::destruct(GLViewWidget* gl)
{
	_gl glAssert();
	glDefaultVAOs::Release(gl);
}

void TransparencyShader::bind(GLViewWidget* gl)
{
	_gl glAssert();

    if(bindShader(gl))
	{
        _gl glDisable(GL_DEPTH_TEST);
        _gl glDepthMask(GL_FALSE);
        _gl glDisable(GL_BLEND);
        _gl glDisable(GL_CULL_FACE);
	}

	_gl glUniform1f(u_center, false);
	_gl glUniform1f(u_layer , 0);

    _gl glAssert();
}

void TransparencyShader::bindCenter(GLViewWidget* gl, bool center)
{
	_gl glUniform1f(u_center, center);
}

void TransparencyShader::bindLayer(GLViewWidget* gl, int layer)
{
	_gl glUniform1f(u_layer, layer / (float) SHRT_MAX);
}

void TransparencyShader::bindMatrix(GLViewWidget* gl, glm::mat4x4 const& matrix)
{
	_gl glUniformMatrix4fv(u_object, 1, GL_FALSE, &matrix[0][0]);
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
			gl_Position = u_projection * (u_modelview * (u_object * vec4(pos, u_layer, 1.0)));
			v_uv        = a_uv;
		});
}

static const char * kFrag()
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

		in vec2 v_uv;
		in vec4 gl_FragCoord;

		out vec4 frag_color;

		void main()
		{
			ivec2 coords = ivec2(gl_FragCoord);
			int tile = (int(coords.x / 10) & 0x01) ^ (int(coords.y / 10) & 0x01);
			float color = (125 + 100 * tile) / 256.f;
			frag_color = vec4(vec3(color), 1.0);
		});
}

