#include "image.h"
#include "Support/imagesupport.h"
#include "Shaders/defaultvaos.h"
#include "Shaders/transparencyshader.h"
#include "Shaders/blitshader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fx/gltf.h>
#include <cctype>
#include <cstring>
#include <climits>

#include "widgets/glviewwidget.h"

std::mutex                       Image::g_mutex;

counted_ptr<Image> Image::Factory(ImageManager * manager, std::string const& documentFilePath)
{
	if(documentFilePath.empty())
		return nullptr;

	counted_string path      = counted_string::MakeShared(documentFilePath);

	std::lock_guard<std::mutex> lock(g_mutex);

	auto itr = manager->loadedImages.find(path);
	if(itr != manager->loadedImages.end())
		return CountedWrap(itr->second);

	const std::size_t slash_pos = documentFilePath.find_last_of("/\\");
	const std::size_t period_pos = documentFilePath.find_last_of(".");

	counted_string directory = counted_string::MakeShared(documentFilePath.substr(0, slash_pos));
	counted_string filename = counted_string::MakeShared(documentFilePath.substr(slash_pos+1, period_pos-(slash_pos+1)));

	auto ptr = UncountedWrap(new Image(manager, path, std::move(directory), std::move(filename)));
	manager->loadedImages.emplace(path, ptr.get());
	return ptr;
}

Image::Image(ImageManager * manager, counted_string const& path, counted_string directory, counted_string filename) :
	m_manager(manager),
	m_path(path),
	m_directory(directory),
	m_filename(filename)
{
	glDefaultVAOs::AddRef();
	TransparencyShader::Shader.AddRef();
	BlitShader::Shader.AddRef();

//	memset(m_vao, 0, sizeof(m_vao));
//	memset(m_vbo, 0, sizeof(m_vbo));
}

Image::~Image()
{
	std::lock_guard<std::mutex> lock(g_mutex);
	auto itr = m_manager->loadedImages.find(m_path);

	if(itr != m_manager->loadedImages.end())
		m_manager->loadedImages.erase(itr);

	glDefaultVAOs::Release(m_manager->gl);
	TransparencyShader::Shader.Release(m_manager->gl);
	BlitShader::Shader.Release(m_manager->gl);
}

void Image::LoadFromFile()
{
	if(m_texture != 0 || m_path.empty())
		return;

	auto gl = m_manager->gl;
	gl->makeCurrent();

	auto image = IO::LoadImage(m_path.c_str());
	m_size      = image.size;
	m_channels  = image.channels;

	m_ownsTexture = true;
	IO::UploadImage(gl, &m_texture, &image.image[0], image.size, image.channels);

	if(m_sprites.empty())
	{
		m_sprites    = IO::GetSprites(&image.image[0], image.size, image.channels);
		m_cropped    = IO::GetCrop(&image.image[0], image.size, image.channels, m_sprites);
	//	m_cropped    = m_sprites;
		m_normalized = IO::NormalizeCrop(m_cropped, image.size);

		if(m_cropped.merge(m_sprites))
			m_normalizedPositions = m_normalized;
		else
			m_normalizedPositions = IO::NormalizeCrop(m_sprites, image.size);


#if HAVE_CHROMA_KEY
		CropHistory hist;
		hist.cropped    = m_cropped;
		hist.normalized = m_normalized;
		hist.color_mask = 0x000000FF;
		hist.color_key  = 0x00000000;

		m_cropHistory.push_back(hist);
#endif
	}

	gl->doneCurrent();
}


#if HAVE_CHROMA_KEY
void Image::UpdateChromaKey(uint32_t color_mask, uint32_t color_key)
{
	for(auto c : m_cropHistory)
	{
		if(color_mask == c.color_mask && color_key == c.color_key)
		{
			m_cropped    = c.cropped;
			m_normalized = c.normalized;
			return;
		}
	}

	auto gl = m_manager->gl;

	LoadFromFile();

	IO::Image image;
	IO::DownloadImage(gl, image, m_texture);

	m_cropped = IO::GetCrop(&image.image[0], image.size, image.channels, m_sprites, color_mask, color_key);
	m_normalized = IO::NormalizeCrop(m_cropped, image.size);

	for(auto c : m_cropHistory)
	{
		if(m_cropped.merge(c.cropped))
			break;
	}

	for(auto c : m_cropHistory)
	{
		if(m_normalized.merge(c.normalized))
			break;
	}

	CropHistory hist;
	hist.cropped    = m_cropped;
	hist.normalized = m_normalized;
	hist.color_mask = color_mask;
	hist.color_key  = color_key;

	m_cropHistory.push_back(hist);
	UploadVertexArrays();
	gl->doneCurrent();
}
#endif

