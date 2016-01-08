runTest(function()
	local mgr = device:getResourceManager()
	mgr:findEffect("wer")
	mgr:findTexture2D("wer")
	mgr:findCubeTexture("wer")
	mgr:findMaterial("wer")
	mgr:findMesh("wer")
	mgr:findRenderTarget("wer")
	local ef = mgr:getOrCreateEffect("wer", "wer", "uri")
	assert(ef)
	assert(mgr:getOrCreateTexture2D("sdf"))
	assert(mgr:getOrCreateCubeTexture("sdf"))
	assert(mgr:getOrCreateMaterial(ef, "werwer"))
	assert(mgr:getOrCreateMesh(solo.VertexFormat({}), "qwe"))
	assert(mgr:getOrCreatePrefabMesh(solo.MeshPrefab.Quad, "qwe"))
	assert(mgr:getOrCreateRenderTarget("rt"))
	mgr:getOrLoadTexture2D("wer")
	mgr:getOrLoadTexture2D("wer", "override")
	mgr:getOrLoadCubeTexture({ "1", "2", "3", "4", "5", "6" })
	mgr:getOrLoadCubeTexture({ "1", "2", "3", "4", "5", "6" }, "override")
	mgr:getOrLoadMesh("wer")
	mgr:getOrLoadMesh("wer", "override")
end, "ResourceManager")
