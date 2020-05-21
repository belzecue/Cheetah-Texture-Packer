#include "gltfmetallicroughness.h"
#include "defaulttextures.h"
#include "Sprite/material.h"
#include <glm/vec4.hpp>
#include "src/widgets/glviewwidget.h"

#define SHADER(k) "#version 150\n" #k
static const char * kVert();
static const char * kFrag();

gltfMetallicRoughness gltfMetallicRoughness::Shader;

void gltfMetallicRoughness::construct(GLViewWidget* gl)
{
    compile(gl, kVert(), GL_VERTEX_SHADER);
    tryLoad(gl, kFrag(), GL_FRAGMENT_SHADER);
    attribute(gl, 0, "a_vertex");
	attribute(gl, 1, "a_center");
    attribute(gl, 3, "a_texCoord0");
	attribute(gl, 4, "a_texCoord1");
    link(gl);

	uniform(gl, u_object,          "u_object");
	uniform(gl, u_center,          "u_center");
	uniform(gl, u_layer,           "u_layer");

	uniform(gl, u_normal,          "u_normal");
	uniform(gl, u_occlusion,       "u_occlusion");
	uniform(gl, u_diffuse,         "u_diffuse");
	uniform(gl, u_pbr,             "u_pbr");

	uniform(gl, u_alphaMode,       "u_alphaMode");
	uniform(gl, u_texCoords,       "u_texCoords");
	uniform(gl, u_NOMR,            "u_NOMR");
	uniform(gl, u_specularFactor,  "u_specularFactor");
	uniform(gl, u_baseColorFactor, "u_baseColorFactor");
	uniform(gl, u_emissionFactor,  "u_emissionFactor");

	DefaultTextures::Get().AddRef();
}

void gltfMetallicRoughness::destruct(GLViewWidget* gl)
{
	_gl glAssert();
	DefaultTextures::Get().Release(gl);
}

