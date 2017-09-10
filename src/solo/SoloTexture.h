/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "SoloCommon.h"
#include "SoloVector2.h"
#include <vector>

namespace solo
{
    class Device;
    class Texture2d;
    class Texture2dData;
    class CubeTexture;
    class CubeTextureData;

    enum class TextureWrapping
    {
        Clamp = 0,
        Repeat
    };

    enum class TextureFiltering
    {
        Nearest = 0,
        Linear,
        NearestMipmapNearest,
        LinearMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapLinear
    };

    enum class CubeTextureFace
    {
        Front = 0,
        Back,
        Left,
        Right,
        Top,
        Bottom
    };

    struct TextureFlags
    {
        static const uint32_t MinFilterNearest = 1 << 0;
        static const uint32_t MinFilterLinear = 1 << 1;
        static const uint32_t MinFilterNearestMipmapNearest = 1 << 2;
        static const uint32_t MinFilterLinearMipmapNearest = 1 << 3;
        static const uint32_t MinFilterNearestMipmapLinear = 1 << 4;
        static const uint32_t MinFilterLinearMipmapLinear = 1 << 5;
        static const uint32_t MagFilterNearest = 1 << 6;
        static const uint32_t MagFilterLinear = 1 << 7;
        static const uint32_t MagFilterNearestMipmapNearest = 1 << 8;
        static const uint32_t MagFilterLinearMipmapNearest = 1 << 9;
        static const uint32_t MagFilterNearestMipmapLinear = 1 << 10;
        static const uint32_t MagFilterLinearMipmapLinear = 1 << 11;
        static const uint32_t HorizontalWrapClamp = 1 << 12;
        static const uint32_t HorizontalWrapRepeat = 1 << 13;
        static const uint32_t VerticalWrapClamp = 1 << 14;
        static const uint32_t VerticalWrapRepeat = 1 << 15;
        static const uint32_t DepthWrapClamp = 1 << 16;
        static const uint32_t DepthWrapRepeat = 1 << 17;
    };

    enum class TextureFormat
    {
        Red,
        RGB, // Not supported in Vulkan (my driver doesn't like it)
        RGBA
    };

    class Texture
    {
    public:
        SL_DISABLE_COPY_AND_MOVE(Texture)

        virtual ~Texture() {}

        virtual void generateMipmaps() = 0;

        auto getHorizontalWrapping() const -> TextureWrapping { return horizontalWrapping; }
        auto getVerticalWrapping() const -> TextureWrapping { return verticalWrapping; }

        virtual void setWrapping(TextureWrapping wrap);
        void setHorizontalWrapping(TextureWrapping horizontalWrap);
        void setVerticalWrapping(TextureWrapping verticalWrap);

        auto getMinFiltering() const -> TextureFiltering { return minFiltering; }
        auto getMagFiltering() const -> TextureFiltering { return magFiltering; }

        void setFiltering(TextureFiltering filtering);
        void setMinFiltering(TextureFiltering filtering);
        void setMagFiltering(TextureFiltering filtering);

        auto getAnisotropyLevel() const -> float { return anisotropy; }
        void setAnisotropyLevel(float level) { anisotropy = level; }

    protected:
        uint32_t flags = 0;

        TextureWrapping horizontalWrapping = TextureWrapping::Clamp;
        TextureWrapping verticalWrapping = TextureWrapping::Clamp;

        TextureFiltering minFiltering = TextureFiltering::Linear;
        TextureFiltering magFiltering = TextureFiltering::Linear;

        float anisotropy = 1.0f;

        Texture();

        virtual void rebuildFlags();
    };

    class Texture2d: public Texture
    {
    public:
        static auto loadFromFile(Device *device, const std::string &path) -> sptr<Texture2d>;
        static auto createFromData(Device *device, Texture2dData *data) -> sptr<Texture2d>;
        static auto createEmpty(Device *device, uint32_t width, uint32_t height, TextureFormat format) -> sptr<Texture2d>;

        auto getDimensions() const -> Vector2 { return dimensions; }

    protected:
        Texture2d(Texture2dData *data);

        TextureFormat format;
        Vector2 dimensions;
    };

    class CubeTexture: public Texture
    {
    public:
        static auto loadFromFaceFiles(Device *device,
            const std::string &frontPath,
            const std::string &backPath,
            const std::string &leftPath,
            const std::string &rightPath,
            const std::string &topPath,
            const std::string &bottomPath) -> sptr<CubeTexture>;
        static auto create(Device *device, CubeTextureData *data) -> sptr<CubeTexture>;

        void setWrapping(TextureWrapping wrapping) override final;

        auto getDepthWrapping() const -> TextureWrapping { return depthWrapping; }
        void setDepthWrapping(TextureWrapping depthWrap);

    protected:
        TextureWrapping depthWrapping = TextureWrapping::Repeat;
        uint32_t dimension = 0;
        TextureFormat format = TextureFormat::RGB;

        CubeTexture(CubeTextureData *data);

        void rebuildFlags() override final;
    };

    inline void CubeTexture::setDepthWrapping(TextureWrapping wrapping)
    {
        this->depthWrapping = wrapping;
        rebuildFlags();
    }
}