void Image::Clear()
{
	auto gl = m_manager->gl;
	if(m_ownsTexture)
		_gl glDeleteTextures(1, &m_texture);
/*
	_gl glDeleteVertexArrays(VAOc, m_vao);
	_gl glDeleteBuffers     (VBOc, m_vbo);

	memset(m_vao, 0, sizeof(m_vao));
	memset(m_vbo, 0, sizeof(m_vbo));*/
}

template<typename T, glm::qualifier Q>
void UploadTextureData(GLViewWidget * gl, CountedSizedArray<glm::vec<4, T, Q>> array, glm::vec<4, T, Q> base, T max_y)
{
	GL_ASSERT;
	std::vector<glm::vec<2, T, Q> > vec;
	vec.reserve((1 + array.size()) * 4);

	vec.push_back({base.x, max_y - base.y});
	vec.push_back({base.z, max_y - base.y});
	vec.push_back({base.z, max_y - base.w});
	vec.push_back({base.x, max_y - base.w});

	for(uint32_t i = 0; i < array.size(); ++i)
	{
		vec.push_back({array[i].x, max_y - array[i].y});
		vec.push_back({array[i].z, max_y - array[i].y});
		vec.push_back({array[i].z, max_y - array[i].w});
		vec.push_back({array[i].x, max_y - array[i].w});
	}

	_gl glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(vec[0]), &vec[0], GL_STATIC_DRAW);
	GL_ASSERT;
}

template<typename T, glm::qualifier Q>
void UploadTextureData(GLViewWidget * gl, CountedSizedArray<glm::vec<4, T, Q>> array, glm::vec<4, T, Q> base)
{
	GL_ASSERT;
	std::vector<glm::vec<2, T, Q> > vec;
	vec.reserve((1 + array.size()) * 4);

	vec.push_back({base.x, base.y});
	vec.push_back({base.z, base.y});
	vec.push_back({base.z, base.w});
	vec.push_back({base.x, base.w});

	for(uint32_t i = 0; i < array.size(); ++i)
	{
		vec.push_back({array[i].x, array[i].y});
		vec.push_back({array[i].z, array[i].y});
		vec.push_back({array[i].z, array[i].w});
		vec.push_back({array[i].x, array[i].w});
	}

	_gl glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(vec[0]), &vec[0], GL_STATIC_DRAW);
	GL_ASSERT;
}


