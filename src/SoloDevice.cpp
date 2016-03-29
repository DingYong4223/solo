#include "SoloDevice.h"
#include "SoloFileSystem.h"
#include "SoloResourceManager.h"
#include "SoloScene.h"
#include "SoloLogger.h"
#include "SoloRenderer.h"
#include "SoloGraphics.h"
#include "platform/stub/SoloStubDevice.h"
#include "platform/opengl/SoloSDLOpenGLDevice.h"

using namespace solo;


uptr<Device> Device::instance = nullptr;


Device* Device::init(const DeviceCreationArgs& args)
{
    if (!instance)
    {
        if (args.mode == DeviceMode::OpenGL)
            instance = std::unique_ptr<Device>(new SDLOpenGLDevice(args));
        else
            instance = std::unique_ptr<Device>(new StubDevice(args));
    }

    return instance.get();
}


Device* Device::get()
{
    return instance.get();
}


void Device::shutdown()
{
    if (instance)
        instance = nullptr;
}


Device::~Device()
{
    // Keep order!

    if (scene)
        scene.reset();

    if (graphics)
        graphics.reset();

    if (resourceManager)
        resourceManager.reset();

    if (fs)
        fs.reset();

    if (renderer)
        renderer.reset();

    if (logger)
        logger.reset();
}


Device::Device(const DeviceCreationArgs& args):
    creationArgs(args)
{
    logger = std::make_unique<Logger>(DeviceToken());
    if (!args.logFilePath.empty())
        logger->setTargetFile(args.logFilePath);

    renderer = Renderer::create(this, DeviceToken());
    fs = FileSystem::create(this, DeviceToken());
    resourceManager = std::make_unique<ResourceManager>(DeviceToken());
    graphics = std::make_unique<Graphics>(this, DeviceToken());
    scene = std::make_unique<Scene>(DeviceToken());
}


void Device::run()
{
    // By design you are allowed to call this method again and again
    // as long as the engine object is alive. Once the engine object has been destroyed (or had it's shutdown()
    // method called), this method will no longer work, because the object is considered disposed.
    running = true;
    while (true)
    {
        resourceManager->update();
        beginUpdate();
        scene->update();
        scene->render();
        endUpdate();
        if (!running)
            break;
    }
}


bool Device::isKeyPressed(KeyCode code, bool firstTime) const
{
    auto where = pressedKeys.find(code);
    return where != pressedKeys.end() && (!firstTime || where->second);
}


bool Device::isKeyReleased(KeyCode code) const
{
    return releasedKeys.find(code) != releasedKeys.end();
}


Vector2 Device::getMouseMotion() const
{
    return Vector2(static_cast<float>(mouseDeltaX), static_cast<float>(mouseDeltaY));
}


bool Device::isMouseButtonDown(MouseButton button, bool firstTime) const
{
    auto where = pressedMouseButtons.find(button);
    return where != pressedMouseButtons.end() && (!firstTime || where->second);
}


bool Device::isMouseButtonReleased(MouseButton button) const
{
    return releasedMouseButtons.find(button) != releasedMouseButtons.end();
}


void Device::updateTime()
{
    auto time = getLifetime();
    timeDelta = time - lastUpdateTime;
    lastUpdateTime = time;
}
