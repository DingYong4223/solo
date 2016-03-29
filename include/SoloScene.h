#pragma once

#include "SoloBase.h"
#include <unordered_map>
#include <list>
#include <vector>


namespace solo
{
    class Component;
    class Node;
    class DeviceToken;

    class Scene final
    {
    public:
        explicit Scene(const DeviceToken&) {}
        ~Scene() {}
        SL_NONCOPYABLE(Scene)

        sptr<Node> createNode();

        void addComponent(uint32_t nodeId, sptr<Component> cmp);
        void removeComponent(uint32_t nodeId, uint32_t typeId);

        Component* getComponent(uint32_t nodeId, uint32_t typeId) const;
        Component* findComponent(uint32_t nodeId, uint32_t typeId) const;

        void update();
        void render();

    private:
        using NodeComponentMap = std::unordered_map<uint32_t, sptr<Component>>;
        using AllComponentMap = std::unordered_map<uint32_t, NodeComponentMap>;

        template <class T>
        void updateRenderQueue(std::list<T>& queue, bool ignoreCamera);
        
        void updateComponents();
        void rebuildComponentsToUpdate();

        uint32_t nodeCounter = 0;
        bool cameraCacheDirty = true;
        bool componentsDirty = true;

        std::list<Component*> cameraQueue;
        std::list<Component*> renderQueue;

        // TODO not cache-friendly
        AllComponentMap components;
        std::vector<sptr<Component>> componentsToUpdate;
    };
}
