/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "SoloOpenGLMaterial.h"

#ifdef SL_OPENGL_RENDERER

#include "SoloDevice.h"
#include "SoloCamera.h"
#include "SoloOpenGLTexture.h"
#include "SoloTransform.h"
#include "SoloTexture.h"

using namespace solo;

#define SET_PARAM(call) \
    setParameter(name, [value](GLuint location, GLuint index) \
    { \
        return [location, index, value](const Camera *camera, const Transform *nodeTransform) \
        { \
            call; \
        }; \
    });

#define SET_PARAM_NO_VAL(call) \
    setParameter(name, [](GLuint location, GLuint index) \
    { \
        return [location, index](const Camera *camera, const Transform *nodeTransform) \
        { \
            call; \
        }; \
    });

gl::Material::Material(sptr<solo::Effect> effect):
    solo::Material(effect)
{
    this->effect = std::dynamic_pointer_cast<Effect>(effect);
}

void gl::Material::applyParams(const Camera *camera, const Transform *nodeTransform) const
{
    for (const auto &apply : appliers)
        apply(camera, nodeTransform);
}

void gl::Material::setFloatParameter(const std::string &name, float value)
{
    SET_PARAM(glUniform1f(location, value));
}

void gl::Material::setVector2Parameter(const std::string &name, const Vector2 &value)
{
    SET_PARAM(glUniform2f(location, value.x, value.y));
}

void gl::Material::setVector3Parameter(const std::string &name, const Vector3 &value)
{
    SET_PARAM(glUniform3f(location, value.x, value.y, value.z));
}

void gl::Material::setVector4Parameter(const std::string &name, const Vector4 &value)
{
    SET_PARAM(glUniform4f(location, value.x, value.y, value.z, value.w));
}

void gl::Material::setMatrixParameter(const std::string &name, const Matrix &value)
{
    SET_PARAM(glUniformMatrix4fv(location, 1, GL_FALSE, value.m));
}

void gl::Material::setTextureParameter(const std::string &name, sptr<solo::Texture> value)
{
    SET_PARAM(
        // TODO note: owns the texture, that's intentional
        glActiveTexture(GL_TEXTURE0 + index);
        glUniform1i(location, index);
        dynamic_cast<Texture*>(value.get())->bind();
    )
}

void gl::Material::bindParameter(const std::string &name, BindParameterSemantics semantics)
{
    switch (semantics)
    {
        case BindParameterSemantics::WorldMatrix:
        {
            SET_PARAM_NO_VAL(
                if (nodeTransform)
                {
                    auto data = nodeTransform->getWorldMatrix().m;
                    glUniformMatrix4fv(location, 1, GL_FALSE, data);
                }
            );
            break;
        }

        case BindParameterSemantics::ViewMatrix:
        {
            SET_PARAM_NO_VAL(
                if (camera)
                {
                    auto data = camera->getViewMatrix().m;
                    glUniformMatrix4fv(location, 1, GL_FALSE, data);
                }
            );
            break;
        }

        case BindParameterSemantics::ProjectionMatrix:
        {
            SET_PARAM_NO_VAL(
                if (camera)
                {
                    auto data = camera->getProjectionMatrix().m;
                    glUniformMatrix4fv(location, 1, GL_FALSE, data);
                }
            );
            break;
        }

        case BindParameterSemantics::WorldViewMatrix:
        {
            SET_PARAM_NO_VAL(
                if (nodeTransform && camera)
                {
                    auto data = nodeTransform->getWorldViewMatrix(camera).m;
                    glUniformMatrix4fv(location, 1, GL_FALSE, data);
                }
            );
            break;
        }

        case BindParameterSemantics::ViewProjectionMatrix:
        {
            SET_PARAM_NO_VAL(
                if (camera)
                {
                    auto data = camera->getViewProjectionMatrix().m;
                    glUniformMatrix4fv(location, 1, GL_FALSE, data);
                }
            );
            break;
        }

        case BindParameterSemantics::WorldViewProjectionMatrix:
        {
            SET_PARAM_NO_VAL(
                if (nodeTransform && camera)
                {
                    auto data = nodeTransform->getWorldViewProjMatrix(camera).m;
                    glUniformMatrix4fv(location, 1, GL_FALSE, data);
                }
            );
            break;
        }

        case BindParameterSemantics::InverseTransposedWorldMatrix:
        {
            SET_PARAM_NO_VAL(
                if (nodeTransform)
                {
                    auto data = nodeTransform->getInvTransposedWorldMatrix().m;
                    glUniformMatrix4fv(location, 1, GL_FALSE, data);
                }
            );
            break;
        }

        case BindParameterSemantics::InverseTransposedWorldViewMatrix:
        {
            SET_PARAM_NO_VAL(
                if (nodeTransform && camera)
                {
                    auto data = nodeTransform->getInvTransposedWorldViewMatrix(camera).m;
                    glUniformMatrix4fv(location, 1, GL_FALSE, data);
                }
            );
            break;
        }

        case BindParameterSemantics::CameraWorldPosition:
        {
            SET_PARAM_NO_VAL(
                if (camera)
                {
                    auto pos = camera->getTransform()->getWorldPosition();
                    glUniform3f(location, pos.x, pos.y, pos.z);
                }
            );
            break;
        }
        
        default:
            SL_PANIC("Unsupported bind parameter semantics");
    }
}

void gl::Material::setParameter(const std::string &paramName,
    std::function<std::function<void(const Camera *, const Transform *)>(GLuint, GLint)> getApplier)
{
    auto locIt = uniformLocations.find(paramName);
    if (locIt == uniformLocations.end())
    {
        GLint location, index;
        auto found = findUniformInProgram(effect->getHandle(), paramName.c_str(), location, index);
        SL_PANIC_IF(!found, SL_FMT("Could not find uniform '", paramName, "'"));

        appliers.push_back(getApplier(location, index));
        uniformLocations[paramName] = location;
        uniformIndexes[paramName] = index;
        applierIndices[paramName] = appliers.size() - 1;
    }
    else
        appliers[applierIndices.at(paramName)] = getApplier(locIt->second, uniformIndexes.at(paramName));
}

#endif
