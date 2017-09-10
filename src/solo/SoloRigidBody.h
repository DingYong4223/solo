/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "SoloCommon.h"
#include "SoloComponent.h"
#include "SoloVector3.h"
#include "SoloTransform.h"

namespace solo
{
    class Collider;

    // class because lua binding doesn't like structs :(
    class RigidBodyConstructionParameters
    {
    public:
        float mass = 0;
        float friction = 0;
        float restitution = 0;
        float linearDamping = 0;
        float angularDamping = 0;
        
        bool kinematic = false;

        Vector3 linearFactor;
        Vector3 angularFactor;
    };

    class RigidBody: public ComponentBase<RigidBody>, public TransformCallback
    {
    public:
        static auto create(const Node &node, const RigidBodyConstructionParameters &parameters) -> sptr<RigidBody>;

        virtual void setCollider(sptr<Collider> collider) = 0;

    protected:
        explicit RigidBody(const Node &node);
    };

    template <>
    template <class... Args>
    auto NodeHelper<RigidBody>::addComponent(Scene *scene, uint32_t nodeId, Args &&...args) -> RigidBody*
    {
        auto body = std::shared_ptr<RigidBody>(RigidBody::create(Node(scene, nodeId), std::forward<Args>(args)...));
        scene->addComponent(nodeId, body);
        return body.get();
    }
}