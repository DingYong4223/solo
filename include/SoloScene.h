/*
    Copyright (c) Aleksey Fedotov

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
        misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include "SoloCommon.h"
#include <unordered_map>
#include <list>
#include <vector>
#include <functional>


namespace solo
{
    class Device;
    class Component;
    class Node;

    class Scene final
    {
    public:
        SL_DISABLE_COPY_AND_MOVE(Scene)

        explicit Scene(Device *device, const FriendToken<Device> &);
        ~Scene() {}

        auto getDevice() const -> Device*;

        auto createNode() -> sptr<Node>;

        auto findComponent(uint32_t nodeId, uint32_t typeId) const -> Component*;
        void addComponent(uint32_t nodeId, sptr<Component> cmp);
        void removeComponent(uint32_t nodeId, uint32_t typeId);

        void update();
        void render();

    private:
        using NodeComponents = std::unordered_map<uint32_t, sptr<Component>>;
        using NodesWithComponents = std::unordered_map<uint32_t, NodeComponents>;

        void rebuildRenderQueue(std::list<Component *> &queue, std::function<bool(Component *)> ignoreComponent);

        void updateComponents();
        void rebuildComponentsToUpdate();

        Device *device = nullptr;

        uint32_t nodeCounter = 0;
        bool cameraCacheDirty = true;
        bool componentsDirty = true;

        std::list<Component *> cameraQueue;
        std::list<Component *> renderQueue;

        // TODO not cache-friendly
        NodesWithComponents nodes;
        std::vector<sptr<Component>> componentsToUpdate;
    };

    inline auto Scene::getDevice() const -> Device *
    {
        return device;
    }
}
