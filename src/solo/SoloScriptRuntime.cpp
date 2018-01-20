/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "SoloScriptRuntime.h"
#include "SoloDevice.h"
#include "lua/SoloLuaScriptRuntime.h"

using namespace solo;

auto ScriptRuntime::create() -> sptr<ScriptRuntime>
{
    return std::make_shared<LuaScriptRuntime2>();
//    return std::make_shared<LuaScriptRuntime>();
}

auto ScriptRuntime::create(Device *device, const FriendToken<Device>&) -> sptr<ScriptRuntime>
{
    return std::make_shared<LuaScriptRuntime2>(device);
//    return std::make_shared<LuaScriptRuntime>(device);
}
