#include "SoloSceneNode.h"
#include "SoloException.h"
#include "SoloBaseInternal.h"

using namespace solo;


SceneNode::SceneNode(const std::string &name)
	: _name(name)
{
}


void SceneNode::addComponent(ptr<IComponent> cmp)
{
	if (findComponent(cmp->id()))
		throw EngineException("Component with same id already exists");
	_components[cmp->id()] = cmp;
	_newComponents.push_back(cmp);
}


ptr<IComponent> SceneNode::getComponent(const std::string &id)
{
	auto cmp = findComponent(id);
	if (!cmp)
		throw EngineException("Component not found");
	return cmp;
}


ptr<IComponent> SceneNode::findComponent(const std::string &id)
{
	auto it = _components.find(id);
	return it != _components.end() ? it->second : nullptr;
}


void SceneNode::update()
{
	if (_newComponents.size() > 0)
	{
		for (auto cmp : _newComponents)
			cmp->start();
		_newComponents.clear();
	}

	for (auto cmp: _components)
		cmp.second->update();
}
