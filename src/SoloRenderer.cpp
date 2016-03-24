#include "SoloRenderer.h"
#include "SoloDevice.h"
#include "platform/opengl/SoloOpenGLRenderer.h"
#include "platform/stub/SoloStubRenderer.h"

using namespace solo;


sptr<Renderer> Renderer::create(Device* device)
{
    if (device->getMode() == DeviceMode::OpenGL)
        return SL_WRAP_SPTR(OpenGLRenderer, device);
    return SL_WRAP_SPTR(StubRenderer, device);
}


Renderer::Renderer(Device* device):
    device(device)
{
}


Renderer::~Renderer()
{
}
