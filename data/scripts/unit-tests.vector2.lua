runTest(function()
	local v1 = solo.Vector2(1, 2)
	local v2 = solo.Vector2(2, 3)
	solo.Vector2.zero()
	solo.Vector2.unit()
	solo.Vector2.unitX()
	solo.Vector2.unitY()
	solo.Vector2.angle(v1, v2)
	solo.Vector2.dot(v1, v2)
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
	v1:set(1, 2)
	v1:assign(solo.Vector2(1, 2))
	v1 = v1:plusScalar(1):addScalar(2):plusVector2(solo.Vector2.unit()):addVector2(solo.Vector2.unit())
	print(v1.x .. v1.y)
end, "Vector2")
