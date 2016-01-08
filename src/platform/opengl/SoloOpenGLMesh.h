#pragma once

#include "SoloBase.h"
#include "SoloMesh.h"
#include "SoloVertexFormat.h"
#include <GL/glew.h>

namespace solo
{
    class OpenGLIndexedMeshPart;

    class OpenGLMesh : public Mesh
    {
    public:
        virtual ~OpenGLMesh();

        virtual void resetVertexData(const VertexFormat &format, float *data, unsigned elementCount, bool dynamic) override;
        virtual void updateVertexData(float *data, unsigned elementCount, unsigned updateFromIndex) override;
        
        virtual IndexedMeshPart *addIndexedPart() override;
        virtual size_t getPartCount() const override;

        virtual void draw() override;
        virtual void drawIndexedPart(unsigned part) override;

    private:
        friend class Mesh;

        OpenGLMesh();

        static GLenum convertPrimitiveType(MeshPrimitiveType primitiveType);
        static GLenum convertIndexType(MeshIndexFormat indexFormat);

        GLuint bufferHandle = 0;
        unsigned bufferElementCount = 0;
        std::vector<shared<OpenGLIndexedMeshPart>> parts;
    };

    inline size_t OpenGLMesh::getPartCount() const
    {
        return parts.size();
    }
}