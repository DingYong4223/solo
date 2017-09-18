/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "SoloObjMeshLoading.h"
#include "SoloMesh.h"
#include "SoloFileSystem.h"
#include "SoloDevice.h"
#include "SoloVector2.h"
#include "SoloVector3.h"
#include "SoloStringUtils.h"
#include "SoloJobPool.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <unordered_map>

using namespace solo;

struct Vertex
{
    Vector3 position;
    Vector3 normal;
    Vector2 texCoord;

    bool operator==(const Vertex &other) const
    {
        return position == other.position && normal == other.normal && texCoord == other.texCoord;
    }
};

namespace std
{
    template<>
    struct hash<Vertex>
    {
        auto operator()(const Vertex &v) const -> size_t
        {
            return (std::hash<Vector3>()(v.position) ^ (hash<Vector3>()(v.normal) << 1)) >> 1 ^ (hash<Vector2>()(v.texCoord) << 1);
        }
    };
}

struct Data
{
    std::vector<float> vertices;
    std::vector<std::vector<uint16_t>> parts;
};

bool obj::canLoadMesh(const std::string &path)
{
    return stringutils::endsWith(path, ".obj");
}

static auto loadMeshData(Device *device, const std::string &path) -> sptr<Data>
{
    auto file = device->getFileSystem()->getStream(path);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    SL_PANIC_IF(!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file.get()), err);
    
    std::vector<float> vertexData;
    std::vector<std::vector<uint16_t>> parts;

    for (const auto &shape: shapes)
    {
        std::unordered_map<Vertex, uint16_t> uniqueVertices;
        std::vector<uint16_t> indexData;

        for (const auto &index: shape.mesh.indices)
        {
            Vertex v;

            if (index.vertex_index >= 0)
            {
                v.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
            }

            if (index.normal_index >= 0)
            {
                v.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }

            if (index.texcoord_index >= 0)
            {
                v.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1 - attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }

            if (uniqueVertices.count(v) == 0)
            {
                uniqueVertices[v] = static_cast<uint16_t>(vertexData.size() / 8);
                vertexData.push_back(v.position.x);
                vertexData.push_back(v.position.y);
                vertexData.push_back(v.position.z);
                vertexData.push_back(v.normal.x);
                vertexData.push_back(v.normal.y);
                vertexData.push_back(v.normal.z);
                vertexData.push_back(v.texCoord.x);
                vertexData.push_back(v.texCoord.y);
            }

            indexData.push_back(uniqueVertices[v]);
        }

        parts.push_back(indexData);
    }

    return std::make_shared<Data>(Data{std::move(vertexData), std::move(parts)});
}

auto obj::loadMesh(Device *device, const std::string &path) -> sptr<Mesh>
{
    auto file = device->getFileSystem()->getStream(path);
    auto data = loadMeshData(device, path);
    auto mesh = Mesh::create(device);
    
    VertexBufferLayout layout;
    layout.addAttribute(3, 0);
    layout.addAttribute(3, 1);
    layout.addAttribute(2, 2);

    mesh->addVertexBuffer(layout, data->vertices.data(), data->vertices.size() / 8);
    
    for (auto &part : data->parts)
        mesh->addPart(part.data(), part.size());

    return mesh;
}

// TODO Refactor
auto obj::loadMeshAsync(Device *device, const std::string &path) -> sptr<AsyncHandle<Mesh>>
{
    auto handle = std::make_shared<AsyncHandle<Mesh>>();

    auto producers = JobBase<Data>::Producers{[=]() { return loadMeshData(device, path); }};
    auto consumer = [handle, device](const std::vector<sptr<Data>> &results)
    {
        auto data = results[0];
        auto mesh = Mesh::create(device);
    
        VertexBufferLayout layout;
        layout.addAttribute(3, 0);
        layout.addAttribute(3, 1);
        layout.addAttribute(2, 2);

        mesh->addVertexBuffer(layout, data->vertices.data(), data->vertices.size() / 8);
    
        for (auto &part : data->parts)
            mesh->addPart(part.data(), part.size());

        handle->finish(mesh);
    };

    device->getJobPool()->addJob(std::make_shared<JobBase<Data>>(producers, consumer));

    return handle;
}
