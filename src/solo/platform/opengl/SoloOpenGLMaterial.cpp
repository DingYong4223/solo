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

#include "SoloOpenGLMaterial.h"
#include "SoloDevice.h"
#include "SoloOpenGLRenderer.h"


#ifdef SL_OPENGL_RENDERER

using namespace solo;


OpenGLMaterial::OpenGLMaterial(Device *device, sptr<Effect> effect):
    Material(device, effect)
{
    renderer = dynamic_cast<OpenGLRenderer *>(device->getRenderer());
}


void OpenGLMaterial::applyState()
{
    renderer->setFaceCull(faceCull);
    renderer->setPolygonMode(polygonMode);
    renderer->setDepthTest(depthTest);
    renderer->setDepthWrite(depthWrite);
    renderer->setDepthFunction(depthFunc);
    renderer->setBlend(transparent);
    renderer->setBlendFactor(srcBlendFactor, dstBlendFactor);
}

#else
#   error OpenGL renderer is not supported on this platform
#endif