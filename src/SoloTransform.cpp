#include "SoloTransform.h"
#include "SoloCamera.h"
#include <algorithm>

using namespace solo;


const unsigned DirtyBitPosition = 1;
const unsigned DirtyBitRotation = 2;
const unsigned DirtyBitScale = 4;
const unsigned DirtyBitWorld = 8;
const unsigned DirtyBitInverseTransposedWorld = 16;
const unsigned DirtyBitAll =
    DirtyBitPosition |
    DirtyBitRotation |
    DirtyBitScale |
    DirtyBitWorld |
    DirtyBitInverseTransposedWorld;


Transform::Transform(Node node):
    ComponentBase(node)
{
}


void Transform::notifyChanged() const
{
    for (auto callback : callbacks)
        callback->onTransformChanged(this);
}


void Transform::init()
{
    localScale = Vector3::unit();
    dirtyFlags.add(DirtyBitAll);
}


void Transform::addCallback(TransformCallback* callback)
{
    callbacks.push_back(callback);
}


void Transform::removeCallback(TransformCallback* callback)
{
    callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), callback), callbacks.end());
}


void Transform::setParent(Transform* parent)
{
    if (parent == this || parent == this->parent)
        return;
    if (this->parent)
    {
        auto& parentChildren = this->parent->children;
        this->parent->children.erase(std::remove(parentChildren.begin(), parentChildren.end(), this), parentChildren.end());
    }
    this->parent = parent;
    if (parent)
        parent->children.push_back(this);
    setDirtyWithChildren(DirtyBitWorld);
}


Transform* Transform::getParent() const
{
    return parent;
}


Transform* Transform::getChild(int index) const
{
    return children[index];
}


void Transform::removeChildren()
{
    while (!children.empty())
    {
        auto child = *children.begin();
        child->setParent(nullptr);
    }
}


int Transform::getChildrenCount() const
{
    return static_cast<int>(children.size());
}


Vector3 Transform::getLocalScale() const
{
    return localScale;
}


Quaternion Transform::getWorldRotation() const
{
    return getWorldMatrix().getRotation();
}


Vector3 Transform::getWorldScale() const
{
    return getWorldMatrix().getScale();
}


Quaternion Transform::getLocalRotation() const
{
    return getMatrix().getRotation();
}


Matrix Transform::getMatrix() const
{
    if (!dirtyFlags.isEmpty())
    {
        auto hasTranslation = !localPosition.isZero();
        auto hasScale = !localScale.isUnit();
        auto hasRotation = !localRotation.isIdentity();

        if (hasTranslation || dirtyFlags.isSet(DirtyBitPosition))
        {
            matrix = Matrix::createTranslation(localPosition);
            if (hasRotation || dirtyFlags.isSet(DirtyBitRotation))
                matrix.rotate(localRotation);
            if (hasScale || dirtyFlags.isSet(DirtyBitScale))
                matrix.scale(localScale);
        }
        else if (hasRotation || dirtyFlags.isSet(DirtyBitRotation))
        {
            matrix = Matrix::createRotation(localRotation);
            if (hasScale || dirtyFlags.isSet(DirtyBitScale))
                matrix.scale(localScale);
        }
        else if (hasScale || dirtyFlags.isSet(DirtyBitScale))
            matrix = Matrix::createScale(localScale);

        dirtyFlags.remove(DirtyBitAll);
    }
    return matrix;
}


Matrix Transform::getWorldMatrix() const
{
    if (dirtyFlags.checkAndRemove(DirtyBitWorld))
    {
        if (parent)
            worldMatrix = parent->getWorldMatrix() * getMatrix();
        else
            worldMatrix = getMatrix();
        setChildrenDirty(DirtyBitWorld);
        setDirtyWithChildren(DirtyBitInverseTransposedWorld);
    }
    return worldMatrix;
}


Matrix Transform::getInverseTransposedWorldMatrix() const
{
    if (dirtyFlags.checkAndRemove(DirtyBitInverseTransposedWorld) || dirtyFlags.isSet(DirtyBitWorld))
    {
        inverseTransposedWorldMatrix = getWorldMatrix();
        inverseTransposedWorldMatrix.invert();
        inverseTransposedWorldMatrix.transpose();
        setChildrenDirty(DirtyBitInverseTransposedWorld);
    }
    return inverseTransposedWorldMatrix;
}


Matrix Transform::getWorldViewMatrix(Camera* camera) const
{
    return camera->getViewMatrix() * getWorldMatrix();
}


