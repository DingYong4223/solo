/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "SoloComponent.h"
#include <LuaIntf.h>
#include <functional>

namespace solo
{
    namespace lua
    {
        class LuaScriptComponent final : public ComponentBase<LuaScriptComponent>
        {
        public:
            static const u32 MinComponentTypeId = 1000000000; // Assume that built-in components don't ever exceed this limit

            LuaScriptComponent(const Node& node, LuaIntf::LuaRef scriptComponent);

            void init() override final;
            void terminate() override final;
            void update() override final;
            void render() override final;

            void onComponentAdded(Component *cmp) override final;
            void onComponentRemoved(Component *cmp) override final;

            auto getTypeId() -> u32 override final;

            auto getRef() const -> LuaIntf::LuaRef;

        private:
            u32 typeId;
            LuaIntf::LuaRef ref;

            std::function<void(LuaIntf::LuaRef)> initFunc;
            std::function<void(LuaIntf::LuaRef)> terminateFunc;
            std::function<void(LuaIntf::LuaRef)> updateFunc;
            std::function<void(LuaIntf::LuaRef)> renderFunc;
            std::function<void(LuaIntf::LuaRef, Component*)> onComponentAddedFunc;
            std::function<void(LuaIntf::LuaRef, Component*)> onComponentRemovedFunc;
        };

        inline auto LuaScriptComponent::getTypeId() -> u32
        {
            return typeId;
        }

        inline auto LuaScriptComponent::getRef() const -> LuaIntf::LuaRef
        {
            return ref;
        }
    }
}