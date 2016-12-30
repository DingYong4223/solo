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

#include "SoloNode.h"


namespace solo
{
    struct RenderContext;

    struct ComponentTypeId
    {
        template <typename T>
        static auto get() -> uint32_t
        {
            static const auto id = counter++;
            return id;
        }

    private:
        static uint32_t counter;
    };


    class Component
    {
    public:
        SL_DISABLE_COPY_AND_MOVE(Component)

        virtual ~Component() {}

        virtual auto getTypeId() -> uint32_t = 0;

        // Called when the component is added to the scene
        virtual void init() {}
        // Called when the component is removed from the scene
        virtual void terminate() {}
        
        virtual void update() {}
        virtual void render(const RenderContext &context) {}

        virtual void onAfterCameraRender() {}
        virtual void onComponentAdded(Component *cmp) {}
        virtual void onComponentRemoved(Component *cmp) {}

        auto getNode() const -> Node;

        auto getTags() const -> uint32_t;
        void setTags(uint32_t tags);

    protected:
        explicit Component(const Node &node): node(node)
        {
        }

        Node node;
        uint32_t tags = 1;
    };

    inline auto Component::getNode() const -> Node
    {
        return node;
    }

    inline auto Component::getTags() const -> uint32_t
    {
        return tags;
    }

    inline void Component::setTags(uint32_t tags)
    {
        this->tags = tags;
    }


    template <class T>
    class ComponentBase: public Component
    {
    public:
        static auto getId() -> uint32_t;

        explicit ComponentBase(const Node &node): Component(node) {}

        auto getTypeId() -> uint32_t override;
    };

    template <class T>
    auto ComponentBase<T>::getId() -> uint32_t
    {
        return ComponentTypeId::get<T>();
    }

    template <class T>
    auto ComponentBase<T>::getTypeId() -> uint32_t
    {
        return getId();
    }
}
