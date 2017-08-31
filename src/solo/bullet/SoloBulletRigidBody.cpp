/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "SoloBulletRigidBody.h"
#include "SoloDevice.h"
#include "SoloBulletPhysics.h"
#include "SoloTransform.h"
#include "SoloCollider.h"
#include "SoloBulletCollider.h"
#include "SoloNode.h"
#include "SoloBulletCommon.h"

using namespace solo;

class MotionState final: public btMotionState
{
public:
    explicit MotionState(Transform *transform): transform(transform)
    {
    }

    void getWorldTransform(btTransform &worldTransform) const override final
    {
        auto worldPos = transform->getWorldPosition();
        auto rotation = transform->getWorldRotation();
        worldTransform.setOrigin(SL_TOBTVEC3(worldPos));
        worldTransform.setRotation(SL_TOBTQTRN(rotation));
    }

    void setWorldTransform(const btTransform &worldTransform) override final
    {
        SL_PANIC_IF(transform->getParent())
        transform->setLocalPosition(SL_FROMBTVEC3(worldTransform.getOrigin()));
        transform->setLocalRotation(SL_FROMBTQTRN(worldTransform.getRotation()));
    }

private:
    Transform *transform;
};

bullet::RigidBody::RigidBody(const Node &node, const RigidBodyConstructionParameters &parameters):
    solo::RigidBody(node),
    mass(parameters.mass),
    shape(nullptr)
{
    world = static_cast<Physics *>(node.getScene()->getDevice()->getPhysics())->getWorld();
    transformCmp = node.findComponent<Transform>();
    motionState = std::make_unique<MotionState>(transformCmp);

    btRigidBody::btRigidBodyConstructionInfo info(parameters.mass, motionState.get(), nullptr);
    info.m_friction = parameters.friction;
    info.m_restitution = parameters.restitution;
    info.m_linearDamping = parameters.linearDamping;
    info.m_angularDamping = parameters.angularDamping;

    body = std::make_unique<btRigidBody>(info);
    body->setUserPointer(this);
}

bullet::RigidBody::~RigidBody()
{
    world->removeRigidBody(body.get());
}

void bullet::RigidBody::setCollider(sptr<solo::Collider> newCollider)
{
    if (newCollider)
    {
        collider = newCollider; // store ownership
        shape = std::dynamic_pointer_cast<Collider>(collider)->getShape();

        btVector3 inertia;
        shape->calculateLocalInertia(mass, inertia);
        syncScale();

        body->setCollisionShape(shape);
        body->setMassProps(mass, inertia);

        world->addRigidBody(body.get());
    }
    else
    {
        world->removeRigidBody(body.get());
        collider = nullptr;
        shape = nullptr;
    }
}

void bullet::RigidBody::onTransformChanged(const Transform *transform)
{
    if (shape)
        syncScale();
}

void bullet::RigidBody::syncScale()
{
    auto scale = transformCmp->getLocalScale();
    shape->setLocalScaling(SL_TOBTVEC3(scale));
}