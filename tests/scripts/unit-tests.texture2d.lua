runTest(function()
	assert(solo.TextureFilter_Linear ~= nil)
	assert(solo.TextureFilter_Nearest ~= nil)
	assert(solo.TextureFilter_LinearMipmapLinear ~= nil)
	assert(solo.TextureFilter_LinearMipmapNearest ~= nil)
	assert(solo.TextureFilter_NearestMipmapLinear ~= nil)
	assert(solo.TextureFilter_NearestMipmapNearest ~= nil)
	assert(solo.ColorFormat_RGB ~= nil)
	assert(solo.ColorFormat_RGBA ~= nil)
	assert(solo.TextureWrapMode_Clamp ~= nil)
	assert(solo.TextureWrapMode_Repeat ~= nil)

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
