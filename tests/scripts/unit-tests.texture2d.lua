runTest(function()
	local mgr = engine:getResourceManager()
	local t = mgr:getOrCreateTexture2D()
	t:setData(solo.ColorFormat_RGB, {1, 2, 3, 4}, 2, 2)
	t:generateMipmaps()
	assert(t:getSize() ~= nil)
	assert(t:getVerticalWrapMode() ~= nil)
	assert(t:getHorizontalWrapMode() ~= nil)
	assert(t:getMinFilter() ~= nil)
	assert(t:getMagFilter() ~= nil)
	assert(t:getAnisotropyLevel() ~= nil)
	t:setVerticalWrapMode(solo.TextureWrapMode_Clamp)
	t:setHorizontalWrapMode(solo.TextureWrapMode_Clamp)
	t:setMinFilter(solo.TextureFilter_Linear)
	t:setMagFilter(solo.TextureFilter_Linear)
	t:setAnisotropyLevel(1)
end, "Texture2D")
