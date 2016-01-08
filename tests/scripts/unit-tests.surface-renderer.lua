runTest(function()
	local rmgr = device:getResourceManager()
	local e = rmgr:getOrCreateEffect("vs", "fs")
	local m = rmgr:getOrCreateMaterial(e, "test/uri")
	local rt = rmgr:getOrCreateRenderTarget("test")
	local renderer = rmgr:getOrCreateSurfaceRenderer(m)
	renderer:renderSurface(rt)
	renderer:renderSurface(nil)
	renderer:renderSurface()
end, "SurfaceRenderer")
