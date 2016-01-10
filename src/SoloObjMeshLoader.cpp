#include "SoloObjMeshLoader.h"
#include "SoloMesh.h"
#include "SoloIndexedMeshPart.h"
#include "SoloFileSystem.h"
#include "SoloResourceManager.h"
#include "SoloVector3.h"
#include "SoloVector2.h"

using namespace solo;


ObjMeshLoader::ObjMeshLoader(FileSystem *fs, ResourceManager *resourceManager):
    MeshLoader(fs, resourceManager)
{
}


bool ObjMeshLoader::isLoadable(const std::string &uri)
{
    return uri.find(".obj", uri.size() - 5) != std::string::npos;
}


Vector3 parseVector3(const char *from, const char *to)
{
    char buf[16];
    float result[3];
    size_t bufIdx = 0;
    size_t resultIdx = -1;
    for (; from <= to; ++from)
    {
        auto c = *from;
        if ((from == to || isspace(c)) && bufIdx > 0)
        {
            buf[bufIdx] = '\0';
            result[++resultIdx] = atof(buf);
            bufIdx = 0;
            if (resultIdx == 2)
                break;
        }
        else
            buf[bufIdx++] = c;
    }
    return Vector3(result[0], result[1], result[2]);
}


void parseIndexes(const char **from, const char *to, unsigned **result)
{
    char buf[16];
    size_t bufIdx = 0;
    size_t resultIdx = 0;
    for (; *from <= to; *from += 1)
    {
        auto c = **from;
        auto okSym = (c >= '0' && c <= '9') || c == '-' || c == '.';
        if (okSym)
            buf[bufIdx++] = c;

        if (!okSym || *from == to)
        {
            if (bufIdx > 0)
            {
                buf[bufIdx] = '\0';
                *result[resultIdx++] = atof(buf);
                bufIdx = 0;
            }
            else if (resultIdx > 0)
                result[resultIdx++] = nullptr;
            if (resultIdx > 2)
                break;
        }
    }
}


shared<Mesh> ObjMeshLoader::load(const std::string &uri)
{
    // TODO this crap could be better...

    std::vector<Vector3> inputVertices, vertices;
    std::vector<Vector3> inputNormals, normals;
    std::vector<Vector2> inputUvs, uvs;
    std::vector<uint16_t> currentPartIndices;
    std::unordered_map<std::string, uint16_t> uniqueIndices;
    std::vector<std::vector<uint16_t>> partIndices;

    auto finishPart = [&]()
    {
        partIndices.push_back(std::move(currentPartIndices));
        currentPartIndices = std::vector<uint16_t>();
        uniqueIndices.clear();
    };

    fs->iterateLines(uri, [&](const std::string & line)
    {
        auto lineSize = line.size();
        if (line[0] == 'v')
        {
            if (line[1] == 'n')
            {
                auto normal = parseVector3(line.c_str() + 3, line.c_str() + lineSize - 3);
                inputNormals.push_back(normal);
            }
            else if (line[1] == 't')
            {
                auto uv = parseVector3(line.c_str() + 3, line.c_str() + lineSize - 3);
                inputUvs.push_back(Vector2(uv.x, uv.y));
            }
            else
            {
                auto vertex = parseVector3(line.c_str() + 2, line.c_str() + lineSize - 2);
                inputVertices.push_back(vertex);
            }
        }
        else if (line[0] == 'f')
        {
            auto from = line.c_str() + 2;
            auto to = from + lineSize - 3;
            unsigned spaceIdx = 1;
            unsigned vIdx, uvIdx, nIdx;
            unsigned *idxs[] = { &vIdx, &uvIdx, &nIdx };
            std::string three;
            for (auto i = 0; i < 3; ++i)
            {
                auto nextSpaceIdx = line.find(' ', spaceIdx + 1);
                three.assign(line.substr(spaceIdx + 1, nextSpaceIdx != std::string::npos ? (nextSpaceIdx - spaceIdx - 1) : lineSize - spaceIdx));
                spaceIdx = nextSpaceIdx;
                auto it = uniqueIndices.find(three);
                if (it != uniqueIndices.end())
                {
                    currentPartIndices.push_back(it->second);
                    from += three.size() + 1;
                }
                else
                {
                    parseIndexes(&from, to, idxs);
                    vertices.push_back(inputVertices[vIdx - 1]);
                    if (idxs[1])
                        uvs.push_back(inputUvs[uvIdx - 1]);
                    if (idxs[2])
                        normals.push_back(inputNormals[nIdx - 1]);
                    auto newIndex = static_cast<uint16_t>(vertices.size() - 1);
                    uniqueIndices[three] = newIndex;
                    currentPartIndices.push_back(newIndex);
                }
            }
        }
        else if (line[0] == 'o')
        {
            if (!currentPartIndices.empty())
                finishPart();
        }
        return true;
    });

    if (!currentPartIndices.empty())
        finishPart();

    auto hasUVs = !uvs.empty();
    auto hasNormals = !normals.empty();
    std::vector<VertexFormatElement> vertexFormatElements{ VertexFormatElement(VertexFormatElementSemantics::Position, 3, 0) };
    if (hasUVs)
        vertexFormatElements.push_back(VertexFormatElement(VertexFormatElementSemantics::TexCoord0, 2, 1));
    if (hasNormals)
        vertexFormatElements.push_back(VertexFormatElement(VertexFormatElementSemantics::Normal, 3, 2));
    auto mesh = resourceManager->getOrCreateMesh(VertexFormat(vertexFormatElements), uri);
    mesh->resetStorage(0, reinterpret_cast<const float*>(vertices.data()), vertices.size(), false);
    if (hasUVs)
        mesh->resetStorage(1, reinterpret_cast<const float*>(uvs.data()), uvs.size(), false);
    if (hasNormals)
        mesh->resetStorage(2, reinterpret_cast<const float*>(normals.data()), normals.size(), false);

    for (const auto& indices: partIndices)
    {
        auto part = mesh->addPart(MeshIndexFormat::UnsignedShort);
        part->setPrimitiveType(MeshPrimitiveType::Triangles);
        part->resetIndexData(reinterpret_cast<const void*>(indices.data()), indices.size(), false);
    }

    return mesh;
}