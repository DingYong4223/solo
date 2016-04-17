#include "SoloSpectator.h"
#include "SoloTransform.h"
#include "SoloDevice.h"
#include "SoloRadian.h"

using namespace solo;


Spectator::Spectator(const Node& node):
    ComponentBase(node),
    device(Device::get())
{
}


void Spectator::init()
{
    transform = node.getComponent<Transform>();
}


void Spectator::update()
{
    auto mouseMotion = device->getMouseMotion();
    auto dt = device->getTimeDelta();

    if (device->isMouseButtonDown(MouseButton::Left, true))
        device->setCursorCaptured(true);
    if (device->isMouseButtonReleased(MouseButton::Left))
        device->setCursorCaptured(false);

    if (device->isMouseButtonDown(MouseButton::Left, false))
    {
        if (mouseMotion.x != 0)
            transform->rotate(Vector3::unitY(), Radian(verticalRotationSpeed * dt * -mouseMotion.x), TransformSpace::World);
        if (mouseMotion.y != 0)
            transform->rotate(Vector3::unitX(), Radian(horizontalRotationSpeed * dt * -mouseMotion.y), TransformSpace::Self);
    }

    auto movement = Vector3::zero();
    if (device->isKeyPressed(KeyCode::W, false)) movement += transform->getLocalForward();
    if (device->isKeyPressed(KeyCode::S, false)) movement += transform->getLocalBack();
    if (device->isKeyPressed(KeyCode::A, false)) movement += transform->getLocalLeft();
    if (device->isKeyPressed(KeyCode::D, false)) movement += transform->getLocalRight();
    if (device->isKeyPressed(KeyCode::Q, false)) movement += transform->getLocalDown();
    if (device->isKeyPressed(KeyCode::E, false)) movement += transform->getLocalUp();
    movement.normalize();
    movement *= dt * movementSpeed;

    transform->translateLocal(movement);
}
