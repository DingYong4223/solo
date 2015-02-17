#include "SoloTexture.h"
#include "platform/SoloOpenGLTexture2D.h"

using namespace solo;


shared<Texture2D> TextureFactory::create2D(std::vector<char> data)
{
	return NEW2(OpenGLTexture2D);
}