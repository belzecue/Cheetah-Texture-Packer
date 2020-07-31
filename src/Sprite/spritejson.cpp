#include "spritejson.h"
#include <fstream>
#include <climits>

#define ReadRequiredField fx::gltf::detail::ReadRequiredField
#define ReadOptionalField fx::gltf::detail::ReadOptionalField
#define WriteField        fx::gltf::detail::WriteField

static constexpr std::array<uint16_t, 4> IdentityAABB{0, 0, USHRT_MAX, USHRT_MAX};

namespace fx {
namespace gltf {
void from_json(nlohmann::json const & json, Asset & db);
void from_json(nlohmann::json const & json, Buffer & db);
void from_json(nlohmann::json const & json, BufferView & db);
void from_json(nlohmann::json const & json, Image & db);
void from_json(nlohmann::json const & json, Texture & db);
void from_json(nlohmann::json const & json, Sampler & db);
void from_json(nlohmann::json const & json, Material & db);

void to_json(nlohmann::json & json, Asset const& db);
void to_json(nlohmann::json & json, Buffer const& db);
void to_json(nlohmann::json & json, BufferView const& db);
void to_json(nlohmann::json & json, Image const& db);
void to_json(nlohmann::json & json, Texture const& db);
void to_json(nlohmann::json & json, Sampler const& db);
void to_json(nlohmann::json & json, Material const& db);
}}

namespace Sprites
{


namespace detail
{

inline Document Create(nlohmann::json const & json, DataContext const & dataContext, bool skip_buffers)
{
	Document document = json;

	if(skip_buffers)
		return document;

	LoadBuffers(document.buffers, dataContext);
	return document;
}

inline void Save(Document const& document, std::string const& documentFilePath, bool useBinaryFormat)
{
	nlohmann::json json = document;

	SaveInternal(std::move(json), document.buffers, documentFilePath, useBinaryFormat, SPRHeaderMagic);
}

};

inline void from_json(nlohmann::json const & json, Animation & db)
{
	ReadRequiredField("name",   json, db.name);
	ReadRequiredField("frames", json, db.frames);
	ReadRequiredField("fps",    json, db.fps);

	detail::ReadExtensionsAndExtras(json, db.extensionsAndExtras);
}

inline void to_json(nlohmann::json & json, Animation const& db)
{
	WriteField("name",   json, db.name);
	WriteField("frames", json, db.frames);
	WriteField("fps",    json, db.fps, 29.97f);

	detail::WriteExtensions(json, db.extensionsAndExtras);
}

inline void from_json(nlohmann::json const & json, Sprite::Frame & db)
{
	ReadOptionalField("attachments", json, db.attachments);
	ReadRequiredField("start",       json, db.start);
	ReadRequiredField("count",       json, db.count);
	ReadRequiredField("AABB",        json, db.AABB);
}

inline void to_json(nlohmann::json & json, Sprite::Frame const& db)
{
	WriteField("attachments", json, db.attachments);
	WriteField("start",       json, db.start, -1);
	WriteField("count",       json, db.count, -1);
	WriteField("AABB",        json, db.AABB, {0, 0, 0, 0});
}

inline void from_json(nlohmann::json const & json, Sprite & db)
{
	ReadRequiredField("name",         json, db.name);
	ReadRequiredField("indicies",     json, db.indices);
	ReadRequiredField("material",     json, db.material);

	ReadRequiredField("attributes",   json, db.attributes);

	ReadRequiredField("frames",       json, db.frames);

	ReadRequiredField("attachments",  json, db.attachments);
	ReadOptionalField("animations",   json, db.animations);

	detail::ReadExtensionsAndExtras(json, db.extensionsAndExtras);
}

inline void to_json(nlohmann::json & json, Sprite const& db)
{
	WriteField("name",         json, db.name);
	WriteField("indicies",     json, db.indices);
	WriteField("material",     json, db.material);

	WriteField("attributes",   json, db.attributes);

	WriteField("frames",       json, db.frames);

	WriteField("attachments",  json, db.attachments);
	WriteField("animations",   json, db.animations);

	detail::WriteExtensions(json, db.extensionsAndExtras);
}

inline void from_json(nlohmann::json const & json, Document & db)
{
	ReadRequiredField("asset",          json, db.asset);
	ReadRequiredField("sprites",        json, db.sprites);

	ReadRequiredField("accessors",      json, db.accessors);
	ReadRequiredField("buffers",        json, db.buffers);
	ReadRequiredField("bufferViews",    json, db.bufferViews);

	ReadRequiredField("materials",      json, db.materials);
	ReadOptionalField("textures",       json, db.textures);
	ReadOptionalField("images",         json, db.images);
	ReadOptionalField("samplers",       json, db.samplers);

	ReadOptionalField("extensionsUsed", json, db.extensionsUsed);
	ReadOptionalField("extensionsRequired", json, db.extensionsRequired);

	detail::ReadExtensionsAndExtras(json, db.extensionsAndExtras);
}

inline void to_json(nlohmann::json & json, Document const& db)
{
	WriteField("asset",          json, db.asset);
	WriteField("sprites",        json, db.sprites);

	WriteField("accessors",      json, db.accessors);
	WriteField("buffers",        json, db.buffers);
	WriteField("bufferViews",    json, db.bufferViews);

	WriteField("materials",      json, db.materials);
	WriteField("textures",       json, db.textures);
	WriteField("images",         json, db.images);
	WriteField("samplers",       json, db.samplers);

	WriteField("extensionsUsed", json, db.extensionsUsed);
	WriteField("extensionsRequired", json, db.extensionsRequired);

	detail::WriteExtensions(json, db.extensionsAndExtras);
}

Document LoadFromBinary(std::vector<uint8_t> binary, std::string const & documentFilePath, bool skip_buffers, ReadQuotas const & readQuotas)
{
	detail::SPRHeader header;
	std::memcpy(&header, &binary[0], detail::HeaderSize);

	if (header.magic != detail::SPRHeaderMagic ||
	    header.jsonHeader.chunkType != detail::SPRChunkJSON ||
	    header.jsonHeader.chunkLength + detail::HeaderSize > header.length)
	{
		throw invalid_document("Invalid GLB header");
	}

	return detail::Create(
		nlohmann::json::parse({ &binary[detail::HeaderSize], header.jsonHeader.chunkLength }),
		{ detail::GetDocumentRootPath(documentFilePath), readQuotas, &binary, header.jsonHeader.chunkLength + detail::HeaderSize },
			skip_buffers);
}

Document LoadFromText(std::string const & documentFilePath, bool skip_buffers, ReadQuotas const & readQuotas)
{
	nlohmann::json json;
	{
		std::ifstream file(documentFilePath);
		if (!file.is_open())
		{
			throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory));
		}

