/* 
 * Copyright (c) Aleksey Fedotov 
 * MIT license 
 */

#include "SoloLuaCommon.h"
#include "SoloMesh.h"

using namespace solo;

static auto addVertexBuffer(Mesh *mesh, const VertexBufferLayout &layout, const vec<float> &data, u32 vertexCount) -> u32
{
    return mesh->addVertexBuffer(layout, data.data(), vertexCount);
}

static auto addDynamicVertexBuffer(Mesh *mesh, const VertexBufferLayout &layout, const vec<float> &data, u32 vertexCount) -> u32
{
    return mesh->addDynamicVertexBuffer(layout, data.data(), vertexCount);
}

static void updateDynamicVertexBuffer(Mesh *mesh, u32 index, u32 vertexOffset, const vec<float> &data, u32 vertexCount)
{
    mesh->updateDynamicVertexBuffer(index, vertexOffset, data.data(), vertexCount);
}

static auto addPart(Mesh *mesh, const vec<u32> &indexData, u32 indexElementCount) -> u32 // TODO 16-bit index support?
{
    return mesh->addPart(indexData.data(), indexElementCount);
}

static void registerVertexBufferLayout(CppBindModule<LuaBinding> &module)
{
    auto el = BEGIN_CLASS(module, VertexAttribute);
    REG_FIELD(el, VertexAttribute, name);
    REG_FIELD(el, VertexAttribute, location);
    REG_FIELD(el, VertexAttribute, elementCount);
    REG_FIELD(el, VertexAttribute, size);
    REG_FIELD(el, VertexAttribute, offset);
    REG_FIELD(el, VertexAttribute, usage);
    el.endClass();

    auto layout = BEGIN_CLASS(module, VertexBufferLayout);
    REG_CTOR(layout);
    REG_METHOD_OVERLOADED(layout, VertexBufferLayout, addAttribute, "addAttribute", void, , VertexAttributeUsage);
    REG_METHOD(layout, VertexBufferLayout, attribute);
    REG_METHOD(layout, VertexBufferLayout, attributeCount);
    REG_METHOD(layout, VertexBufferLayout, size);
    REG_METHOD(layout, VertexBufferLayout, elementCount);
    layout.endClass();
}

static void registerMesh(CppBindModule<LuaBinding> &module)
{
    {
        auto binding = BEGIN_CLASS(module, Mesh);
        REG_STATIC_METHOD(binding, Mesh, empty);
        REG_STATIC_METHOD(binding, Mesh, fromFile);
        REG_STATIC_METHOD(binding, Mesh, fromFileAsync);
        REG_FREE_FUNC_AS_METHOD(binding, addVertexBuffer);
        REG_FREE_FUNC_AS_METHOD(binding, addDynamicVertexBuffer);
        REG_FREE_FUNC_AS_METHOD(binding, updateDynamicVertexBuffer);
        REG_METHOD(binding, Mesh, removeVertexBuffer);
        REG_FREE_FUNC_AS_METHOD(binding, addPart);
        REG_METHOD(binding, Mesh, removePart);
        REG_METHOD(binding, Mesh, partCount);
        REG_METHOD(binding, Mesh, primitiveType);
        REG_METHOD(binding, Mesh, setPrimitiveType);
        REG_PTR_EQUALITY(binding, Mesh);
        binding.endClass();
    }
    {
        auto binding = BEGIN_CLASS_RENAMED(module, AsyncHandle<Mesh>, "MeshAsyncHandle");
        REG_METHOD(binding, AsyncHandle<Mesh>, done);
        binding.endClass();
    }
}

void registerMeshApi(CppBindModule<LuaBinding> &module)
{
    registerMesh(module);
    registerVertexBufferLayout(module);
}
