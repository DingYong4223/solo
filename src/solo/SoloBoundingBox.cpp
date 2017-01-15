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

#include "SoloBoundingBox.h"
#include "SoloPlane.h"
#include "SoloBoundingSphere.h"
#include "SoloFrustum.h"
#include "SoloRay.h"
#include <algorithm>

using namespace solo;


BoundingBox::BoundingBox(const Vector3 &min, const Vector3 &max):
    min(min),
    max(max)
{
}


auto BoundingBox::empty() -> const BoundingBox &
{
    static BoundingBox b;
    return b;
}


auto BoundingBox::getCorners() const -> std::vector<Vector3>
{
    return
    {
        // Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
        // Left-top-front
        {min.x, max.y, max.z},

        // Left-bottom-front
        {min.x, min.y, max.z},

        // Right-bottom-front
        {max.x, min.y, max.z},

        // Right-top-front
        {max.x, max.y, max.z},

        // Far face, specified counter-clockwise looking towards the origin from the negative z-axis.
        // Right-top-back.
        {max.x, max.y, min.z},

        // Right-bottom-back
        {max.x, min.y, min.z},

        // Left-bottom-back
        {min.x, min.y, min.z},

        // Left-top-back
        {min.x, max.y, min.z}
    };
}


auto BoundingBox::getCenter() const -> Vector3
{
    auto center = max - min;
    center *= 0.5f;
    center += min;
    return center;
}


bool BoundingBox::intersectsBoundingSphere(const BoundingSphere &sphere) const
{
    return sphere.intersectsBoundingBox(*this);
}


bool BoundingBox::intersectsBoundingBox(const BoundingBox &box) const
{
    return ((min.x >= box.min.x && min.x <= box.max.x) || (box.min.x >= min.x && box.min.x <= max.x)) &&
           ((min.y >= box.min.y && min.y <= box.max.y) || (box.min.y >= min.y && box.min.y <= max.y)) &&
           ((min.z >= box.min.z && min.z <= box.max.z) || (box.min.z >= min.z && box.min.z <= max.z));
}


bool BoundingBox::intersectsFrustum(const Frustum &frustum) const
{
    return intersectPlane(frustum.getNearPlane()) != PlaneIntersection::Back &&
           intersectPlane(frustum.getFarPlane()) != PlaneIntersection::Back &&
           intersectPlane(frustum.getLeftPlane()) != PlaneIntersection::Back &&
           intersectPlane(frustum.getRightPlane()) != PlaneIntersection::Back &&
           intersectPlane(frustum.getBottomPlane()) != PlaneIntersection::Back &&
           intersectPlane(frustum.getTopPlane()) != PlaneIntersection::Back;
}


auto BoundingBox::intersectPlane(const Plane &plane) const -> PlaneIntersection
{
    Vector3 center((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
    auto distance = plane.getDistanceToPoint(center);

    auto extentX = (max.x - min.x) * 0.5f;
    auto extentY = (max.y - min.y) * 0.5f;
    auto extentZ = (max.z - min.z) * 0.5f;

    const auto &planeNormal = plane.getNormal();
    if (fabsf(distance) <= fabsf(extentX * planeNormal.x) + fabsf(extentY * planeNormal.y) + fabsf(extentZ * planeNormal.z))
        return PlaneIntersection::Intersecting;

    return distance > 0.0f ? PlaneIntersection::Front : PlaneIntersection::Back;
}


auto BoundingBox::hitByRay(const Ray &ray) const -> float
{
    float tmin;
    float tmax;

    const auto &origin = ray.getOrigin();
    const auto &direction = ray.getDirection();

    auto div = 1.0f / direction.x;
    if (div >= 0.0f)
    {
        tmin = (min.x - origin.x) * div;
        tmax = (max.x - origin.x) * div;
    }
    else
    {
        tmin = (max.x - origin.x) * div;
        tmax = (min.x - origin.x) * div;
    }

    auto dnear = tmin;
    auto dfar = tmax;

    // Check if the ray misses the box.
    if (dnear > dfar || dfar < 0.0f)
        return -1;

    div = 1.0f / direction.y;
    if (div >= 0.0f)
    {
        tmin = (min.y - origin.y) * div;
        tmax = (max.y - origin.y) * div;
    }
    else
    {
        tmin = (max.y - origin.y) * div;
        tmax = (min.y - origin.y) * div;
    }

    if (tmin > dnear)
        dnear = tmin;
    if (tmax < dfar)
        dfar = tmax;

    // Check if the ray misses the box.
    if (dnear > dfar || dfar < 0.0f)
        return -1;

    div = 1.0f / direction.z;
    if (div >= 0.0f)
    {
        tmin = (min.z - origin.z) * div;
        tmax = (max.z - origin.z) * div;
    }
    else
    {
        tmin = (max.z - origin.z) * div;
        tmax = (min.z - origin.z) * div;
    }

    if (tmin > dnear)
        dnear = tmin;
    if (tmax < dfar)
        dfar = tmax;

    // Check if the ray misses the box.
    if (dnear > dfar || dfar < 0.0f)
        return -1;

    // The ray getIntersection the box (and since the direction of a Ray is normalized, dnear is the distance to the ray).
    return dnear;
}


void BoundingBox::mergeBoundingBox(const BoundingBox &box)
{
    min.x = std::min(min.x, box.min.x);
    min.y = std::min(min.y, box.min.y);
    min.z = std::min(min.z, box.min.z);

    max.x = std::max(max.x, box.max.x);
    max.y = std::max(max.y, box.max.y);
    max.z = std::max(max.z, box.max.z);
}


void BoundingBox::mergeBoundingSphere(const BoundingSphere &sphere)
{
    const auto &center = sphere.center;
    auto radius = sphere.radius;

    min.x = std::min(min.x, center.x - radius);
    min.y = std::min(min.y, center.y - radius);
    min.z = std::min(min.z, center.z - radius);

    max.x = std::max(max.x, center.x + radius);
    max.y = std::max(max.y, center.y + radius);
    max.z = std::max(max.z, center.z + radius);
}
