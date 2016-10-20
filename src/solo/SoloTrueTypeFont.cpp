/*
    Copyright (c) Aleksey Fedotov

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
        misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#include "SoloTrueTypeFont.h"
#include "SoloRectTexture.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

using namespace solo;


TrueTypeFont::TrueTypeFont(uint8_t* fontData, uint32_t size, uint32_t atlasWidth, uint32_t atlasHeight,
    uint32_t firstChar, uint32_t charCount, uint32_t oversampleX, uint32_t oversampleY):
    firstChar(firstChar)
{
    charInfo = std::make_unique<stbtt_packedchar[]>(charCount);
    
    auto pixels = std::make_unique<uint8_t[]>(atlasWidth * atlasHeight);

    stbtt_pack_context context;
    auto ret = stbtt_PackBegin(&context, pixels.get(), atlasWidth, atlasHeight, 0, 1, nullptr);
    SL_ERR_IF(!ret)

    stbtt_PackSetOversampling(&context, oversampleX, oversampleY);
    stbtt_PackFontRange(&context, fontData, 0, static_cast<float>(size), firstChar, charCount, charInfo.get());
    stbtt_PackEnd(&context);

    atlas = RectTexture::create();
    atlas->setFiltering(TextureFiltering::Linear);
    atlas->setData(TextureFormat::Red, pixels.get(), atlasWidth, atlasHeight);
    atlas->generateMipmaps();
}


auto TrueTypeFont::getGlyphInfo(uint32_t character, float offsetX, float offsetY) -> GlyphInfo
{
    stbtt_aligned_quad quad;
    auto atlasSize = atlas->getSize();

    stbtt_GetPackedQuad(charInfo.get(), static_cast<uint32_t>(atlasSize.x), static_cast<uint32_t>(atlasSize.y),
        character - firstChar, &offsetX, &offsetY, &quad, 1);
    auto xmin = quad.x0;
    auto xmax = quad.x1;
    auto ymin = -quad.y1;
    auto ymax = -quad.y0;

    auto result = GlyphInfo();
    result.offsetX = offsetX;
    result.offsetY = offsetY;
    result.positions[0] = Vector3(xmin, ymin, 0);
    result.positions[1] = Vector3(xmin, ymax, 0);
    result.positions[2] = Vector3(xmax, ymax, 0);
    result.positions[3] = Vector3(xmax, ymin, 0);
    result.uvs[0] = Vector2(quad.s0, quad.t1);
    result.uvs[1] = Vector2(quad.s0, quad.t0);
    result.uvs[2] = Vector2(quad.s1, quad.t0);
    result.uvs[3] = Vector2(quad.s1, quad.t1);
    return result; // TODO move
}


auto Font::create(uint8_t* fontData, uint32_t size, uint32_t atlasWidth, uint32_t atlasHeight,
    uint32_t firstChar, uint32_t charCount, uint32_t oversampleX, uint32_t oversampleY) -> sptr<Font>
{
    // TODO if constructors throws...
    return std::unique_ptr<Font>(new TrueTypeFont(fontData, size, atlasWidth, atlasHeight, firstChar, charCount, oversampleX, oversampleY));
}