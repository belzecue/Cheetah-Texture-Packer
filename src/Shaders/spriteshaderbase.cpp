#include "spriteshaderbase.h"
#include "src/widgets/glviewwidget.h"
#include "src/Sprite/document.h"
#include "defaulttextures.h"
#include <iostream>

void SpriteShaderBase::SpriteLink(GLViewWidget * gl)
{
	static const char tex_template[] = "a_texCoord_";

	attribute(gl, 0, "a_vertex");
	attribute(gl, 1, "a_id");

	{
		char buffer[sizeof(tex_template)];
		strncpy(buffer, tex_template, sizeof(buffer));

		for(int i = 0; i < 8; ++i)
		{
			buffer[sizeof(buffer)-2] = '0' + i;
			attribute(gl, i+2, buffer);
		}
	}

	link(gl);
	uniformBlock(gl, 0, "Matrices");

	uniform(gl, u_object,    "u_object");
	uniform(gl, u_layer,     "u_layer");
	uniform(gl, u_center,    "u_center");
	uniform(gl, u_texCoords, "u_texCoords");
	uniform(gl, u_bufferTexture,"u_bufferTexture");
}

void SpriteShaderBase::SheetLink(GLViewWidget * gl)
{
	attribute(gl, 0, "a_vertex");
	attribute(gl, 1, "a_id");
	attribute(gl, 2, "v_texCoord0");
	attribute(gl, 3, "v_texCoord1");

	link(gl);
	uniformBlock(gl, 0, "Matrices");

	uniform(gl, u_object,    "u_object");
	uniform(gl, u_layer,     "u_layer");
	uniform(gl, u_center,    "u_center");
	uniform(gl, u_texCoords, "u_texCoords");
	uniform(gl, u_bufferTexture,"u_bufferTexture");
}

void SpriteShaderBase::bindTexCoords(GLViewWidget * gl, glm::ivec4 v)
{
	_gl glUniform4i(u_layer, v.x, v.y, v.z, v.w);
}

void SpriteShaderBase::bindTexCoords(GLViewWidget * gl, int v)
{
	_gl glUniform4i(u_layer, v, v, v, v);
}

void SpriteShaderBase::bindLayer(GLViewWidget* gl, int layer)
{
	float l = layer / (float) UCHAR_MAX;
	_gl glUniform1f(u_layer, l);
}

void SpriteShaderBase::bindMatrix(GLViewWidget* gl, glm::mat4x4 const& matrix)
{
	_gl glUniformMatrix4fv(u_object, 1, GL_FALSE, &matrix[0][0]);
}

void SpriteShaderBase::bindCenter(GLViewWidget* gl, bool value)
{
	_gl glUniform1f(u_center, value);
}