		file >> json;
	}

	return detail::Create(json, { detail::GetDocumentRootPath(documentFilePath), readQuotas }, skip_buffers);
}

Document LoadFromBinary(std::string const & documentFilePath, bool skip_buffers, ReadQuotas const & readQuotas)
{
	std::vector<uint8_t> binary{};
	{
		std::ifstream file(documentFilePath, std::ios::binary);

		if (!file.is_open())
		{
			throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), documentFilePath);
		}

		file.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

		const std::size_t fileSize = detail::GetFileSize(file);
		if (fileSize < detail::HeaderSize)
		{
			throw invalid_document("Invalid Sprite file");
		}

		if (fileSize > readQuotas.MaxFileSize)
		{
			throw invalid_document("Quota exceeded : file size > MaxFileSize");
		}

		binary.resize(fileSize);
		file.read(reinterpret_cast<char *>(&binary[0]), fileSize);
	}

	return Sprites::LoadFromBinary(binary, documentFilePath, skip_buffers, readQuotas);
}

Document & LoadExternalBuffers(Document & document, std::string const & documentFilePath, ReadQuotas const & readQuotas)
{
	detail::LoadBuffers(document.buffers, { detail::GetDocumentRootPath(documentFilePath), readQuotas });

	return document;
}

void Save(Document const & document, std::string documentFilePath, bool useBinaryFormat)
{
	detail::ValidateBuffers(document.buffers, useBinaryFormat);
	detail::Save(document, documentFilePath, useBinaryFormat);
}
#if 0
void AtlasFile::Load()
{
	std::ifstream file(path, std::ios::in);

	if (!file.is_open())
	{
		throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), path);
	}

	file.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

	std::string line;
	std::getline(file, line);

	if(line.find_first_of("textures: ") != 0)
		throw invalid_document("Atlas file improperly formatted, must start with 'textures: .*\\n'");

	imageFile = line.substr(11, std::string::npos);

	Frame frame;
	while(!file.eof())
	{
		std::getline(file, frame.image, '\t');
		std::getline(file, line, '\t');
		frame.bounding_box[0] = std::stoi(line);
		std::getline(file, line, '\t');
		frame.bounding_box[1] = std::stoi(line);
		std::getline(file, line, '\t');
		frame.bounding_box[2] = std::stoi(line);
		std::getline(file, line, '\t');
		frame.bounding_box[3] = std::stoi(line);
		std::getline(file, line, '\t');

		frame.crop_box[0] = std::stoi(line);
		std::getline(file, line, '\t');
		frame.crop_box[1] = std::stoi(line);
		std::getline(file, line, '\t');
		frame.crop_box[2] = std::stoi(line);
		std::getline(file, line, '\t');
		frame.crop_box[3] = std::stoi(line);

		std::getline(file, line, '\n');
		frame.rotated = (!line.empty() && line.front() == 'r');

		frames.push_back(frame);
	}

	file.close();
}
#endif

}