/*
void Image::UploadVertexArrays()
{
	if(m_texture == 0)
		return;

	auto gl = m_manager->gl;

	GL_ASSERT;

//-------------------------------
// SET UP VAOs
//-------------------------------
	if(!m_vao[0])
	{
		_gl glGenVertexArrays(VAOc, &m_vao[0]);
		_gl glGenBuffers(VBOc, &m_vbo[0]);

		_gl glBindVertexArray(m_vao[0]);
		glDefaultVAOs::BindSquareIndexVBO(gl);GL_ASSERT;

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[SpriteCoords]);
		_gl glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, 0, nullptr);GL_ASSERT;

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[Centers]);
		_gl glVertexAttribPointer(1, 2, GL_SHORT, GL_FALSE, 0, nullptr);GL_ASSERT;

		_gl glEnableVertexAttribArray(0);
		_gl glEnableVertexAttribArray(1);GL_ASSERT;

		_gl glBindVertexArray(m_vao[1]);
		glDefaultVAOs::BindSquareIndexVBO(gl);GL_ASSERT;

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[CropBoxes]);
		_gl glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, 0, nullptr);GL_ASSERT;

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[Centers]);
		_gl glVertexAttribPointer(1, 2, GL_SHORT, GL_FALSE, 0, nullptr);GL_ASSERT;

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[TexCoord]);
		_gl glVertexAttribPointer(2, 2, GL_UNSIGNED_SHORT, GL_TRUE, 0, nullptr);GL_ASSERT;

		_gl glEnableVertexAttribArray(0);
		_gl glEnableVertexAttribArray(1);
		_gl glEnableVertexAttribArray(3);GL_ASSERT;

//upload perminant things
		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[SpriteCoords]);
		UploadTextureData(gl, m_sprites, glm::i16vec4(0, 0, m_size.x, m_size.y), m_size.y);GL_ASSERT;

		std::vector<glm::i16vec2> vec;
		vec.reserve((m_sprites.size()+1));GL_ASSERT;

		vec.push_back({m_size.x/2, m_size.y/2});
		vec.push_back({m_size.x/2, m_size.y/2});
		vec.push_back({m_size.x/2, m_size.y/2});
		vec.push_back({m_size.x/2, m_size.y/2});

		for(uint32_t i = 0; i < m_sprites.size(); ++i)
		{
			vec.push_back({(m_sprites[i].x + m_sprites[i].z)/2, m_size.y - (m_sprites[i].y + m_sprites[i].w)/2});
			vec.push_back({(m_sprites[i].x + m_sprites[i].z)/2, m_size.y - (m_sprites[i].y + m_sprites[i].w)/2});
			vec.push_back({(m_sprites[i].x + m_sprites[i].z)/2, m_size.y - (m_sprites[i].y + m_sprites[i].w)/2});
			vec.push_back({(m_sprites[i].x + m_sprites[i].z)/2, m_size.y - (m_sprites[i].y + m_sprites[i].w)/2});
		}

		_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[Centers]);
		_gl glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(vec[0]), &vec[0], GL_STATIC_DRAW);GL_ASSERT;
	}

	_gl glBindVertexArray(m_vao[0]);

	_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[CropBoxes]);
	UploadTextureData(gl, m_cropped, glm::i16vec4(0, 0, m_size.x, m_size.y), m_size.y);GL_ASSERT;

	_gl glBindBuffer(GL_ARRAY_BUFFER, m_vbo[TexCoord]);
	UploadTextureData(gl, m_normalized, glm::u16vec4(0, 0, USHRT_MAX, USHRT_MAX));GL_ASSERT;
}

void Image::Render(GLViewWidget * gl, int frame, int outline)
{
	if(m_sprites.empty())
		return;

	GL_ASSERT;
	assert(gl == m_manager->gl);

	if(frame >= 0) frame %= m_sprites.size();

	uint32_t elements = frame < 0? 6 * m_sprites.size() : 6;
	intptr_t first = 6 * (1 + frame * (frame > 0));
	void * offset   = (void*)(2 * first);
	bool center = (frame >= 0);

	glm::mat4 matrix = glm::mat4(1);

	if(!center) matrix =  glm::translate(matrix, -glm::vec3(m_size.x/2.f, m_size.y/2.f, 0));

	if(outline < 0)
	{
//draw backdrop
		_gl glBindVertexArray(m_vao[0]);

		if(frame < 0)
		{
			TransparencyShader::Shader.bind(gl);
			TransparencyShader::Shader.bindCenter(gl, center);
			TransparencyShader::Shader.bindMatrix(gl, matrix);

			_gl glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);GL_ASSERT;
		}

//draw sprites boxes
		BlitShader::Shader.bind(gl);
		BlitShader::Shader.bindCenter(gl, center);
		BlitShader::Shader.bindMatrix(gl, matrix);

		BlitShader::Shader.bindLayer(gl, 3);
		BlitShader::Shader.bindColor(gl, glm::vec4(0, 0, 0, 0));GL_ASSERT;

		_gl glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_SHORT, offset);GL_ASSERT;

//draw sprite outlines
		_gl glBindVertexArray(m_vao[1]);

		if(frame < 0)
		{
			BlitShader::Shader.bindLayer(gl, 2);
			BlitShader::Shader.bindColor(gl, glm::vec4(1, 1, 1, 1));
			_gl glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);GL_ASSERT;
		}

//draw sprites
		_gl glDisable(GL_DEPTH_TEST);
		BlitShader::Shader.bindLayer(gl, 4);
		BlitShader::Shader.bindTexture(gl, m_texture);GL_ASSERT;
		BlitShader::Shader.clearColor(gl);
		_gl glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_SHORT, offset);
	}

	else
	{
		BlitShader::Shader.bind(gl);
		BlitShader::Shader.bindLayer(gl, 1);
		BlitShader::Shader.bindCenter(gl, frame >= 0);
		BlitShader::Shader.bindColor(gl, glm::vec4(1, 1, 0, 0));GL_ASSERT;

		_gl glBindVertexArray(m_vao[outline % 2]);
		_gl glDrawArrays(GL_TRIANGLES, first, elements);
	}

	GL_ASSERT;
}
*/
