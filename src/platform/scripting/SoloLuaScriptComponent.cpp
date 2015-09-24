#include "SoloLuaScriptComponent.h"
#include "SoloTransform.h"

using namespace solo;
using namespace LuaIntf;


LuaScriptComponent::LuaScriptComponent(
	const Node& node,
	size_t typeId,
	LuaRef& component,
	std::function<void(LuaRef)> initFunc,
	std::function<void(LuaRef)> updateFunc,
	std::function<void(LuaRef)> renderFunc,
	std::function<void(LuaRef)> postRenderFunc,
	std::function<void(LuaRef)> terminateFunc) :
	ComponentBase<LuaScriptComponent>(node),
	typeId(typeId),
	component(component),
	initFunc(initFunc),
	updateFunc(updateFunc),
	renderFunc(renderFunc),
	postRenderFunc(postRenderFunc),
	terminateFunc(terminateFunc)
{
	component.set("node", node);
}


void LuaScriptComponent::init()
{
	initFunc(component);
}


void LuaScriptComponent::update()
{
	updateFunc(component);
}


void LuaScriptComponent::render(RenderContext& context)
{
	renderFunc(component/* TODO pass context */);
}


void LuaScriptComponent::postRender()
{
	postRenderFunc(component);
}


void LuaScriptComponent::terminate()
{
	terminateFunc(component);
}


size_t LuaScriptComponent::getTypeId()
{
	return typeId;
}


Component* LuaScriptComponent::findBuiltInComponent(Node* node, const std::string& typeName)
{
	if (typeName == "Transform")
		return node->findComponent<Transform>();
	return nullptr;
}


std::function<LuaRef(Node*, const std::string&)> LuaScriptComponent::getFindComponentFunc(lua_State* lua)
{
	return std::bind(&findComponent, lua, std::placeholders::_1, std::placeholders::_2);
}


LuaRef LuaScriptComponent::findComponent(lua_State *lua, Node* node, const std::string& componentTypeId)
{
	auto typeId = getHash(componentTypeId);
	auto component = node->getScene()->findComponent(node->getId(), typeId);
	auto scriptComponent = dynamic_cast<LuaScriptComponent*>(component);
	return scriptComponent ? scriptComponent->component : LuaRef(lua, nullptr);
}


void LuaScriptComponent::addComponent(Node* node, LuaRef& component)
{
	auto typeIdString = component.get<std::string>("typeId");
	auto typeId = getHash(typeIdString);
	auto initFunc = component.has("init") ? component.get<std::function<void(LuaRef)>>("init") : [](LuaRef) {};
	auto updateFunc = component.has("update") ? component.get<std::function<void(LuaRef)>>("update") : [](LuaRef) {};
	auto renderFunc = component.has("render") ? component.get<std::function<void(LuaRef)>>("render") : [](LuaRef) {};
	auto postRenderFunc = component.has("postRender") ? component.get<std::function<void(LuaRef)>>("postRender") : [](LuaRef) {};
	auto terminateFunc = component.has("terminate") ? component.get<std::function<void(LuaRef)>>("terminate") : [](LuaRef) {};
	auto actualComponent = NEW<LuaScriptComponent>(*node, typeId, component, initFunc, updateFunc, renderFunc, postRenderFunc, terminateFunc);
	node->getScene()->addComponent(node->getId(), actualComponent, typeId);
}


void LuaScriptComponent::removeComponent(Node* node, const std::string& componentTypeId)
{
	auto typeId = getHash(componentTypeId);
	node->getScene()->removeComponent(node->getId(), typeId);
}
