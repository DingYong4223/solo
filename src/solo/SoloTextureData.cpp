/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "SoloTextureData.h"
#include "stb/SoloSTBTextureData.h"

namespace solo
{
    class InMemoryTexture2DData final: public Texture2DData
    {
    public:
        explicit InMemoryTexture2DData(u32 width, u32 height, TextureFormat format, const vec<u8> &data):
            width(width),
            height(height),
            format(format),
            data(data)
        {
        }

        auto getSize() const -> u32 override final { return data.size(); }
        auto getWidth() const -> u32 override final { return width; }
        auto getHeight() const -> u32 override final { return height; }
        auto getData() const -> const void* override final { return data.data(); }
        auto getFormat() const -> TextureFormat override final { return format; }

    private:
        u32 width;
        u32 height;
        TextureFormat format;
        vec<u8> data;
    };
}

using namespace solo;

auto Texture2DData::loadFromFile(Device *device, const str &path) -> sptr<Texture2DData>
{
    if (STBTexture2DData::canLoadFromFile(path))
        return STBTexture2DData::loadFromFile(device, path);
    return panic<nullptr_t>(SL_FMT("Unsupported cube texture file ", path));
}

auto Texture2DData::createFromMemory(u32 width, u32 height, TextureFormat format,
    const vec<u8> &data) -> sptr<Texture2DData>
{
    return std::make_shared<InMemoryTexture2DData>(width, height, format, data);
}

auto CubeTextureData::loadFromFaceFiles(
    Device *device,
    const str& positiveXPath, const str& negativeXPath,
	const str& positiveYPath, const str& negativeYPath,
	const str& positiveZPath, const str& negativeZPath) -> sptr<CubeTextureData>
{
    if (STBCubeTextureData::canLoadFromFaceFiles(positiveXPath, negativeXPath, positiveYPath, negativeYPath, positiveZPath, negativeZPath))
        return STBCubeTextureData::loadFromFaceFiles(device, positiveXPath, negativeXPath, positiveYPath, negativeYPath, positiveZPath, negativeZPath);
    return panic<nullptr_t>(SL_FMT("Unsupported cube texture face files ", positiveXPath, ", ..."));
}
