/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "SoloDevice.h"

namespace solo
{
    class NullDevice final : public Device
    {
    public:
        explicit NullDevice(const DeviceSetup &setup) : Device(setup) {}

        void setWindowTitle(const str &title) override final { windowTitle = title; }
        auto getWindowTitle() const -> str override final { return windowTitle; }
        void saveScreenshot(const str &path) override final {}
        void setCursorCaptured(bool captured) override final {}
        auto getCanvasSize() const -> Vector2 override final { return {1, 1}; }
        auto getDpiIndependentCanvasSize() const -> Vector2 override final { return {1, 1}; }
        auto getLifetime() const -> float override final { return 0; }

    private:
        str windowTitle;

        void beginUpdate() override final {}
        void endUpdate() override final {}
    };
}
