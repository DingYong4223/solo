#pragma once

#include <SDL.h>
#include "SoloDevice.h"

namespace solo
{
	class SDLOpenGLDevice : public Device
	{
	public:
		SDLOpenGLDevice(const EngineCreationArgs &args);
		virtual ~SDLOpenGLDevice();

		virtual void setWindowTitle(const std::string &title) override;
		virtual std::string getWindowTitle() const override;

		virtual void setCursorCaptured(bool captured) override;

		virtual float getLifetime() const override;

		virtual Vector2 getCanvasSize() const override;

		virtual void beginUpdate() override;
		virtual void endUpdate() override;

	private:
		void prepareKeyboardState();
		void prepareMouseState();

		void readWindowState();
		void readEvents();
		void processKeyboardEvent(const SDL_Event& evt);
		void processMouseEvent(const SDL_Event& evt);
		void processWindowEvent(const SDL_Event& evt);
		
		bool hasMouseFocus = false;
		bool hasKeyboardFocus = false;

		SDL_Window *window = nullptr;
		SDL_GLContext context = nullptr;

		std::tuple<int, int> selectContextVersion();
	};
}
