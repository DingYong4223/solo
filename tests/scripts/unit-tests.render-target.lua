runTest(function()
	local rt = device:getResourceManager():getOrCreateRenderTarget("test/uri")
	local tex = device:getResourceManager():getOrCreateTexture2D("nonesense")
	rt:setTextures({ tex, tex })
	assert(rt:getTextureCount() == 2)
	rt:getTexture(0)
end, "RenderTarget")
