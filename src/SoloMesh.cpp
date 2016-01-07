#include "SoloDevice.h"
#include "platform/stub/SoloStubMesh.h"
#include "platform/opengl/SoloOpenGLMesh.h"

using namespace solo;


shared<Mesh> Mesh::create(DeviceMode mode)
{
    if (mode == DeviceMode::OpenGL)
        return SL_NEW_SHARED(StubMesh);
    return SL_NEW_SHARED(OpenGLMesh);
}

//
//void Mesh::rebuildAsQuad()
//{
//    setVertices(
//    {
//        Vector3(-1, -1, 0),
//        Vector3(-1, 1, 0),
//        Vector3(1, 1, 0),
//        Vector3(1, -1, 0)
//    });
//    setNormals(
//    {
//        Vector3(0, 0, -1),
//        Vector3(0, 0, -1),
//        Vector3(0, 0, -1),
//        Vector3(0, 0, -1)
//    });
//    setUVs(
//    {
//        Vector2(0, 0),
//        Vector2(0, 1),
//        Vector2(1, 1),
//        Vector2(1, 0)
//    });
//    setIndices(
//    {
//        0, 1, 2,
//        0, 2, 3
//    });
//}
//
//
//void Mesh::rebuildAsBox()
//{
//    setVertices(
//    {
//        Vector3(-1, -1, 1),
//        Vector3(-1, 1, 1),
//        Vector3(1, 1, 1),
//        Vector3(1, -1, 1),
//
//        Vector3(-1, -1, -1),
//        Vector3(-1, 1, -1),
//        Vector3(-1, 1, 1),
//        Vector3(-1, -1, 1),
//
//        Vector3(1, -1, -1),
//        Vector3(1, 1, -1),
//        Vector3(-1, 1, -1),
//        Vector3(-1, -1, -1),
//
//        Vector3(1, -1, 1),
//        Vector3(1, 1, 1),
//        Vector3(1, 1, -1),
//        Vector3(1, -1, -1),
//
//        Vector3(-1, 1, 1),
//        Vector3(-1, 1, -1),
//        Vector3(1, 1, -1),
//        Vector3(1, 1, 1),
//
//        Vector3(-1, -1, -1),
//        Vector3(-1, -1, 1),
//        Vector3(1, -1, 1),
//        Vector3(1, -1, -1)
//    });
//
//    setUVs(
//    {
//        Vector2(0, 0),
//        Vector2(0, 1),
//        Vector2(1, 1),
//        Vector2(1, 0),
//        Vector2(0, 0),
//        Vector2(0, 1),
//        Vector2(1, 1),
//        Vector2(1, 0),
//        Vector2(0, 0),
//        Vector2(0, 1),
//        Vector2(1, 1),
//        Vector2(1, 0),
//        Vector2(0, 0),
//        Vector2(0, 1),
//        Vector2(1, 1),
//        Vector2(1, 0),
//        Vector2(0, 0),
//        Vector2(0, 1),
//        Vector2(1, 1),
//        Vector2(1, 0),
//        Vector2(0, 0),
//        Vector2(0, 1),
//        Vector2(1, 1),
//        Vector2(1, 0)
//    });
//
//    setIndices(
//    {
//        0, 1, 2,
//        0, 2, 3,
//        4, 5, 6,
//        4, 6, 7,
//        8, 9, 10,
//        8, 10, 11,
//        12, 13, 14,
//        12, 14, 15,
//        16, 17, 18,
//        16, 18, 19,
//        20, 21, 22,
//        20, 22, 23
//    });
//}
