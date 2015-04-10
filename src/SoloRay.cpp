#include "SoloRay.h"
#include "SoloMath.h"
#include "SoloBoundingSphere.h"
#include "SoloBoundingBox.h"
#include "SoloMatrix.h"
#include "SoloFrustum.h"
#include "SoloException.h"

using namespace solo;


Ray::Ray():
	direction{0, 0, 1}
{
}


Ray::Ray(const Vector3& origin, const Vector3& direction)
{
	set(origin, direction);
}


Ray::Ray(float originX, float originY, float originZ, float dirX, float dirY, float dirZ)
{
	set(Vector3(originX, originY, originZ), Vector3(dirX, dirY, dirZ));
}


const Vector3& Ray::getOrigin() const
{
	return origin;
}


void Ray::setOrigin(const Vector3& origin)
{
	this->origin = origin;
}


void Ray::setOrigin(float x, float y, float z)
{
	origin.set(x, y, z);
}


const Vector3& Ray::getDirection() const
{
	return direction;
}


void Ray::setDirection(const Vector3& direction)
{
	this->direction = direction;
	normalize();
}


void Ray::setDirection(float x, float y, float z)
{
	direction.set(x, y, z);
	normalize();
}


float Ray::intersects(const BoundingSphere& sphere) const
{
	return sphere.intersects(*this);
}


float Ray::intersects(const BoundingBox& box) const
{
	return box.intersects(*this);
}


float Ray::intersects(const Frustum& frustum) const
{
	auto n = frustum.getNear();
	auto nD = intersects(n);
	auto nOD = n.getDistance(origin);

	auto f = frustum.getFar();
	auto fD = intersects(f);
	auto fOD = f.getDistance(origin);

	auto l = frustum.getLeft();
	auto lD = intersects(l);
	auto lOD = l.getDistance(origin);

	auto r = frustum.getRight();
	auto rD = intersects(r);
	auto rOD = r.getDistance(origin);

	auto b = frustum.getBottom();
	auto bD = intersects(b);
	auto bOD = b.getDistance(origin);

	auto t = frustum.getTop();
	auto tD = intersects(t);
	auto tOD = t.getDistance(origin);

	// If the ray's origin is in the negative half-space of one of the frustum's planes
	// and it does not intersect that same plane, then it does not intersect the frustum.
	if ((nOD < 0.0f && nD < 0.0f) || (fOD < 0.0f && fD < 0.0f) ||
		(lOD < 0.0f && lD < 0.0f) || (rOD < 0.0f && rD < 0.0f) ||
		(bOD < 0.0f && bD < 0.0f) || (tOD < 0.0f && tD < 0.0f))
	{
		return static_cast<float>(INTERSECTS_NONE);
	}

	// Otherwise, the intersection distance is the minimum positive intersection distance.
	auto d = (nD > 0.0f) ? nD : 0.0f;
	d = (fD > 0.0f) ? ((d == 0.0f) ? fD : std::min(fD, d)) : d;
	d = (lD > 0.0f) ? ((d == 0.0f) ? lD : std::min(lD, d)) : d;
	d = (rD > 0.0f) ? ((d == 0.0f) ? rD : std::min(rD, d)) : d;
	d = (tD > 0.0f) ? ((d == 0.0f) ? bD : std::min(bD, d)) : d;
	d = (bD > 0.0f) ? ((d == 0.0f) ? tD : std::min(tD, d)) : d;

	return d;
}


float Ray::intersects(const Plane& plane) const
{
	const auto& normal = plane.getNormal();
	// If the origin of the ray is on the plane then the distance is zero.
	auto alpha = (normal.dot(origin) + plane.getDistance());
	if (fabs(alpha) < MATH_EPSILON)
		return 0.0f;

	auto dot = normal.dot(direction);

	// If the dot product of the plane's normal and this ray's direction is zero,
	// then the ray is parallel to the plane and does not intersect it.
	if (dot == 0.0f)
		return static_cast<float>(INTERSECTS_NONE);

	// Calculate the distance along the ray's direction vector to the point where
	// the ray intersects the plane (if it is negative the plane is behind the ray).
	auto d = -alpha / dot;
	if (d < 0.0f)
		return static_cast<float>(INTERSECTS_NONE);
	
	return d;
}


void Ray::set(const Vector3& origin, const Vector3& direction)
{
	this->origin = origin;
	this->direction = direction;
	normalize();
}


void Ray::set(const Ray& ray)
{
	origin = ray.origin;
	direction = ray.direction;
	normalize();
}


void Ray::transform(const Matrix& matrix)
{
	matrix.transformPoint(&origin);
	matrix.transformDirection(&direction);
	direction.normalize();
}


void Ray::normalize()
{
	if (direction.isZero())
		THROW(EngineException, "Invalid ray object: the ray's direction must be non-zero");

	// Normalize the ray's direction vector
	auto normalizeFactor = 1.0f / sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	if (normalizeFactor != 1.0f)
	{
		direction.x *= normalizeFactor;
		direction.y *= normalizeFactor;
		direction.z *= normalizeFactor;
	}
}