--
-- Copyright (c) Aleksey Fedotov
-- MIT license
-- 

require "Common"

return function(scene, tag)
    local getImagePath = function(fileName) return getAssetPath("textures/skyboxes/deep-space/") .. fileName end
    local tex = sl.CubeTexture.loadFromFaceFiles(sl.device,
        getImagePath("Front.png"), getImagePath("Back.png"), getImagePath("Left.png"),
        getImagePath("Right.png"), getImagePath("Top.png"), getImagePath("Bottom.png")
    )
    tex:setWrapping(sl.TextureWrapping.Clamp)
    tex:setFiltering(sl.TextureFiltering.Linear)
    local node = scene:createNode()
    local renderer = node:addComponent("SkyboxRenderer")
    renderer:setTexture(tex)
    renderer:setTag(tag)
end