﻿#pragma once

#include "TestBase.h"


class MeshRenderer_Test final: public TestBase
{
public:
    MeshRenderer_Test(Device *device): TestBase(device)
    {
    }

    void run() override final
    {
        material = Material::create(Effect::create("1", "2"));
        mesh = Mesh::create();
        mesh->addPart(nullptr, 0);
        mesh->addPart(nullptr, 0);
        mesh->addPart(nullptr, 0);

        test_EnsureNoMaterialsAtFirst();
        test_NoMeshAfterSetUnset();
        test_SetMaterialForVariousIndexes();
        test_MaterialCountChanges();
    }

private:
    void test_MaterialCountChanges()
    {
        auto renderer = scene->createNode()->addComponent<MeshRenderer>();
        renderer->setMesh(mesh);
        renderer->setMaterial(0, material);
        renderer->setMaterial(1, material);
        assert(renderer->getMaterialCount() == 2);
        assert(renderer->getMaterial(0) == material.get());
        assert(renderer->getMaterial(1) == material.get());
        renderer->setMaterial(0, nullptr);
        assert(renderer->getMaterialCount() == 1);
        assert(renderer->getMaterial(1) == material.get());
    }

    void test_SetMaterialForVariousIndexes()
    {
        auto renderer = scene->createNode()->addComponent<MeshRenderer>();
        renderer->setMesh(mesh);
        renderer->setMaterial(0, material);
        renderer->setMaterial(1, material);
        renderer->setMaterial(2, material);
    }

    void test_EnsureNoMaterialsAtFirst()
    {
        auto renderer = scene->createNode()->addComponent<MeshRenderer>();
        assert(renderer->getMaterialCount() == 0);
    }

    void test_NoMeshAfterSetUnset()
    {
        auto renderer = scene->createNode()->addComponent<MeshRenderer>();
        renderer->setMesh(mesh);
        assert(renderer->getMesh() == mesh.get());
        assert(renderer->getMaterialCount() == 0); // materials get reset

        renderer->setMesh(nullptr);
        assert(renderer->getMesh() == nullptr);
    }

private:
    sptr<Mesh> mesh = nullptr;
    sptr<Material> material = nullptr;
};
