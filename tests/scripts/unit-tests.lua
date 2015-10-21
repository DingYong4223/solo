print("Running unit tests...")

function runTest(test, name)
	io.write("Running " .. name .. " tests... ")
	local _, err = select(1, pcall(test))
	if err then
		print("\n" .. err)
	else
		print("done")
	end
end

engine = solo.Engine.create(solo.EngineCreationArgs(EngineMode_Stub, 1, 1))

dofile("../tests/scripts/unit-tests.enums.lua")
dofile("../tests/scripts/unit-tests.vector2.lua")
dofile("../tests/scripts/unit-tests.vector3.lua")
dofile("../tests/scripts/unit-tests.vector4.lua")
dofile("../tests/scripts/unit-tests.plane.lua")
dofile("../tests/scripts/unit-tests.quaternion.lua")
dofile("../tests/scripts/unit-tests.bounding-box.lua")
dofile("../tests/scripts/unit-tests.bounding-sphere.lua")
dofile("../tests/scripts/unit-tests.matrix.lua")
dofile("../tests/scripts/unit-tests.frustum.lua")
dofile("../tests/scripts/unit-tests.node.lua")
dofile("../tests/scripts/unit-tests.transform.lua")
dofile("../tests/scripts/unit-tests.camera.lua")
dofile("../tests/scripts/unit-tests.render-target.lua")
dofile("../tests/scripts/unit-tests.material.lua")
dofile("../tests/scripts/unit-tests.effect.lua")
dofile("../tests/scripts/unit-tests.model-renderer.lua")
dofile("../tests/scripts/unit-tests.texture2d.lua")
dofile("../tests/scripts/unit-tests.mesh.lua")
dofile("../tests/scripts/unit-tests.model.lua")
dofile("../tests/scripts/unit-tests.resource-manager.lua")
dofile("../tests/scripts/unit-tests.engine.lua")
dofile("../tests/scripts/unit-tests.device.lua")

print("Finished unit tests\n")