void SpriteShaderBase::bindTextures(GLViewWidget* gl, Material * material)
{
	typedef Material::Tex Tex;

	if(!material)
	{
		_gl glActiveTexture(GL_TEXTURE0);
		_gl glBindTexture(GL_TEXTURE_2D, 0);

		_gl glActiveTexture(GL_TEXTURE1);
		_gl glBindTexture(GL_TEXTURE_2D, 0);

		_gl glActiveTexture(GL_TEXTURE2);
		_gl glBindTexture(GL_TEXTURE_2D, 0);

		_gl glActiveTexture(GL_TEXTURE3);
		_gl glBindTexture(GL_TEXTURE_2D, 0);
		return;
	}


	bool use_specular = !material->pbrSpecularGlossiness.is_empty;

	_gl glActiveTexture(GL_TEXTURE0);
	if(use_specular && material->image_slots[(int)Tex::Diffuse])
		_gl glBindTexture(GL_TEXTURE_2D, material->image_slots[(int)Tex::Diffuse]->GetTexture());
	else if(!use_specular && material->image_slots[(int)Tex::BaseColor])
		_gl glBindTexture(GL_TEXTURE_2D, material->image_slots[(int)Tex::BaseColor]->GetTexture());
	else
		_gl glBindTexture(GL_TEXTURE_2D, DefaultTextures::Get().GetWhiteTexture(gl));

	_gl glActiveTexture(GL_TEXTURE1);
	if(material->image_slots[(int)Tex::Normal])
		_gl glBindTexture(GL_TEXTURE_2D, material->image_slots[(int)Tex::Normal]->GetTexture());
	else
		_gl glBindTexture(GL_TEXTURE_2D, DefaultTextures::Get().GetNormalTexture(gl));

	_gl glActiveTexture(GL_TEXTURE2);
	if(use_specular && material->image_slots[(int)Tex::SpecularGlossiness])
		_gl glBindTexture(GL_TEXTURE_2D, material->image_slots[(int)Tex::SpecularGlossiness]->GetTexture());
	else if(!use_specular && material->image_slots[(int)Tex::MetallicRoughness])
		_gl glBindTexture(GL_TEXTURE_2D, material->image_slots[(int)Tex::MetallicRoughness]->GetTexture());
	else
		_gl glBindTexture(GL_TEXTURE_2D, DefaultTextures::Get().GetWhiteTexture(gl));

	_gl glActiveTexture(GL_TEXTURE3);
	if(material->image_slots[(int)Tex::Occlusion])
		_gl glBindTexture(GL_TEXTURE_2D, material->image_slots[(int)Tex::Occlusion]->GetTexture());
	else
		_gl glBindTexture(GL_TEXTURE_2D, DefaultTextures::Get().GetWhiteTexture(gl));
}


const char * SpriteShaderBase::SpriteVert()
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
		uniform ivec4 u_texCoords;
		uniform float u_center;
		uniform isamplerBuffer u_bufferTexture;

		in vec2 a_vertex;
		in int  a_id;
		in vec4 a_texCoord0;
		in vec2 a_texCoord1;
		in vec2 a_texCoord2;
		in vec2 a_texCoord3;
		in vec2 a_texCoord4;
		in vec2 a_texCoord5;
		in vec2 a_texCoord6;
		in vec2 a_texCoord7;

		out vec2 v_position;
		out vec4 v_texCoord0;
		out vec4 v_texCoord1;

		void main()
		{
			vec4 bounds = texelFetch(u_bufferTexture, a_id);
			vec2 center = (bounds.xy + bounds.zw) / 2;
			bounds = bounds; // - vec4(center, center);

			vec2 pos    = mix(bounds.xy, bounds.zw, (a_vertex + 1) / 2); //+ center * u_center;
			gl_Position = u_projection * (u_modelview * (u_object * vec4(pos, u_layer, 1.0)));
			v_position  = gl_Position.xy;

			vec2 texCoord[9] = vec2[9](
				a_texCoord0.xy,
				a_texCoord0.wz,
				a_texCoord1.xy,
				a_texCoord2.xy,
				a_texCoord3.xy,
				a_texCoord4.xy,
				a_texCoord5.xy,
				a_texCoord6.xy,
				a_texCoord7.xy
			);

			v_texCoord0 = vec4(texCoord[u_texCoords[0] % 9], texCoord[u_texCoords[1] % 9]);
			v_texCoord1 = vec4(texCoord[u_texCoords[2] % 9], texCoord[u_texCoords[3] % 9]);
		});

}


const char * SpriteShaderBase::SheetVert()
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
		uniform ivec4 u_texCoords;
		uniform isamplerBuffer u_bufferTexture;

		in vec2 a_vertex;
		in int  a_id;
		in vec2 a_texCoord0;

		out vec2 v_position;
		out vec4 v_texCoord0;
		out vec4 v_texCoord1;

		void main()
		{
			vec2 pos    = a_vertex + texelFetch(u_bufferTexture, a_id).rg;
			gl_Position = u_projection * (u_modelview * (u_object * vec4(pos, u_layer, 1.0)));
			v_position  = gl_Position.xy;

			v_texCoord0 = vec4(a_texCoord0, a_texCoord0);
			v_texCoord1 = vec4(a_texCoord0, a_texCoord0);
		});

}
