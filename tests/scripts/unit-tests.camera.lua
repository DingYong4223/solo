runTest(function()
	local v = solo.Vector3(1, 2, 3)
	local q = solo.Quaternion()
	local scene = engine:getScene()
	local c = scene:createNode():addComponent("Camera")
	local rt = engine:getResourceManager():getOrCreateRenderTarget("abcd")

	c:setClearColor(1, 2, 3, 4)

	assert(c:getViewport())
	c:setViewport(1, 2, 3, 4)
	c:resetViewport()

	assert(c:isPerspective() ~= nil)
	c:setPerspective(true)

	assert(c:getNear() ~= nil)
	c:setNear(1)

	assert(c:getFar() ~= nil)
	c:setFar(1)

	assert(c:getFOV() ~= nil)
	c:setFOV(1)

	assert(c:getWidth() ~= nil)
	c:setWidth(1)

	assert(c:getHeight() ~= nil)
	c:setHeight(1)

	assert(c:getAspectRatio() ~= nil)
	c:setAspectRatio(1)

	c:getRenderTarget()
	c:setRenderTarget(rt)

	assert(c:getRenderMode() ~= nil)
	c:setRenderMode(solo.CameraRenderMode_Deferred)
end, "Camera")
