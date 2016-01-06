#pragma once

#include "SoloBase.h"

namespace solo
{
    class Mesh;

    class Model
    {
    public:
        static shared<Model> create();

        Model(const Model &other) = delete;
        Model(Model &&other) = delete;
        Model &operator=(const Model &other) = delete;
        Model &operator=(Model &&other) = delete;
        ~Model() {}

        void addMesh(shared<Mesh> mesh);
        void removeMesh(shared<Mesh> mesh);
        Mesh *getMesh(unsigned index) const;
        size_t getMeshCount() const;

    private:
        Model() {}

        std::vector<shared<Mesh>> meshes;
    };
}