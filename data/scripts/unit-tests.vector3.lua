runTest(function()
	local v1 = solo.Vector3(1, 2, 3)
	local v2 = solo.Vector3(2, 3, 4)
	solo.Vector3.zero()
	solo.Vector3.unit()
	solo.Vector3.unitX()
	solo.Vector3.unitY()
	solo.Vector3.unitZ()
	solo.Vector3.angle(v1, v2)
	solo.Vector3.cross(v1, v2)
	solo.Vector3.dot(v1, v2)
	v1:dot(v2)
	v1:distance(v2)
	v1:distanceSquared(v2)
	v1:clamp(v1, v2)
	v1:length()
	v1:lengthSquared()
	v1:isUnit()
	v1:isZero()
	v1:normalize()
	v1:normalized()
	v1:set(1, 2, 3)
	v1:assign(solo.Vector3(1, 2, 3))
	v1 = v1:plusScalar(1):addScalar(2):plusVector3(solo.Vector3.unit()):addVector3(solo.Vector3.unit())
	print(v1.x .. v1.y .. v1.z)
end, "Vector3")
