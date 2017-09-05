/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "SoloCamera.h"
#include "SoloNode.h"
#include "SoloDevice.h"
#include "SoloRadian.h"
#include "SoloDegree.h"
#include "SoloScene.h"
#include "SoloRenderer.h"
#include "SoloRenderCommand.h"

using namespace solo;

const uint32_t ViewDirtyBit = 1;
const uint32_t ProjectionDirtyBit = 1 << 1;
const uint32_t ViewProjectionDirtyBit = 1 << 2;
const uint32_t InvViewDirtyBit = 1 << 3;
const uint32_t InvViewProjectionDirtyBit = 1 << 4;
const uint32_t AllProjectionDirtyBits = ProjectionDirtyBit | ViewProjectionDirtyBit | InvViewProjectionDirtyBit;

auto Camera::create(const Node &node) -> sptr<Camera>
{
    return std::shared_ptr<Camera>(new Camera(node));
}

Camera::Camera(const Node &node):
    ComponentBase(node),
    device(node.getScene()->getDevice()),
    renderer(device->getRenderer()),
    fov(Degree(60))
{
    auto canvasSize = device->getCanvasSize();
    viewport = {0, 0, canvasSize.x, canvasSize.y};
}

void Camera::init()
{
    transform = node.findComponent<Transform>();
    transform->addCallback(this);
    auto canvasSize = device->getCanvasSize();
    setAspectRatio(canvasSize.x / canvasSize.y);
}

void Camera::onTransformChanged(const Transform *)
{
    dirtyFlags |= ViewDirtyBit | ViewProjectionDirtyBit | InvViewDirtyBit | InvViewProjectionDirtyBit;
}

void Camera::setPerspective(bool perspective)
{
    ortho = !perspective;
    dirtyFlags |= AllProjectionDirtyBits;
}

void Camera::setFOV(const Radian &fov)
{
    this->fov = fov;
    dirtyFlags |= AllProjectionDirtyBits;
}

void Camera::setOrthoSize(const Vector2& size)
{
    orthoSize = size;
    dirtyFlags |= AllProjectionDirtyBits;
}

void Camera::setAspectRatio(float ratio)
{
    aspectRatio = ratio;
    dirtyFlags |= AllProjectionDirtyBits;
}

void Camera::setFar(float far)
{
    this->farClip = far;
    dirtyFlags |= AllProjectionDirtyBits;
}

void Camera::setNear(float near)
{
    this->nearClip = near;
    dirtyFlags |= AllProjectionDirtyBits;
}

auto Camera::getViewMatrix() const -> const Matrix
{
    if (dirtyFlags & ViewDirtyBit)
    {
        viewMatrix = transform->getWorldMatrix();
        viewMatrix.invert();
        dirtyFlags &= ~ViewDirtyBit;
    }
    return viewMatrix;
}

auto Camera::getInvViewMatrix() const -> const Matrix
{
    if (dirtyFlags & InvViewDirtyBit)
    {
        invViewMatrix = getViewMatrix();
        invViewMatrix.invert();
        dirtyFlags &= ~InvViewDirtyBit;
    }
    return invViewMatrix;
}

auto Camera::getProjectionMatrix() const -> const Matrix
{
    if (dirtyFlags & ProjectionDirtyBit)
    {
        if (ortho)
            projectionMatrix = Matrix::createOrthographic(orthoSize.x, orthoSize.y, nearClip, farClip);
        else
            projectionMatrix = Matrix::createPerspective(fov, aspectRatio, nearClip, farClip);
        dirtyFlags &= ~ProjectionDirtyBit;
    }
    return projectionMatrix;
}

auto Camera::getViewProjectionMatrix() const -> const Matrix
{
    if (dirtyFlags & ViewProjectionDirtyBit)
    {
        viewProjectionMatrix = getProjectionMatrix() * getViewMatrix();
        dirtyFlags &= ~ViewProjectionDirtyBit;
    }
    return viewProjectionMatrix;
}

auto Camera::getInvViewProjectionMatrix() const -> const Matrix
{
    if (dirtyFlags & InvViewProjectionDirtyBit)
    {
        invViewProjectionMatrix = getViewProjectionMatrix();
        invViewProjectionMatrix.invert();
        dirtyFlags &= ~InvViewProjectionDirtyBit;
    }
    return invViewProjectionMatrix;
}

void Camera::renderFrame(std::function<void()> render)
{
    renderer->addRenderCommand(RenderCommand::beginCamera(this));
    render();
    renderer->addRenderCommand(RenderCommand::endCamera(this));
}
