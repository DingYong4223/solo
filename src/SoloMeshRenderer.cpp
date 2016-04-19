#include "SoloMeshRenderer.h"
#include "SoloMesh.h"
#include "SoloMaterial.h"
#include "SoloRenderContext.h"

using namespace solo;


MeshRenderer::MeshRenderer(const Node& node):
    ComponentBase(node)
{
    renderQueue = KnownRenderQueues::Opaque;
}


void MeshRenderer::render(RenderContext& context)
{
    if (!mesh || materials.empty())
        return;

    auto partCount = mesh->getPartCount();
    if (partCount == 0)
    {
        auto material = getMaterial(0);
        if (material)
        {
            material->apply(context);
            mesh->draw(material->getEffect());
        }
    }
    else
    {
        for (uint32_t part = 0; part < partCount; ++part)
        {
            auto material = getMaterial(part);
            if (material)
            {
                material->apply(context);
                mesh->drawPart(material->getEffect(), part);
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