Matrix Transform::getWorldViewProjectionMatrix(Camera* camera) const
{
    return camera->getViewProjectionMatrix() * getWorldMatrix();
}


Matrix Transform::getInverseTransposedWorldViewMatrix(Camera* camera) const
{
    auto result = camera->getViewMatrix() * getWorldMatrix();
    result.invert();
    result.transpose();
    return result;
}


void Transform::translateLocal(const Vector3& translation)
{
    localPosition += translation;
    setDirtyWithChildren(DirtyBitPosition | DirtyBitWorld);
}


void Transform::rotate(const Quaternion& rotation, TransformSpace space)
{
    auto normalizedRotation(const_cast<Quaternion&>(rotation));
    normalizedRotation.normalize();

    switch (space)
    {
    case TransformSpace::Self:
        localRotation = localRotation * normalizedRotation;
        setDirtyWithChildren(DirtyBitRotation | DirtyBitWorld);
        break;
    case TransformSpace::Parent:
        localRotation = normalizedRotation * localRotation;
        setDirtyWithChildren(DirtyBitRotation | DirtyBitWorld);
        break;
    case TransformSpace::World:
        {
            auto inverseWorldRotation = getWorldRotation();
            inverseWorldRotation.inverse();
            localRotation = localRotation * inverseWorldRotation * normalizedRotation * getWorldRotation();
            setDirtyWithChildren(DirtyBitRotation | DirtyBitWorld);
            break;
        }
    }
}


void Transform::rotate(const Vector3& axis, float angleRadians, TransformSpace space)
{
    auto rotation = Quaternion::createFromAxisAngle(axis, angleRadians);
    rotate(rotation, space);
}


void Transform::scaleLocal(const Vector3& scale)
{
    localScale.x *= scale.x;
    localScale.y *= scale.y;
    localScale.z *= scale.z;
    setDirtyWithChildren(DirtyBitScale | DirtyBitWorld);
}


void Transform::setLocalScale(const Vector3& scale)
{
    localScale = scale;
    setDirtyWithChildren(DirtyBitScale | DirtyBitWorld);
}


void Transform::lookAt(const Vector3& target, const Vector3& up)
{
    auto finalTarget = target;
    auto finalUp = up;

    if (parent)
    {
        auto m(parent->getWorldMatrix());
        m.invert();
        finalTarget = m.transformPoint(target);
        finalUp = m.transformDirection(up);
    }

    auto lookAtMatrix = Matrix::createLookAt(localPosition, finalTarget, finalUp);
    setLocalRotation(lookAtMatrix.getRotation());
}


Vector3 Transform::transformPoint(const Vector3& point) const
{
    return getMatrix().transformPoint(point);
}


Vector3 Transform::transformDirection(const Vector3& direction) const
{
    return getMatrix().transformDirection(direction);
}


void Transform::setLocalRotation(const Quaternion& rotation)
{
    localRotation = rotation;
    setDirtyWithChildren(DirtyBitRotation | DirtyBitWorld);
}


void Transform::setLocalRotation(const Vector3& axis, float angleRadians)
{
    localRotation = Quaternion::createFromAxisAngle(axis, angleRadians);
    setDirtyWithChildren(DirtyBitRotation | DirtyBitWorld);
}


void Transform::setLocalPosition(const Vector3& position)
{
    localPosition = position;
    setDirtyWithChildren(DirtyBitPosition | DirtyBitWorld);
}


Vector3 Transform::getLocalPosition() const
{
    return localPosition;
}


Vector3 Transform::getWorldPosition() const
{
    return getWorldMatrix().getTranslation();
}


Vector3 Transform::getLocalUp() const
{
    return getMatrix().getUpVector();
}


Vector3 Transform::getLocalDown() const
{
    return getMatrix().getDownVector();
}


Vector3 Transform::getLocalLeft() const
{
    return getMatrix().getLeftVector();
}


Vector3 Transform::getLocalRight() const
{
    return getMatrix().getRightVector();
}


Vector3 Transform::getLocalForward() const
{
    return getMatrix().getForwardVector();
}


Vector3 Transform::getLocalBack() const
{
    return getMatrix().getBackVector();
}


void Transform::setDirtyWithChildren(unsigned flags) const
{
    dirtyFlags.add(flags);
    notifyChanged();
    for (auto child : children)
        child->setDirtyWithChildren(flags);
}


void Transform::setChildrenDirty(unsigned flags) const
{
    for (auto child : children)
        child->setDirtyWithChildren(flags);
}
