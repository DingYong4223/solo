#pragma once

#include "SoloBase.h"
#include "SoloRenderer.h"
#include "SoloVector2.h"
#include "SoloVector3.h"
#include "SoloVector4.h"
#include "SoloMatrix.h"
#include <unordered_map>


namespace solo
{
    class Effect;
    class Texture;
    struct RenderContext;

    enum class PolygonFace
    {
        CW,
        CCW,
        All
    };

    enum class DepthPassFunction
    {
        Never,
        Less,
        Equal,
        LEqual,
        Greater,
        NotEqual,
        GEqual,
        Always
    };

    enum class AutoBinding
    {
        WorldMatrix,
        ViewMatrix,
        ProjectionMatrix,
        WorldViewMatrix,
        ViewProjectionMatrix,
        WorldViewProjectionMatrix,
        InverseTransposedWorldMatrix,
        InverseTransposedWorldViewMatrix,
        CameraWorldPosition
    };

    class Material final
    {
    public:
        explicit Material(Renderer* renderer, shared<Effect> effect);
        ~Material();

        SL_NONCOPYABLE(Material)

        void setFloatParameter(const std::string& name, float value);
        void setFloatArrayParameter(const std::string& name, const std::vector<float>& value);
        void setVector2Parameter(const std::string& name, const Vector2& value);
        void setVector2ArrayParameter(const std::string& name, const std::vector<Vector2>& value);
        void setVector3Parameter(const std::string& name, const Vector3& value);
        void setVector3ArrayParameter(const std::string& name, const std::vector<Vector3>& value);
        void setVector4Parameter(const std::string& name, const Vector4& value);
        void setVector4ArrayParameter(const std::string& name, const std::vector<Vector4>& value);
        void setMatrixParameter(const std::string& name, const Matrix& value);
        void setMatrixArrayParameter(const std::string& name, const std::vector<Matrix>& value);
        void setTextureParameter(const std::string& name, shared<Texture> value);
        void setParameterAutoBinding(const std::string& name, AutoBinding autoBinding);

        void bind(RenderContext& context);
        void unbind(RenderContext& context);

        Effect* getEffect() const;

        PolygonFace getPolygonFace() const;
        void setPolygonFace(PolygonFace face);

        bool isDepthWriteEnabled() const;
        void setDepthWriteEnabled(bool enabled);

        bool isDepthTestEnabled() const;
        void setDepthTestEnabled(bool enabled);

        DepthPassFunction getDepthPassFunction() const;
        void setDepthPassFunction(DepthPassFunction func);

    private:
        enum class ParameterValueType
        {
            Float = 0,
            FloatArray,
            Vector2,
            Vector2Array,
            Vector3,
            Vector3Array,
            Vector4,
            Vector4Array,
            Matrix,
            MatrixArray,
            Texture,
            AutoBinding
        };

        // TODO this is quite ugly and takes too much memory
        struct ParameterData
        {
            UniformHandle handle;
            ParameterValueType type = ParameterValueType::Float;
            float floatValue = 0;
            Vector2 vector2Value;
            Vector3 vector3Value;
            Vector4 vector4Value;
            Matrix matrixValue;
            shared<Texture> textureValue;
            std::vector<float> floatArrayValue;
            std::vector<Vector2> vector2ArrayValue;
            std::vector<Vector3> vector3ArrayValue;
            std::vector<Vector4> vector4ArrayValue;
            std::vector<Matrix> matrixArrayValue;
            AutoBinding autoBinding = AutoBinding::CameraWorldPosition;
        };

        void applyState();
        void clearOldValue(ParameterData& data, ParameterValueType newType);

        template <class FieldType>
        void clearAndAssignValue(const std::string& name, FieldType ParameterData::*field, const FieldType& newValue, ParameterValueType valueType);

        void applyAutoBinding(const ParameterData& data, const RenderContext& context);

        Renderer* renderer;
        shared<Effect> effect;

        std::unordered_map<std::string, ParameterData> parameters;

        PolygonFace polygonFace = PolygonFace::CW;
        bool depthWrite = true;
        bool depthTest = true;
        DepthPassFunction depthPassFunc = DepthPassFunction::Less;
    };

    template <class FieldType>
    inline void Material::clearAndAssignValue(const std::string& name, FieldType ParameterData::*field, const FieldType& newValue, ParameterValueType valueType)
    {
        auto& data = parameters.at(name);
        clearOldValue(data, valueType);
        data.*field = newValue;
        data.type = valueType;
    }

    inline Effect* Material::getEffect() const
    {
        return effect.get();
    }

    inline PolygonFace Material::getPolygonFace() const
    {
        return polygonFace;
    }

    inline void Material::setPolygonFace(PolygonFace face)
    {
        polygonFace = face;
    }

    inline bool Material::isDepthWriteEnabled() const
    {
        return depthWrite;
    }

    inline void Material::setDepthWriteEnabled(bool enabled)
    {
        depthWrite = enabled;
    }

    inline bool Material::isDepthTestEnabled() const
    {
        return depthTest;
    }

    inline void Material::setDepthPassFunction(DepthPassFunction func)
    {
        depthPassFunc = func;
    }

    inline DepthPassFunction Material::getDepthPassFunction() const
    {
        return depthPassFunc;
    }

    inline void Material::setDepthTestEnabled(bool enabled)
    {
        depthTest = enabled;
    }
}
