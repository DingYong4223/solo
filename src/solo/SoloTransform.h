/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "SoloComponent.h"
#include "SoloVector3.h"
#include "SoloQuaternion.h"
#include "SoloMatrix.h"
#include "SoloNode.h"
#include <vector>

namespace solo
{
    class Camera;
    class TransformCallback;
    class Transform;
    struct Radian;

    enum class TransformSpace
    {
        Self,
        Parent,
        World
    };

    class TransformCallback
    {
    public:
        virtual ~TransformCallback() {}

        virtual void onTransformChanged(const Transform *transform) = 0;
    };

    class Transform final: public ComponentBase<Transform>
    {
    public:
        explicit Transform(const Node &node);

        void init() override final;

        void addCallback(TransformCallback *callback);
        void removeCallback(TransformCallback *callback);

        void setParent(Transform *parent);
        auto getParent() const -> Transform* { return parent; }
        
        auto getChild(uint32_t index) const -> Transform* { return children[index]; }
        auto getChildrenCount() const -> uint32_t { return static_cast<uint32_t>(children.size()); }
        void clearChildren();

        auto getWorldScale() const -> Vector3 { return getWorldMatrix().getScale(); }
        auto getLocalScale() const -> Vector3 { return localScale; }

        auto getWorldRotation() const -> Quaternion { return getWorldMatrix().getRotation(); }
        auto getLocalRotation() const -> Quaternion { return localRotation; }

        auto getWorldPosition() const -> Vector3 { return getWorldMatrix().getTranslation(); }
        auto getLocalPosition() const -> Vector3 { return localPosition; }

        auto getWorldUp() const -> Vector3 { return getWorldMatrix().getUpVector(); }
        auto getLocalUp() const -> Vector3 { return getMatrix().getUpVector(); }

        auto getWorldDown() const -> Vector3 { return getWorldMatrix().getDownVector(); }
        auto getLocalDown() const -> Vector3 { return getMatrix().getDownVector(); }

        auto getWorldLeft() const -> Vector3 { return getWorldMatrix().getLeftVector(); }
        auto getLocalLeft() const -> Vector3 { return getMatrix().getLeftVector(); }

        auto getWorldRight() const -> Vector3 { return getWorldMatrix().getRightVector(); }
        auto getLocalRight() const -> Vector3 { return getMatrix().getRightVector(); }

        auto getWorldForward() const -> Vector3 { return getWorldMatrix().getForwardVector(); }
        auto getLocalForward() const -> Vector3 { return getMatrix().getForwardVector(); }

        auto getWorldBack() const -> Vector3 { return getWorldMatrix().getBackVector(); }
        auto getLocalBack() const -> Vector3 { return getMatrix().getBackVector(); }

        void translateLocal(const Vector3 &translation);
        void scaleLocal(const Vector3 &scale);

        void setLocalPosition(const Vector3 &position);
        void setLocalScale(const Vector3 &scale);

        void rotate(const Quaternion &rotation, TransformSpace space = TransformSpace::Self);
        void rotateByAxisAngle(const Vector3 &axis, const Radian &angle, TransformSpace space = TransformSpace::Self);

        void setLocalRotation(const Quaternion &rotation);
        void setLocalAxisAngleRotation(const Vector3 &axis, const Radian &angle);

        void lookAt(const Vector3 &target, const Vector3 &up);

        auto getMatrix() const -> Matrix;
        auto getWorldMatrix() const -> Matrix;
        auto getWorldViewMatrix(const Camera *camera) const -> Matrix;
        auto getWorldViewProjMatrix(const Camera *camera) const -> Matrix;
        auto getInvTransposedWorldViewMatrix(const Camera *camera) const -> Matrix;
        auto getInvTransposedWorldMatrix() const -> Matrix;

        auto transformPoint(const Vector3 &point) const -> Vector3;
        auto transformDirection(const Vector3 &direction) const -> Vector3;

    private:
        mutable uint32_t dirtyFlags = ~0;

        Transform *parent = nullptr;
        std::vector<Transform *> children;
        std::vector<TransformCallback *> callbacks;

        Vector3 localPosition;
        Vector3 localScale;
        Quaternion localRotation;
        mutable Matrix matrix;
        mutable Matrix worldMatrix;
        mutable Matrix invTransposedWorldMatrix;

        void setDirtyWithChildren(uint32_t flags) const;
        void setChildrenDirty(uint32_t flags) const;
        void notifyChanged() const;
    };
}