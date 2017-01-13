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


namespace solo
{
    class Material;
    class FrameBuffer;
    class Device;

    class Graphics
    {
    public:
        SL_DISABLE_COPY_AND_MOVE(Graphics)

        static auto create(Device *device, const FriendToken<Device> &token) -> sptr<Graphics>;

        virtual ~Graphics() {}

        virtual void blit(Material *material, FrameBuffer *target) = 0;

    protected:
        explicit Graphics(const FriendToken<Device> &) {}
    };
}