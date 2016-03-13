#pragma once

#include "SoloBase.h"
#include "SoloVertexFormat.h"
#include "SoloRenderer.h"


namespace solo
{
    class Effect;

    enum class MeshPrefab
    {
        Quad,
        Cube
    };

    class Mesh final
    {
    public:
        // TODO make this a simple constructor?
        static shared<Mesh> createPrefab(Renderer* renderer, MeshPrefab prefab);

        explicit Mesh(Renderer* renderer);
        ~Mesh();

        SL_NONCOPYABLE(Mesh)

        int addBuffer(const VertexBufferLayout& layout, const float* data, int elementCount);
        void removeBuffer(int index);

        int addIndex(const void* data, int elementCount);
        void removeIndex(int index);
        int getIndexCount() const;

        void draw(Effect* effect);
        void drawIndex(int index, Effect* effect);

        void setPrimitiveType(PrimitiveType type);
        PrimitiveType getPrimitiveType() const;

    protected:
        static shared<Mesh> createQuadMesh(Renderer* renderer);
        static shared<Mesh> createBoxMesh(Renderer* renderer);
        void rebuildVertexObject();
        void rebuildEffectBinding(Effect* effect);
        void recalculateMinVertexCount();

        Renderer* renderer;
        Effect* lastEffect = nullptr;

        PrimitiveType primitiveType = PrimitiveType::Triangles;
        std::vector<VertexBufferHandle> vertexBuffers;
        std::vector<IndexBufferHandle> indexBuffers;
        std::vector<int> vertexCounts;
        int minVertexCount = 0;
        VertexObjectHandle vertexObjectHandle;
        VertexObjectHandle effectBindingVertexObjectHandle;
    };

    inline void Mesh::setPrimitiveType(PrimitiveType type)
    {
        primitiveType = type;
    }

    inline PrimitiveType Mesh::getPrimitiveType() const
    {
        return primitiveType;
    }

    inline int Mesh::getIndexCount() const
    {
        return static_cast<int>(indexBuffers.size());
    }
}
