local v = solo.Vector3(1, 2, 3)
local s = solo.BoundingSphere(v, 1)
local bb = solo.BoundingBox(v, v)

assert(s.center)
assert(s.radius)

assert(s:intersectsBoundingSphere(s) ~= nil)
assert(s:intersectsBoundingBox(bb) ~= nil)
assert(s:intersectsFrustum(solo.Frustum()) ~= nil)

assert(s:hitByRay(solo.Ray(v, v)) ~= nil)

assert(s:intersectPlane(solo.Plane(v, 1)) ~= nil)

assert(s:isEmpty() ~= nil)

s:mergeBoundingBox(bb)
s:mergeBoundingSphere(s)