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

#include <LuaIntf.h>
#include <map>
#include <vector>

namespace LuaIntf
{
    LUA_USING_SHARED_PTR_TYPE(std::shared_ptr)
    LUA_USING_LIST_TYPE(std::vector)
    LUA_USING_MAP_TYPE(std::map) // TODO unordered_map
}

using namespace LuaIntf;

#define BEGIN_CLASS(module, klass) module.beginClass<klass>(#klass)
#define BEGIN_CLASS_RENAMED(module, klass, name) module.beginClass<klass>(name)
#define BEGIN_CLASS_EXTEND(module, klass, base) module.beginExtendClass<klass, base>(#klass)

#define REG_CTOR(binding, ...) binding.addConstructor(LUA_ARGS(__VA_ARGS__))

#define REG_VARIABLE(binding, klass, name) binding.addVariable(#name, &klass::name, true)

#define REG_META_METHOD(binding, name, func) binding.addMetaFunction(name, func)
#define REG_METHOD(binding, klass, name) binding.addFunction(#name, &klass::name)
#define REG_METHOD_OVERLOADED(binding, klass, name, nameStr, resultType, modifier, ...) \
	binding.addFunction(nameStr, static_cast<resultType(klass::*)(__VA_ARGS__)modifier>(&klass::name))

#define REG_STATIC_METHOD(binding, klass, name) binding.addStaticFunction(#name, &klass::name)
#define REG_STATIC_METHOD_OVERLOADED(binding, klass, name, nameStr, resultType, modifier, ...) \
	binding.addStaticFunction(nameStr, static_cast<resultType(*)(__VA_ARGS__)modifier>(&klass::name))

#define REG_FREE_FUNC_AS_METHOD(binding, func) binding.addFunction(#func, func)
#define REG_FREE_FUNC_AS_METHOD_RENAMED(binding, func, name) binding.addFunction(name, func)
#define REG_FREE_FUNC_AS_STATIC_FUNC_RENAMED(binding, func, name) binding.addStaticFunction(name, func)

#define REG_MODULE_CONSTANT(module, holder, constant) module.addConstant(#constant, holder::constant)