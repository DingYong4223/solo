local demos = {
	{ "Demo 1", "../tests/scripts/demos/demo1.lua" },
	{ "Demo 2", "../tests/scripts/demos/demo2.lua" },
}

function runDemo(path)
	local args = solo.DeviceCreationArgs(solo.DeviceMode.OpenGL, 1366, 700, false, "Solo Tester")
	args.logFilePath = "demo.log"
	local device = solo.Device.create(args)
	local demo = dofile(path)

	local _, err = pcall(function()
		demo:init(device)
	end)
	if err then
		device:getLogger():logCritical(err)
	end

	device:run()
	device:reset()

	demo = nil
	device = nil
	collectgarbage()
end

function prompt()
	io.write("\nAvailable demos:\n")
	for i, info in ipairs(demos) do
		print(i .. ": " .. info[1])
	end

	io.write("Demo to run: ")
	local number = tonumber(io.read())
	local demoInfo = demos[number]
	if demoInfo then
		runDemo(demoInfo[2])
		return true
	end
	return false
end

while prompt() do
end