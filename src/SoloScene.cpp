#include "SoloScene.h"
#include "SoloComponent.h"
#include "SoloException.h"
#include "SoloTransform.h"
#include "SoloCamera.h"
#include "SoloModelRenderer.h"

using namespace solo;


Scene::Scene():
	_nodeCounter(0),
	_primaryCamera(nullptr)
{
}


ptr<Scene> Scene::create()
{
	return NEW2(Scene);
}


size_t Scene::createEmptyNode()
{
	_nodeComponents[++_nodeCounter];
	return _nodeCounter;
}


size_t Scene::createNode()
{
	auto node = createEmptyNode();
	auto transform = Transform::create();
	this->Scene::addComponent(node, transform);
	return node;
}


bool Scene::nodeExists(size_t node)
{
	return _nodeComponents.find(node) != _nodeComponents.end();
}


ptr<Camera> Scene::addCamera(size_t node)
{
	auto camera = Camera::create();
	addComponent(node, camera);
	if (!_primaryCamera)
	{
		_primaryCamera = camera;
		_primaryCamera->setPrimary(true);
	}
	return camera;
}


ptr<ModelRenderer> Scene::addModelRenderer(size_t node)
{
	auto renderer = ModelRenderer::create();
	addComponent(node, renderer);
	return renderer;
}


void Scene::addComponent(size_t node, ptr<Component> cmp)
{
	ensureNodeExists(node);
	if (findComponent(node, cmp->getComponentTypeId()))
		THROW(EngineException, "Component ", cmp->getComponentTypeId(), " already exists");
	_nodeComponents[node][cmp->getComponentTypeId()] = cmp;
}


ptr<Component> Scene::getComponent(size_t node, size_t typeId)
{
	auto cmp = findComponent(node, typeId);
	if (!cmp)
		THROW(EngineException, "Component ", typeId, " not found");
	return cmp;
}


ptr<Component> Scene::findComponent(size_t node, size_t typeId)
{
	ensureNodeExists(node);
	auto nodeComponents = _nodeComponents[node];
	auto it = nodeComponents.find(typeId);
	return it != nodeComponents.end() ? it->second : nullptr;
}


void Scene::update()
{
	for (auto node : _nodeComponents)
	{
		for (auto component : node.second)
			component.second->update();
	}
}


void Scene::render()
{
	if (_primaryCamera)
		_primaryCamera->render();
	for (auto node : _nodeComponents)
	{
		for (auto component : node.second)
		{
			if (component.second != _primaryCamera)
				component.second->render();
		}
	}
}


void Scene::ensureNodeExists(size_t node)
{
	if (!nodeExists(node))
		THROW(EngineException, "Node ", node, " not found");
}
