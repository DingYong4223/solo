runTest(function()
	local mgr = engine:getResourceManager()
	local e = mgr:getOrCreateEffect("vs", "fs")
	local m = mgr:getOrCreateMaterial(e, "test/uri")
	local mp = m:getParameter("param")
	assert(mp)

	local v2 = solo.Vector2.unit()
	local v3 = solo.Vector3.unit()
	local v4 = solo.Vector4.unit()
	local m = solo.Matrix()
	local tex = mgr:getOrCreateTexture2D()
	mp:setFloat(1)
	mp:setFloatArray({1, 2, 3})
	mp:setInt(1)
	mp:setIntArray({1, 2, 3})
	mp:setVector2(v2)
	mp:setVector2Array({v2})
	mp:setVector3(v3)
	mp:setVector3Array({v3})
	mp:setVector4(v4)
	mp:setVector4Array({v4})
	mp:setMatrix(m)
	mp:setMatrixArray({m})
	mp:setTexture(tex)
	mp:setTextureArray({tex})
	mp:bindValue(solo.AutoBinding_WorldMatrix)

	local var = e:findVariable("wer")
	assert(var)
	var:setFloat(1)
	var:setFloatArray({1, 2, 3}, 3)
end, "MaterialParameter")
