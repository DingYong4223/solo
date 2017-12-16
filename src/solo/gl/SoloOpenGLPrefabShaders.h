/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "SoloCommon.h"

#ifdef SL_OPENGL_RENDERER

namespace solo
{
    struct OpenGLPrefabShaders
    {
        struct Vertex
        {
            static const s8 *skybox;
            static const s8 *font;
        };

        struct Fragment
        {
            static const s8 *skybox;
            static const s8 *font;
        };
    };
}

#endif
