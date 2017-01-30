-- TODO avoid using globals in functions

dev = solo.device
loader = dev:getAssetLoader()
physics = dev:getPhysics()
renderer = dev:getRenderer()
logger = dev:getLogger()

function initMainCamera()
    local node = scene:createNode()
        
    local t = node:findComponent("Transform")
    t:setLocalPosition(solo.Vector3(10, 10, 10))
    t:lookAt(solo.Vector3(0, 0, 0), solo.Vector3(0, 1, 0))
    
    local cam = node:addComponent("Camera")
    cam:setClearColor(solo.Vector4(0.0, 0.6, 0.6, 1.0))
    cam:setNear(0.05)

    return cam
end

mainCamera = initMainCamera()

function keepRunning()
    return not dev:isQuitRequested() and
           not dev:isWindowCloseRequested() and
           not dev:isKeyPressed(solo.KeyCode.Escape, true)
end

function update()
    scene:visit(function(cmp) cmp:update() end)

    local lifetime = dev:getLifetime()
        
    local color = camera:getClearColor()
    color.x = math.abs(math.sin(lifetime))

    camera:setClearColor(color)
end

function renderByTags(tags, ctx)
    scene:visitByTags(tags, function(cmp) cmp:render(ctx) end)
end

function render()
    mainCamera:renderFrame(function(ctx) end)
end

function run()
    while keepRunning() do
        dev:update(function()
            loader:update()
                loader:update()
                physics:update()
                renderer:renderFrame(function()
                    update()
                    render()
                end)
        end)
    end
end

function runSafe()
    local _, err = select(1, pcall(run))
    if err then
        logger:logError(err)
    end
end

runSafe()