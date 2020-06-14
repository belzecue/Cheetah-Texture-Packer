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
	attribute(gl, 1, "a_id");
    attribute(gl, 2, "a_uv");
    link(gl);
	uniform(gl, u_layer,   "u_layer");
	uniform(gl, u_object,  "u_object");
	uniform(gl, u_centers,  "u_centers");

	glDefaultVAOs::AddRef();
}

void TransparencyShader::destruct(GLViewWidget* gl)
{
	_gl glAssert();
	glDefaultVAOs::Release(gl);
}

void TransparencyShader::bind(GLViewWidget* gl, Material *)
{
	GL_ASSERT;

    if(bindShader(gl))
	{
        _gl glDisable(GL_DEPTH_TEST);
        _gl glDepthMask(GL_FALSE);
        _gl glDisable(GL_BLEND);
        _gl glDisable(GL_CULL_FACE);
	}

	_gl glUniform1i(u_centers, 10);
	_gl glUniform1f(u_layer , 0);

	_gl glActiveTexture(GL_TEXTURE10);
	_gl glBindTexture(GL_TEXTURE_BUFFER, 0);

    GL_ASSERT;
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
		uniform float u_layer;
		uniform samplerBuffer u_centers;

		in vec2 a_vertex;
		in vec2 a_center;
		in int  a_id;

		void main()
		{
			vec2 pos = a_vertex - texelFetch(u_centers, a_id).rg;
			gl_Position = u_projection * (u_modelview * (u_object * vec4(pos, u_layer, 1.0)));
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

