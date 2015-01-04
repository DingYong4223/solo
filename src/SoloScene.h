#ifndef __SOLO_SCENE_H__
#define __SOLO_SCENE_H__

#include "SoloBaseInternal.h"
#include "SoloIScene.h"

namespace solo
{
	class IComponent;

	class Scene: public IScene
	{
	public:
		explicit Scene();

		virtual size_t createEmptyNode() override;
		virtual size_t createNode() override;

		virtual bool nodeExists(size_t node) override;

		virtual void addComponent(size_t node, ptr<IComponent> cmp) override;
		virtual ptr<IComponent> findComponent(size_t node, size_t typeId) override;
		virtual ptr<IComponent> getComponent(size_t node, size_t typeId) override;

		void update();

	private:
		typedef std::map<size_t, std::map<size_t, ptr<IComponent>>> NodeComponents;

		int _nodeCounter;
		NodeComponents _nodeComponents;

		void _ensureNodeExists(size_t node);
	};
}

#endif