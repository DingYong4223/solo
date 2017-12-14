/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "SoloSkyboxRenderer.h"
#include "SoloMesh.h"
#include "SoloMaterial.h"
#include "SoloTexture.h"
#include "SoloTransform.h"
#include "SoloDevice.h"
#include "SoloRenderCommand.h"

using namespace solo;

SkyboxRenderer::SkyboxRenderer(const Node &node):
    ComponentBase(node),
    renderer(node.getScene()->getDevice()->getRenderer())
{
    transform = node.findComponent<Transform>();

    const auto device = node.getScene()->getDevice();

    const auto effect = Effect::createFromPrefab(device, EffectPrefab::Skybox);
    material = Material::create(device, effect);
    material->bindParameter("projMatrix", BindParameterSemantics::ProjectionMatrix);
    material->bindParameter("worldViewMatrix", BindParameterSemantics::WorldViewMatrix);
    material->setDepthTest(true);
    material->setDepthWrite(false);
    material->setFaceCull(FaceCull::Back);

    quadMesh = Mesh::createFromPrefab(device, MeshPrefab::Quad);
}

void SkyboxRenderer::render()
{
    renderer->addRenderCommand(RenderCommand::drawMesh(quadMesh.get(), transform, material.get())); // TODO transform needed here?
}

void SkyboxRenderer::setTexture(sptr<CubeTexture> texture)
{
    this->texture = texture;
    material->setTextureParameter("mainTex", texture);
}
