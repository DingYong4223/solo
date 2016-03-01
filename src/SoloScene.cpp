
#include "SoloScene.h"
#include "SoloComponent.h"
#include "SoloRenderContext.h"
#include "SoloCamera.h"
#include "SoloNode.h"
#include <algorithm>

using namespace solo;


Scene::Scene(Device* device) :
    device(device)
{
}


Scene::~Scene()
{
}


shared<Node> Scene::createNode()
{
    auto node = SL_NEW_SHARED(Node, this, nodeCounter++);
    node->addComponent<Transform>();
    return node;
}


void Scene::addComponent(int nodeId, shared<Component> cmp)
{
    auto typeId = cmp->getTypeId();
    auto nodeIt = components.find(nodeId);
    if (nodeIt != components.end())
    {
        auto& nodeComponents = nodeIt->second;
        if (nodeComponents.find(typeId) != nodeComponents.end())
            SL_THROW_FMT(InvalidOperationException, "Component ", typeId, " already exists");
    }

    components[nodeId][typeId] = cmp;
    cmp->init();

    componentsDirty = true;
    if (typeId == Camera::getId())
        cameraCacheDirty = true;
}


void Scene::removeComponent(int nodeId, int typeId)
{
    auto nodeIt = components.find(nodeId);
    if (nodeIt == components.end())
        return;
    auto cmpIt = nodeIt->second.find(typeId);
    if (cmpIt == nodeIt->second.end())
        return;
    
    auto cmp = cmpIt->second;
    nodeIt->second.erase(cmpIt);
    if (nodeIt->second.empty())
        components.erase(nodeIt);

    cmp->terminate();

    if (typeId == Camera::getId())
        cameraCacheDirty = true;
    componentsDirty = true;
}


Component* Scene::getComponent(int nodeId, int typeId) const
{
    auto cmp = findComponent(nodeId, typeId);
    if (!cmp)
        SL_THROW_FMT(InvalidOperationException, "Component ", typeId, " not found on node ", nodeId);
    return cmp;
}


Component* Scene::findComponent(int nodeId, int typeId) const
{
    auto nodeIt = components.find(nodeId);
    if (nodeIt == components.end())
        return nullptr;

    auto& nodeComponents = nodeIt->second;
    auto cmpIt = nodeComponents.find(typeId);
    if (cmpIt != nodeComponents.end())
        return cmpIt->second.get();

    return nullptr;
}


template <class T>
void Scene::updateRenderQueue(std::list<T>& queue, int cmpTypeIdFilter)
{
    queue.clear();

    for (auto& nodeComponents : components)
    {
        auto nodeId = nodeComponents.first;
        for (auto& pair : nodeComponents.second)
        {
            auto component = pair.second.get();
            if (cmpTypeIdFilter > 0 && component->getTypeId() != cmpTypeIdFilter)
                continue;

            if (component->getRenderQueue() == KnownRenderQueues::NotRendered)
                continue;

            auto transform = Node::findComponent<Transform>(this, nodeId);
            if (!transform) // TODO save this transform for later use
                continue;

            for (auto it = queue.begin();; ++it)
            {
                if (it == queue.end() || component->getRenderQueue() < (*it)->getRenderQueue())
                {
                    queue.insert(it, component);
                    break;
                }
            }
        }
    }
}


void Scene::update()
{
    if (componentsDirty)
    {
        rebuildComponentsToUpdate();
        componentsDirty = false;
    }

    updateComponents();
}


void Scene::updateComponents()
{
    // TODO A component added during one of these update() methods
    // will be updated for the first time only on the next call to updateComponents(). That is intended.
    // TODO It's possible that a component can receive update() after it has been removed.
    // For example, a component can remove other component during update(), but the component
    // being removed sits later in this list.
    for (auto cmp : componentsToUpdate)
        cmp->update();
}


void Scene::rebuildComponentsToUpdate()
{
    componentsToUpdate.clear(); // TODO maybe overkill
    for (auto& nodeIt : components)
    {
        for (auto& cmpIt : nodeIt.second)
            componentsToUpdate.push_back(cmpIt.second);
    }
}


bool tagsAreRenderable(const BitFlags& objectTags, const BitFlags& cameraTags)
{
    auto rawObjectTags = objectTags.getRaw();
    auto rawCameraTags = cameraTags.getRaw();
    return (rawObjectTags & rawCameraTags) != 0;
}


void Scene::render()
{
    updateRenderQueue(cameraQueue, Camera::getId());
    updateRenderQueue(renderQueue, 0);

    for (auto cam : cameraQueue)
    {
        auto camera = dynamic_cast<Camera*>(cam);

        camera->apply();

        RenderContext context;
        context.scene = this;
        context.camera = camera;

        auto renderTags = camera->getRenderTags();

        for (auto cmp : renderQueue)
        {
            if (tagsAreRenderable(cmp->getTags(), renderTags))
            {
                auto transform = Node::findComponent<Transform>(this, cmp->getNode().getId()); // TODO can be cached
                context.nodeTransform = transform;
                cmp->render(context);
            }
        }

        camera->finish();

        for (auto& pair : components.at(camera->getNode().getId()))
            pair.second->onAfterCameraRender();
    }
}

