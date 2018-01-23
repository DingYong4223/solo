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

OpenGLMaterial::OpenGLMaterial(sptr<Effect> effect):
    effect(std::static_pointer_cast<OpenGLEffect>(effect))
{
}

void OpenGLMaterial::applyParams(const Camera *camera, const Transform *nodeTransform) const
{
    for (const auto &p : appliers)
        p.second(camera, nodeTransform);
}

void OpenGLMaterial::setFloatParameter(const str &name, float value)
{
    setParameter(name, [value](GLuint location, GLuint index)
    {
        return [location, value](const Camera *, const Transform *)
        {
            glUniform1f(location, value);
        };
    });
}

void OpenGLMaterial::setVector2Parameter(const str &name, const Vector2 &value)
{
    setParameter(name, [value](GLuint location, GLuint index)
    {
        return [location, value](const Camera *, const Transform *)
        {
            glUniform2f(location, value.x, value.y);
        };
    });
}

void OpenGLMaterial::setVector3Parameter(const str &name, const Vector3 &value)
{
    setParameter(name, [value](GLuint location, GLuint index)
    {
        return [location, value](const Camera *, const Transform *)
        {
            glUniform3f(location, value.x, value.y, value.z);
        };
    });
}

void OpenGLMaterial::setVector4Parameter(const str &name, const Vector4 &value)
{
    setParameter(name, [value](GLuint location, GLuint index)
    {
        return [location, value](const Camera *, const Transform *)
        {
            glUniform4f(location, value.x, value.y, value.z, value.w);
        };
    });
}

void OpenGLMaterial::setMatrixParameter(const str &name, const Matrix &value)
{
    setParameter(name, [value](GLuint location, GLuint index)
    {
        return [location, value](const Camera *, const Transform *)
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, value.m);
        };
    });
}

void OpenGLMaterial::setTextureParameter(const str &name, sptr<Texture> value)
{
    auto tex = std::dynamic_pointer_cast<OpenGLTexture>(value);
    setParameter(name, [tex](GLuint location, GLuint index)
    {
        return [location, index, tex](const Camera *, const Transform *)
        {
            glActiveTexture(GL_TEXTURE0 + index);
            glUniform1i(location, index);
            tex->bind();
        };
    });
}

void OpenGLMaterial::bindParameter(const str &name, BindParameterSemantics semantics)
{
    switch (semantics)
    {
        case BindParameterSemantics::WorldMatrix:
        {
            setParameter(name, [](GLuint location, GLuint index)
            {
                return [location](const Camera *camera, const Transform *nodeTransform)
                {
                    if (nodeTransform)
                    {
                        auto data = nodeTransform->getWorldMatrix().m;
                        glUniformMatrix4fv(location, 1, GL_FALSE, data);
                    }
                };
            });
            break;
        }

        case BindParameterSemantics::ViewMatrix:
        {
            setParameter(name, [](GLuint location, GLuint index)
            {
                return [location](const Camera *camera, const Transform *nodeTransform)
                {
                    if (camera)
                    {
                        auto data = camera->getViewMatrix().m;
                        glUniformMatrix4fv(location, 1, GL_FALSE, data);
                    }
                };
            });
            break;
        }

        case BindParameterSemantics::ProjectionMatrix:
        {
            setParameter(name, [](GLuint location, GLuint index)
            {
                return [location](const Camera *camera, const Transform *nodeTransform)
                {
                    if (camera)
                    {
                        auto data = camera->getProjectionMatrix().m;
                        glUniformMatrix4fv(location, 1, GL_FALSE, data);
                    }
                };
            });
            break;
        }

        case BindParameterSemantics::WorldViewMatrix:
        {
            setParameter(name, [](GLuint location, GLuint index)
            {
                return [location](const Camera *camera, const Transform *nodeTransform)
                {
                    if (nodeTransform && camera)
                    {
                        auto data = nodeTransform->getWorldViewMatrix(camera).m;
                        glUniformMatrix4fv(location, 1, GL_FALSE, data);
                    }
                };
            });
            break;
        }

        case BindParameterSemantics::ViewProjectionMatrix:
        {
            setParameter(name, [](GLuint location, GLuint index)
            {
                return [location](const Camera *camera, const Transform *nodeTransform)
                {
                    if (camera)
                    {
                        auto data = camera->getViewProjectionMatrix().m;
                        glUniformMatrix4fv(location, 1, GL_FALSE, data);
                    }
                };
            });
            break;
        }

        case BindParameterSemantics::WorldViewProjectionMatrix:
        {
            setParameter(name, [](GLuint location, GLuint index)
            {
                return [location](const Camera *camera, const Transform *nodeTransform)
                {
                    if (nodeTransform && camera)
                    {
                        auto data = nodeTransform->getWorldViewProjMatrix(camera).m;
                        glUniformMatrix4fv(location, 1, GL_FALSE, data);
                    }
                };
            });
            break;
        }

        case BindParameterSemantics::InverseTransposedWorldMatrix:
        {
            setParameter(name, [](GLuint location, GLuint index)
            {
                return [location](const Camera *camera, const Transform *nodeTransform)
                {
                    if (nodeTransform)
                    {
                        auto data = nodeTransform->getInvTransposedWorldMatrix().m;
                        glUniformMatrix4fv(location, 1, GL_FALSE, data);
                    }
                };
            });
            break;
        }

        case BindParameterSemantics::InverseTransposedWorldViewMatrix:
        {
            setParameter(name, [](GLuint location, GLuint index)
            {
                return [location](const Camera *camera, const Transform *nodeTransform)
                {
                    if (nodeTransform && camera)
                    {
                        auto data = nodeTransform->getInvTransposedWorldViewMatrix(camera).m;
                        glUniformMatrix4fv(location, 1, GL_FALSE, data);
                    }
                };
            });
            break;
        }

        case BindParameterSemantics::CameraWorldPosition:
        {
            setParameter(name, [](GLuint location, GLuint index)
            {
                return [location](const Camera *camera, const Transform *nodeTransform)
                {
                    if (camera)
                    {
                        auto pos = camera->getTransform()->getWorldPosition();
                        glUniform3f(location, pos.x, pos.y, pos.z);
                    }
                };
            });
            break;
        }

        default:
            SL_PANIC("Unsupported bind parameter semantics");
    }
}

void OpenGLMaterial::setParameter(const str &paramName, const std::function<ParameterApplier(GLuint, GLint)> &getApplier)
{
    auto name = paramName;
    const auto idx = paramName.find_last_of(':');
	if (idx != std::string::npos)
		name.replace(idx, 1, "_");
    const auto info = effect->getUniformInfo(name);
    appliers[paramName] = getApplier(info.location, info.samplerIndex);
}

#endif
