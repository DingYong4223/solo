#include <png.h>
#include "SoloPngTextureLoader.h"
#include "SoloFileSystem.h"
#include "SoloTexture2D.h"

using namespace solo;


struct PngReadContext
{
	std::vector<uint8_t>* bytes;
	unsigned int offset;
};


static void readCallback(png_structp png, png_bytep data, png_size_t length)
{
	auto context = reinterpret_cast<PngReadContext*>(png_get_io_ptr(png));
	if (!context)
		png_error(png, "Error reading PNG.");
	memcpy(data, context->bytes->data() + context->offset, length);
	context->offset += length;
}


PngTextureLoader::PngTextureLoader(FileSystem* fs) :
	TextureLoader(fs)
{
}


bool PngTextureLoader::isLoadable(const std::string& uri)
{
	return uri.find(".png", uri.size() - 5) != std::string::npos;
}


shared<Texture2D> PngTextureLoader::load2D(const std::string& uri)
{
	auto bytes = fs->readBytes(uri);
	if (bytes.size() < 8 || png_sig_cmp(&bytes[0], 0, 8) != 0)
		THROW_FMT(EngineException, "Failed to read PNG file ", uri);

	auto png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	auto info = png_create_info_struct(png);
	if (setjmp(png_jmpbuf(png)))
	{
		png_destroy_read_struct(&png, &info, nullptr);
		THROW_FMT(EngineException, "Failed to read PNG file ", uri);
	}

	std::unique_ptr<PngReadContext> context(new PngReadContext{ &bytes, 8 });
	png_set_read_fn(png, reinterpret_cast<png_voidp>(context.get()), readCallback);
	png_set_sig_bytes(png, 8);
	png_read_png(png, info, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, nullptr);

	auto width = png_get_image_width(png, info);
	auto height = png_get_image_height(png, info);
	auto colorType = png_get_color_type(png, info);

	ColorFormat colorFormat;
	switch (colorType)
	{
	case PNG_COLOR_TYPE_RGB:
		colorFormat = ColorFormat::RGB;
		break;
	case PNG_COLOR_TYPE_RGBA:
		colorFormat = ColorFormat::RGBA;
		break;
	default:
		png_destroy_read_struct(&png, &info, nullptr);
		THROW_FMT(EngineException, "Unsupported PNG color type ", colorType);
	}

	auto stride = png_get_rowbytes(png, info);
	auto data = std::vector<uint8_t>(stride * height);
	auto rows = png_get_rows(png, info);
	for (unsigned int i = 0; i < height; ++i)
		memcpy(data.data() + stride * (height - 1 - i), rows[i], stride);

	png_destroy_read_struct(&png, &info, nullptr);

	auto tex = TextureFactory::create2D();
	tex->setData(colorFormat, data, width, height);
	return tex;
}