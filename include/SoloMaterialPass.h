#pragma once

#include "SoloBase.h"
#include "SoloRenderState.h"
#include "SoloMaterial.h"

namespace solo
{
	class Effect;

	class MaterialPass: public RenderState
	{
		friend Material;

	public:
		~MaterialPass() {}

		void bind();
		void unbind();

	private:
		MaterialPass(Material *material, ptr<Effect> effect);

		static ptr<MaterialPass> create(Material *material, ptr<Effect> effect);

		Material *_material;
		ptr<Effect> _effect;
	};
}