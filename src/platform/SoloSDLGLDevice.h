#ifndef __SOLO_SDLGL_DEVICE_H__
#define __SOLO_SDLGL_DEVICE_H__

#include <SDL.h>
#include "../SoloDevice.h"

namespace solo
{
	class SDLGLDevice : public Device
	{
	public:
		explicit SDLGLDevice(const EngineCreationArgs &args);
		~SDLGLDevice();

		virtual void setWindowTitle(const char *title) override;

		virtual unsigned long getLifetime() const override;

		virtual Vector2 getCanvasSize() const override;

		virtual void beginUpdate() override;
		virtual void endUpdate() override;

	private:
		SDL_Window *_window;
		SDL_GLContext _context;

		void processSystemEvents();
		std::tuple<int, int> selectContextVersion(int desiredMajorVersion, int desiredMinorVersion);
		std::tuple<SDL_Window*, SDL_GLContext> tryCreateWindowWithContext(bool fake, int ctxMajorVersion, int ctxMinorVersion);
	};
}

#endif