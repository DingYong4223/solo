runTest(function()
	assert(solo.KeyCode_A ~= nil)
	assert(solo.KeyCode_B ~= nil)
	assert(solo.KeyCode_C ~= nil)
	assert(solo.KeyCode_D ~= nil)
	assert(solo.KeyCode_E ~= nil)
	assert(solo.KeyCode_F ~= nil)
	assert(solo.KeyCode_G ~= nil)
	assert(solo.KeyCode_H ~= nil)
	assert(solo.KeyCode_I ~= nil)
	assert(solo.KeyCode_J ~= nil)
	assert(solo.KeyCode_K ~= nil)
	assert(solo.KeyCode_L ~= nil)
	assert(solo.KeyCode_M ~= nil)
	assert(solo.KeyCode_N ~= nil)
	assert(solo.KeyCode_O ~= nil)
	assert(solo.KeyCode_P ~= nil)
	assert(solo.KeyCode_Q ~= nil)
	assert(solo.KeyCode_R ~= nil)
	assert(solo.KeyCode_S ~= nil)
	assert(solo.KeyCode_T ~= nil)
	assert(solo.KeyCode_U ~= nil)
	assert(solo.KeyCode_V ~= nil)
	assert(solo.KeyCode_W ~= nil)
	assert(solo.KeyCode_X ~= nil)
	assert(solo.KeyCode_Y ~= nil)
	assert(solo.KeyCode_Z ~= nil)
	assert(solo.KeyCode_LeftArrow ~= nil)
	assert(solo.KeyCode_RightArrow ~= nil)
	assert(solo.KeyCode_UpArrow ~= nil)
	assert(solo.KeyCode_DownArrow ~= nil)
	assert(solo.KeyCode_Escape ~= nil)

	assert(solo.MouseButton_Left ~= nil)
	assert(solo.MouseButton_Middle ~= nil)
	assert(solo.MouseButton_Right ~= nil)

	assert(solo.EngineMode_Stub ~= nil)
	assert(solo.EngineMode_OpenGL ~= nil)

	assert(solo.PolygonFace_All ~= nil)
	assert(solo.PolygonFace_CCW ~= nil)
	assert(solo.PolygonFace_CW ~= nil)

	assert(solo.AutoBinding_CameraWorldPosition ~= nil)
	assert(solo.AutoBinding_InverseTransposedWorldMatrix ~= nil)
	assert(solo.AutoBinding_InverseTransposedWorldViewMatrix ~= nil)
	assert(solo.AutoBinding_ProjectionMatrix ~= nil)
	assert(solo.AutoBinding_ViewMatrix ~= nil)
	assert(solo.AutoBinding_ViewProjectionMatrix ~= nil)
	assert(solo.AutoBinding_WorldMatrix ~= nil)
	assert(solo.AutoBinding_WorldViewMatrix ~= nil)
	assert(solo.AutoBinding_WorldViewProjectionMatrix ~= nil)
	assert(solo.AutoBinding_None ~= nil)

	assert(solo.TransformSpace_Self ~= nil)
	assert(solo.TransformSpace_Parent ~= nil)
	assert(solo.TransformSpace_World ~= nil)

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

	assert(solo.Plane.Intersection_Front ~= nil)
	assert(solo.Plane.Intersection_Back ~= nil)
	assert(solo.Plane.Intersection_Intersecting ~= nil)
end, "Enums")
