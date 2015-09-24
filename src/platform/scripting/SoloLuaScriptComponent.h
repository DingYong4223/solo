#pragma once

#include <functional>
#define LUAINTF_LINK_LUA_COMPILED_IN_CXX 0
#include <LuaIntf.h>
#include "SoloComponent.h"

namespace solo
{
	class LuaScriptComponent: public ComponentBase<LuaScriptComponent>
	{
	public:
		explicit LuaScriptComponent(const Node& node, size_t typeId,
			std::function<void()> initFunc,
			std::function<void()> updateFunc,
			std::function<void()> renderFunc,
			std::function<void()> postRenderFunc,
			std::function<void()> terminateFunc);

		virtual void init() override;
		virtual void update() override;
		virtual void render(RenderContext& context) override;
		virtual void postRender() override;
		virtual void terminate() override;

		virtual size_t getTypeId() override;

		static void addComponent(Node *node, LuaIntf::LuaRef& component);
		static void removeComponent(Node *node, const std::string& componentTypeId);

	private:
		size_t typeId;
		std::function<void()> initFunc;
		std::function<void()> updateFunc;
		std::function<void()> renderFunc;
		std::function<void()> postRenderFunc;
		std::function<void()> terminateFunc;
	};
}