/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "SoloMeshRenderer.h"
#include "SoloMesh.h"
#include "SoloMaterial.h"
#include "SoloRenderContext.h"
#include "SoloTransform.h"
#include "SoloDevice.h"

using namespace solo;


MeshRenderer::MeshRenderer(const Node &node):
    ComponentBase(node),
    renderer(node.getScene()->getDevice()->getRenderer())
{
    transform = node.findComponent<Transform>();
}


void MeshRenderer::render(const RenderContext &context)
{
    if (!mesh || materials.empty())
        return;

    auto partCount = mesh->getPartCount();
    if (partCount == 0)
    {
        auto material = getMaterial(0);
        if (material)
        {
            renderer->addRenderCommand(RenderCommand::applyMaterial(material));
            renderer->addRenderCommand(RenderCommand::drawMesh(mesh.get()));
        }
    }
    else
    {
        for (uint32_t part = 0; part < partCount; ++part)
        {
            auto material = getMaterial(part);
            if (material)
            {
                renderer->addRenderCommand(RenderCommand::applyMaterial(material));
                renderer->addRenderCommand(RenderCommand::drawMeshPart(mesh.get(), part));
            }
        }
    }
}


void MeshRenderer::setMaterial(uint32_t index, sptr<Material> material)
{
    if (material)
        materials[index] = material;
    else
        materials.erase(index);
}