void gltfMetallicRoughness::bind(GLViewWidget* gl, Material * material)
{
typedef fx::gltf::Material::AlphaMode AlphaMode;

    if(bindShader(gl))
	{
		 _gl glEnable(GL_DEPTH_TEST);
		 _gl glDepthMask(GL_TRUE);

		_gl glEnable(GL_BLEND);
		_gl glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	_gl glUniform1f(u_center, false);
	_gl glUniform1f(u_layer , 0);

	_gl glUniform1i( u_diffuse, 0);
	_gl glUniform1i( u_normal, 1);
	_gl glUniform1i( u_pbr, 2);
	_gl glUniform1i( u_occlusion, 3);

	bindTextures(gl, material);

	bool use_specular = !material->pbrSpecularGlossiness.is_empty;

	if(material->alphaMode == AlphaMode::Opaque)
		_gl glUniform1f(u_alphaMode, 1.f);
	else if(material->alphaMode == AlphaMode::Mask)
		_gl glUniform1f(u_alphaMode, material->alphaCutoff);
	else
		_gl glUniform1f(u_alphaMode, -1.f);

	_gl glUniform4i(u_texCoords,
		material->normalTexture.texCoord,
		material->occlusionTexture.texCoord,
		use_specular?
			material->pbrSpecularGlossiness.diffuseTexture.texCoord :
			material->pbrMetallicRoughness.baseColorTexture.texCoord,
		use_specular?
			material->pbrSpecularGlossiness.specularGlossinessTexture.texCoord :
			material->pbrMetallicRoughness.metallicRoughnessTexture.texCoord);

	_gl glUniform4f(u_NOMR,
		material->normalTexture.scale,
		material->occlusionTexture.strength,
		material->pbrMetallicRoughness.metallicFactor,
		use_specular?
			material->pbrSpecularGlossiness.glossinessFactor :
			material->pbrMetallicRoughness.roughnessFactor);

	_gl glUniform4fv(u_baseColorFactor, 1, use_specular?
		&material->pbrSpecularGlossiness.diffuseFactor[0] :
		&material->pbrMetallicRoughness.baseColorFactor[0]);

	_gl glUniform4f(u_specularFactor,
		material->pbrSpecularGlossiness.specularFactor[0],
		material->pbrSpecularGlossiness.specularFactor[1],
		material->pbrSpecularGlossiness.specularFactor[2],
		use_specular);

	_gl glUniform3fv(u_emissionFactor, 1, &material->emissiveFactor[0]);

    _gl glAssert();
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
		in vec2 a_texCoord0;
		in vec2 a_texCoord1;
		in vec2 a_sprCoord;

		out vec3 v_position;
		out vec4 v_texCoord;
		out vec2 v_sprCoord;

		void main()
		{
			vec2 pos    = a_vertex - mix(vec2(0, 0), a_center, u_center);
			gl_Position = u_projection * (u_modelview * (u_object * vec4(pos, 0, 1.0)));
			v_position  = gl_Position.xyz;

			v_texCoord = vec4(a_texCoord0, a_texCoord1);
			v_sprCoord = a_sprCoord;
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
	vec4  u_cursorPos;
	float u_time;
};

uniform sampler2D u_normal;
uniform sampler2D u_occlusion;
uniform sampler2D u_diffuse;
uniform sampler2D u_pbr;

uniform float u_alphaMode;
uniform ivec4 u_texCoords;
uniform vec4  u_NOMR;
uniform vec4  u_specularFactor;
uniform vec4  u_baseColorFactor;
uniform vec3  u_emissionFactor;

in vec3 v_position;
in vec4 v_texCoord;
in vec2 v_sprCoord;

out vec4 frag_color;


// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct PBRInfo
{
	float NdotL;                  // cos angle between normal and light direction
	float NdotV;                  // cos angle between normal and view direction
	float NdotH;                  // cos angle between normal and half vector
	float LdotH;                  // cos angle between light direction and half vector
	float VdotH;                  // cos angle between view direction and half vector
	//    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
	//    float metalness;              // metallic value at the surface

	vec3 reflectance0;            // full reflectance color (normal incidence angle)
	vec3 reflectance90;           // reflectance color at grazing angle
	float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
	vec3 diffuseColor;            // color contribution from diffuse lighting
	vec3 specularColor;           // color contribution from specular lighting
};

const float M_PI = 3.141592653589793;
const float c_MinRoughness = 0.04;

vec4 SRGBtoLINEAR(vec4 srgbIn)
{
	vec3 bLess = step(vec3(0.04045),srgbIn.xyz);
	vec3 linOut = mix( srgbIn.xyz/vec3(12.92), pow((srgbIn.xyz+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
	return vec4(linOut,srgbIn.w);
}

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 getNormal(vec2 texCoord)
{
	vec3 n = texture(u_normal, texCoord).rgb;
	n = ((2.0 * n - 1.0) * vec3(u_NOMR[0], u_NOMR[0], 1.0));
	return normalize(n);
}

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 diffuse(PBRInfo pbrInputs)
{
	return pbrInputs.diffuseColor / M_PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 specularReflection(PBRInfo pbrInputs)
{
	float VdotH = clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0);
	float VdotH2 = VdotH * VdotH;
	float VdotH5 = VdotH2 * VdotH2 * VdotH;
	return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * VdotH5;
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float geometricOcclusion(PBRInfo pbrInputs)
{
	float NdotL = pbrInputs.NdotL;
	float NdotV = pbrInputs.NdotV;
	float r = pbrInputs.alphaRoughness;

	float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
	float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
	return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float microfacetDistribution(PBRInfo pbrInputs)
{
	float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
	float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
	return roughnessSq / (M_PI * f * f);
}

void main()
{
	vec2 texCoord[3] = vec2[3](
		v_texCoord.xy,
		v_texCoord.wz,
		v_sprCoord
	);

// The albedo may be defined from a base texture or a flat color
	vec4 baseColor = texture(u_diffuse, texCoord[u_texCoords[2]]) * u_baseColorFactor;
	vec4 pbr       = texture(u_pbr,     texCoord[u_texCoords[3]]);
	vec3 diffuseColor = baseColor.rgb;
	vec3 specularColor;
	float alphaRoughness;

	if(u_specularFactor.z == 1.f)
	{
		alphaRoughness = (1 -  pbr.a * u_NOMR[3]);
		alphaRoughness       = alphaRoughness * alphaRoughness;
		specularColor        = pbr.rbg * u_specularFactor.rgb;
	}
	else
	{
		// Metallic and Roughness material properties are packed together
		// In glTF, these factors can be specified by fixed scalar values
		// or from a metallic-roughness map
		float perceptualRoughness = pbr.g * u_NOMR[3];
		float metallic = pbr.b * u_NOMR[2];

		perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);
		metallic = clamp(metallic, 0.0, 1.0);

		// Roughness is authored as perceptual roughness; as is convention,
		// convert to material roughness by squaring the perceptual roughness [2].
		float alphaRoughness = perceptualRoughness * perceptualRoughness;

		vec3 f0 = vec3(0.04);
		diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
		diffuseColor *= 1.0 - metallic;
		specularColor = mix(f0, baseColor.rgb, metallic);
	}

	// Compute reflectance.
	float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

	// For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
	// For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
	float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
	vec3 specularEnvironmentR0 = specularColor.rgb;
	vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

	vec3 screen_space = vec3(gl_FragCoord.xy - u_screenSize.xy * .5,  256 - v_position.z * .004);

	vec3 n = getNormal(texCoord[u_texCoords[0]]);                             // normal at surface point
	vec3 v = normalize(screen_space);                // Vector from surface point to camera

	PBRInfo pbrInputs = PBRInfo(
		0,
		0,
		0,
		0,
		0,
		specularEnvironmentR0,
		specularEnvironmentR90,
		alphaRoughness,
		diffuseColor,
		specularColor
	);

	vec3 specContrib    = vec3(0, 0, 0);
	vec3 diffuseContrib = vec3(0, 0, 0);

	{
		float radius   = 500;
		vec3  distance = u_cursorPos.xyz - v_position;
		distance.z    *= .5;

		float length2 =  dot(distance, distance);
		float length  =  sqrt(length2);

		int is_on      =  int(length2 < radius*radius);
		length2        = min(length2, radius * radius - 1);
	//	color4 = mix(color4, vec4(0,0,0,0), vec4(float(distance.z < 0)));

	// Vector from surface point to light
		vec3 l = distance/length;
		vec3 h = normalize(l+v);                          // Half vector between both l and v
		vec3 reflection = -normalize(reflect(v, n));

		pbrInputs.NdotL = clamp(dot(n, l), 0.001, 1.0);
		pbrInputs.NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
		pbrInputs.NdotH = clamp(dot(n, h), 0.0, 1.0);
		pbrInputs.LdotH = clamp(dot(l, h), 0.0, 1.0);
		pbrInputs.VdotH = clamp(dot(v, h), 0.0, 1.0);

		// Calculate the shading terms for the microfacet specular shading model
		vec3 F = specularReflection(pbrInputs);
		float G = geometricOcclusion(pbrInputs);
		float D = microfacetDistribution(pbrInputs);

		float d_prime = length / (1 - length2 / (radius * radius));
		float att     = 1 + d_prime / radius;
		att           = att*att;
		float color   = (pbrInputs.NdotL * is_on / att );

		//vec3 color     = vec3(pbrInputs.NdotL);
		// Calculation of analytical lighting contribution
		diffuseContrib += color * (1.0 - F);
		specContrib    += color * F * (G * D / (4.0 * pbrInputs.NdotL * pbrInputs.NdotV));
	}

	vec3 color = diffuseContrib * diffuse(pbrInputs) + specContrib;

	frag_color = vec4(color, baseColor.a);
});
}

