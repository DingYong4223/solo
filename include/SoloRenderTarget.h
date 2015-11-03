#pragma once

#include "SoloBase.h"

namespace solo
{
	class Texture2D;
	enum class DeviceMode;

	class RenderTarget
	{
	public:
		RenderTarget(const RenderTarget& other) = delete;
		RenderTarget(RenderTarget&& other) = delete;
		RenderTarget& operator=(const RenderTarget& other) = delete;
		RenderTarget& operator=(RenderTarget&& other) = delete;
		virtual ~RenderTarget() {}

		virtual void bind() = 0;
		virtual void unbind() = 0;

		// should be reentrant
		void setTextures(const std::vector<shared<Texture2D>> &textures);
		std::vector<shared<Texture2D>> getTextures() const;

	protected:
		RenderTarget() {}

		virtual void update() = 0;

		std::vector<shared<Texture2D>> textures;
	};

	class RenderTargetFactory
	{
		friend class ResourceManager;
		static shared<RenderTarget> create(DeviceMode mode);
	};
}
