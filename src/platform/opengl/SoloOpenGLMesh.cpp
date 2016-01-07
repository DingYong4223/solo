#include "SoloOpenGLMesh.h"

using namespace solo;


OpenGLMesh2::OpenGLMesh2(const VertexFormat &vertexFormat, bool dynamic) :
    Mesh2(vertexFormat, dynamic)
{
    glGenBuffers(1, &vertexBufferHandle);
    if (!vertexBufferHandle)
        SL_THROW_FMT(EngineException, "Unable to obtain mesh handle");
}


void OpenGLMesh2::setVertexData(float *data, unsigned vertexCount, unsigned firstVertexIndex)
{
    if (vertexCount == 0 || firstVertexIndex >= vertexCount)
        SL_THROW_FMT(EngineException, "Invalid combination of first vertex index and vertex count parameters (have ", firstVertexIndex, " and ", vertexCount, ")");

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);

    if (firstVertexIndex + vertexCount > lastVertexCount)
        glBufferData(GL_ARRAY_BUFFER, vertexFormat.getVertexSize() * vertexCount, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    else
        glBufferSubData(GL_ARRAY_BUFFER, firstVertexIndex * vertexFormat.getVertexSize(), vertexCount * vertexFormat.getVertexSize(), data);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


static GLuint buildElementArrayBuffer(GLuint existingHandle, const std::vector<uint16_t> &elements)
{
    auto handle = existingHandle ? existingHandle : 0;
    if (!handle)
        glGenBuffers(1, &handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(uint16_t), elements.data(), GL_STATIC_DRAW);
    return handle;
}


template<typename TElement>
static GLuint buildArrayBuffer(GLuint existingHandle, const std::vector<TElement> &elements, GLuint elementSize, GLuint vertexAttribIndex, GLenum elementType)
{
    auto handle = existingHandle ? existingHandle : 0;
    if (!handle)
        glGenBuffers(1, &handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glBufferData(GL_ARRAY_BUFFER, elements.size() * sizeof(TElement), elements.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(vertexAttribIndex);
    glVertexAttribPointer(vertexAttribIndex, elementSize, elementType, GL_FALSE, 0, nullptr);
    return handle;
}


OpenGLMesh::OpenGLMesh():
    Mesh()
{
    glGenVertexArrays(1, &vertexArrayHandle);
    if (!vertexArrayHandle)
        SL_THROW_FMT(EngineException, "Failed to obtain mesh handle");
}


OpenGLMesh::~OpenGLMesh()
{
    if (vertexBufferHandle)
        glDeleteBuffers(1, &vertexBufferHandle);
    if (normalBufferHandle)
        glDeleteBuffers(1, &normalBufferHandle);
    if (uvBufferHandle)
        glDeleteBuffers(1, &uvBufferHandle);
    if (indicesBufferHandle)
        glDeleteBuffers(1, &indicesBufferHandle);
    glDeleteVertexArrays(1, &vertexArrayHandle);
}


void OpenGLMesh::draw()
{
    glBindVertexArray(vertexArrayHandle);
    if (indicesCount > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferHandle);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCount), GL_UNSIGNED_SHORT, nullptr);
    }
    else
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verticesCount));
}


void OpenGLMesh::setVertices(const std::vector<Vector3> &vertices)
{
    if (!vertices.empty())
    {
        glBindVertexArray(vertexArrayHandle);
        vertexBufferHandle = buildArrayBuffer(vertexBufferHandle, vertices, 3, 0, GL_FLOAT); // binds to the zero vertex attribute
        verticesCount = vertices.size();
    }
}


void OpenGLMesh::setNormals(const std::vector<Vector3> &normals)
{
    if (!normals.empty())
    {
        glBindVertexArray(vertexArrayHandle);
        normalBufferHandle = buildArrayBuffer(normalBufferHandle, normals, 3, 1, GL_FLOAT); // binds to the first vertex attribute
        normalsCount = normals.size();
    }
}


void OpenGLMesh::setUVs(const std::vector<Vector2> &uvs)
{
    if (!uvs.empty())
    {
        glBindVertexArray(vertexArrayHandle);
        uvBufferHandle = buildArrayBuffer(uvBufferHandle, uvs, 2, 2, GL_FLOAT); // binds to the second vertex attribute
        uvsCount = uvs.size();
    }
}


void OpenGLMesh::setIndices(const std::vector<uint16_t> &indices)
{
    if (!indices.empty())
    {
        glBindVertexArray(vertexArrayHandle);
        indicesBufferHandle = buildElementArrayBuffer(indicesBufferHandle, indices);
        indicesCount = indices.size();
    }
}