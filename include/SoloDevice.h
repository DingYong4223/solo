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

#include "SoloCommon.h"
#include "SoloVector2.h"
#include "SoloDeviceSetup.h"
#include <unordered_map>
#include <unordered_set>
#include <functional>


namespace solo
{
    class Scene;
    class AssetLoader;
    class FileSystem;
    class Renderer;
    class Physics;
    class ScriptRuntime;
    class Logger;

    enum class KeyCode
    {
        Digit0,
        Digit1,
        Digit2,
        Digit3,
        Digit4,
        Digit5,
        Digit6,
        Digit7,
        Digit8,
        Digit9,
        Q, W, E, R, T, Y, U, I, O, P,
        A, S, D, F, G, H, J, K, L,
        Z, X, C, V, B, N, M,
        Escape,
        LeftArrow,
        RightArrow,
        UpArrow,
        DownArrow,
        Backspace,
        Space,
        Return,
        LeftShift,
        LeftCtrl,
        LeftAlt,
        RightShift,
        RightCtrl,
        RightAlt
        // ...
    };

    enum class MouseButton
    {
        Left,
        Middle,
        Right
    };

    enum class DeviceMode
    {
        Null,
        OpenGL,
        Vulkan
    };

    class Device
    {
    public:
        SL_DISABLE_COPY_AND_MOVE(Device)

        static auto create(const DeviceSetup &setup) -> uptr<Device>;

        virtual ~Device() {}

        virtual auto getWindowTitle() const -> std::string = 0;
        virtual void setWindowTitle(const std::string &title) = 0;

        virtual auto getCanvasSize() const -> Vector2 = 0;

        virtual void saveScreenshot(const std::string &path) = 0;

        virtual void setCursorCaptured(bool captured) = 0;

        virtual auto getLifetime() const -> float = 0;

        auto getTimeDelta() const -> float;

        bool isWindowCloseRequested() const;
        bool isQuitRequested() const;

        bool isKeyPressed(KeyCode code, bool firstTime = false) const;
        bool isKeyReleased(KeyCode code) const;

        auto getMouseMotion() const -> Vector2;
        bool isMouseButtonDown(MouseButton button, bool firstTime = false) const;
        bool isMouseButtonReleased(MouseButton button) const;

        void update(std::function<void()> update);

        auto getSetup() const -> DeviceSetup;

        auto getFileSystem() const -> FileSystem*;
        auto getAssetLoader() const -> AssetLoader*;
        auto getRenderer() const -> Renderer*;
        auto getPhysics() const -> Physics*;
        auto getScriptRuntime() const -> ScriptRuntime*;
        auto getLogger() const -> Logger*;

    protected:
        explicit Device(const DeviceSetup &setup);
        
        virtual void beginUpdate() = 0;
        virtual void endUpdate() = 0;
        void initSubsystems();
        void updateTime();

        DeviceSetup setup;

        // Keep order of these fields - matters during destruction
        sptr<Logger> logger;
        sptr<Renderer> renderer;
        sptr<Physics> physics;
        sptr<FileSystem> fs;
        sptr<AssetLoader> assetLoader;
        // Scene may contain refs to script objects, so it must be destroyed first
        sptr<ScriptRuntime> scriptRuntime;

        // key code -> was pressed for the first time
        std::unordered_map<KeyCode, bool> pressedKeys;
        std::unordered_set<KeyCode> releasedKeys;

        int32_t mouseDeltaX = 0;
        int32_t mouseDeltaY = 0;
        std::unordered_map<MouseButton, bool> pressedMouseButtons;
        std::unordered_set<MouseButton> releasedMouseButtons;

        float lastUpdateTime = 0;
        float timeDelta = 0;

        bool windowCloseRequested = false;
        bool quitRequested = false;
    };

    inline bool Device::isWindowCloseRequested() const
    {
        return windowCloseRequested;
    }

    inline bool Device::isQuitRequested() const
    {
        return quitRequested;
    }

    inline auto Device::getTimeDelta() const -> float
    {
        return timeDelta;
    }

    inline auto Device::getFileSystem() const -> FileSystem *
    {
        return fs.get();
    }

    inline auto Device::getRenderer() const -> Renderer *
    {
        return renderer.get();
    }

    inline auto Device::getAssetLoader() const -> AssetLoader *
    {
        return assetLoader.get();
    }

    inline auto Device::getPhysics() const -> Physics *
    {
        return physics.get();
    }

    inline auto Device::getScriptRuntime() const -> ScriptRuntime*
    {
        return scriptRuntime.get();
    }

    inline auto Device::getLogger() const -> Logger *
    {
        return logger.get();
    }

    inline auto Device::getSetup() const -> DeviceSetup
    {
        return setup;
    }
}
