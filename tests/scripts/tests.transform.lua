runTest(function()
	local v = solo.Vector3(1, 2, 3)
	local q = solo.Quaternion()
	local m = solo.Matrix()
	local scene = device:getScene()
	local t = scene:createNode():findComponent("Transform")
	local t2 = scene:createNode():findComponent("Transform")
	local cam = scene:createNode():addComponent("Camera")
	t:setParent(t2)
	assert(t:getParent())
	assert(t2:getChild(0))
	assert(t2:getChildrenCount())
	t2:removeChildren()
	assert(t:getWorldScale())
	assert(t:getLocalScale())
	assert(t:getWorldRotation())
	assert(t:getLocalRotation())
	assert(t:getWorldPosition())
	assert(t:getLocalPosition())
	assert(t:getLocalUp())
	assert(t:getLocalDown())
	assert(t:getLocalLeft())
	assert(t:getLocalRight())
	assert(t:getLocalForward())
	assert(t:getLocalBack())
	t:translateLocal(v)
	t:rotate(q, solo.TransformSpace.Self)
	t:rotateAxisAngle(v, 1, solo.TransformSpace.Self)
	t:scaleLocal(v)
	t:setLocalPosition(v)
	t:setLocalRotation(q)
	t:setLocalRotationAxisAngle(v, 1)
	t:setLocalScale(v)
	t:lookAt(v, v)
	assert(t:getMatrix())
	assert(t:getWorldMatrix())
	assert(t:getWorldViewMatrix(cam))
	assert(t:getWorldViewProjectionMatrix(cam))
	assert(t:getInverseTransposedWorldViewMatrix(cam))
	assert(t:getInverseTransposedWorldMatrix())
	t:transformPoint(v)
	t:transformDirection(v)
	assert(t:getTags())
	assert(t:getNode())
end, "Transform")