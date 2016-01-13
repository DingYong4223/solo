#pragma once

#include "SoloTypeId.h"
#include "SoloNode.h"
#include "SoloBitFlags.h"
#include "SoloRenderQueue.h"

namespace solo
{
    struct RenderContext;

    class Component
    {
    public:
        SL_NONCOPYABLE(Component);
        virtual ~Component() {}

        virtual size_t getTypeId() = 0;

        virtual void init() {}
        virtual void update() {}
        virtual void terminate() {}
        virtual void render(RenderContext& context) {}
        virtual void onAfterCameraRender() {}

        virtual int getRenderQueue() const;
        virtual void setRenderQueue(int queue);

        Node getNode() const;

        BitFlags& getTags();

    protected:
        explicit Component(const Node& node) : node(node)
        {
            tags.set(1);
        }

        Node node;
        BitFlags tags;
        unsigned renderQueue = KnownRenderQueues::NotRendered;
    };

    inline int Component::getRenderQueue() const
    {
        return renderQueue;
    }

    inline void Component::setRenderQueue(int queue)
    {
        renderQueue = queue;
    }

    inline Node Component::getNode() const
    {
        return node;
    }

    inline BitFlags& Component::getTags()
    {
        return tags;
    }


    template <class T>
    class ComponentBase: public Component
    {
    public:
        explicit ComponentBase(const Node& node): Component(node) {}

        static size_t getId();
        virtual size_t getTypeId() override;
    };

    template <class T>
    inline size_t ComponentBase<T>::getId()
    {
        return TypeId::get<T>();
    }

    template <class T>
    inline size_t ComponentBase<T>::getTypeId()
    {
        return getId();
    }
}
