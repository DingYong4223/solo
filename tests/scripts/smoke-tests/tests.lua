local args = solo.DeviceCreationArgs()
args.mode = solo.DeviceMode.Stub
args.canvasWidth = 1
args.canvasHeight = 1
args.logFilePath = "lua-tests.log"
device = solo.Device.create(args)
logger = device:getLogger()
logger:logInfo("Running Lua tests...")

function runTest(test, name)
	local _, err = select(1, pcall(test))
	if err then
		logger:logError(name .. ": failed\n" .. err)
	else
		logger:logInfo(name .. ": success")
	end
end

dofile("../tests/scripts/smoke-tests/bit-flags.lua")
dofile("../tests/scripts/smoke-tests/bounding-box.lua")
dofile("../tests/scripts/smoke-tests/bounding-sphere.lua")
dofile("../tests/scripts/smoke-tests/camera.lua")
dofile("../tests/scripts/smoke-tests/component.lua")
dofile("../tests/scripts/smoke-tests/cube-texture.lua")
dofile("../tests/scripts/smoke-tests/device.lua")
dofile("../tests/scripts/smoke-tests/effect.lua")
dofile("../tests/scripts/smoke-tests/enums.lua")
dofile("../tests/scripts/smoke-tests/file-system.lua")
dofile("../tests/scripts/smoke-tests/frame-buffer.lua")
dofile("../tests/scripts/smoke-tests/frustum.lua")
dofile("../tests/scripts/smoke-tests/graphics.lua")
dofile("../tests/scripts/smoke-tests/logger.lua")
dofile("../tests/scripts/smoke-tests/material.lua")
dofile("../tests/scripts/smoke-tests/matrix.lua")
dofile("../tests/scripts/smoke-tests/mesh-renderer.lua")
dofile("../tests/scripts/smoke-tests/mesh.lua")
dofile("../tests/scripts/smoke-tests/node.lua")
dofile("../tests/scripts/smoke-tests/plane.lua")
dofile("../tests/scripts/smoke-tests/quaternion.lua")
dofile("../tests/scripts/smoke-tests/ray.lua")
dofile("../tests/scripts/smoke-tests/resource-manager.lua")
dofile("../tests/scripts/smoke-tests/scene.lua")
dofile("../tests/scripts/smoke-tests/skybox-renderer.lua")
dofile("../tests/scripts/smoke-tests/spectator.lua")
dofile("../tests/scripts/smoke-tests/texture2d.lua")
dofile("../tests/scripts/smoke-tests/transform.lua")
dofile("../tests/scripts/smoke-tests/vector2.lua")
dofile("../tests/scripts/smoke-tests/vector3.lua")
dofile("../tests/scripts/smoke-tests/vector4.lua")
dofile("../tests/scripts/smoke-tests/vertex-buffer-layout.lua")

logger:logInfo("Finished tests")
