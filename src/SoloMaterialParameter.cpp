#include "SoloMaterialParameter.h"
#include "SoloTransform.h"
#include "SoloVector2.h"
#include "SoloVector3.h"
#include "SoloVector4.h"
#include "SoloMatrix.h"
#include "SoloRenderContext.h"
#include "SoloMaterial.h"
#include "SoloEffect.h"
#include "SoloCamera.h"

using namespace solo;


MaterialParameter::MaterialParameter(const std::string& name):
	name(name)
{
}


void MaterialParameter::setFloat(float value)
{
	clearValue();
	floatValue = value;
	type = ValueType::Float;
}


void MaterialParameter::setFloatArray(const std::vector<float>& value)
{
	clearValue();
	floatArrayValue = value;
	type = ValueType::FloatArray;
}


void MaterialParameter::setInt(int value)
{
	clearValue();
	intValue = value;
	type = ValueType::Int;
}


void MaterialParameter::setIntArray(const std::vector<int>& value)
{
	clearValue();
	intArrayValue = value;
	type = ValueType::IntArray;
}


void MaterialParameter::setVector2(const Vector2 &value)
{
	clearValue();
	vector2Value = value;
	type = ValueType::Vector2;
}


void MaterialParameter::setVector2Array(const std::vector<Vector2>& value)
{
	clearValue();
	vector2ArrayValue = value;
	type = ValueType::Vector2Array;
}


void MaterialParameter::setVector3(const Vector3 &value)
{
	clearValue();
	vector3Value = value;
	type = ValueType::Vector3;
}


void MaterialParameter::setVector3Array(const std::vector<Vector3>& value)
{
	clearValue();
	vector3ArrayValue = value;
	type = ValueType::Vector3Array;
}


void MaterialParameter::setVector4(const Vector4& value)
{
	clearValue();
	vector4Value = value;
	type = ValueType::Vector4;
}


void MaterialParameter::setVector4Array(const std::vector<Vector4>& value)
{
	clearValue();
	vector4ArrayValue = value;
	type = ValueType::Vector4Array;
}


void MaterialParameter::setMatrix(const Matrix& value)
{
	clearValue();
	matrixValue = value;
	type = ValueType::Matrix;
}


void MaterialParameter::setMatrixArray(const std::vector<Matrix>& value)
{
	clearValue();
	matrixArrayValue = value;
	type = ValueType::MatrixArray;
}


void MaterialParameter::setTexture(const shared<Texture> texture)
{
	clearValue();
	textureValue = texture;
	type = ValueType::Texture;
}


void MaterialParameter::setTextureArray(const std::vector<shared<Texture>>& textures)
{
	clearValue();
	textureArrayValue = textures;
	type = ValueType::TextureArray;
}


void MaterialParameter::setFunction(std::function<void(EffectVariable* variable, const RenderContext& context)> func)
{
	clearValue();
	this->func = func;
	type = ValueType::Func;
}


void MaterialParameter::apply(const RenderContext& context)
{
	auto variable = context.material->getEffect()->findVariable(name);
	if (!variable)
		return;
	switch (type)
	{
		case ValueType::Float:
			variable->setFloat(floatValue);
			break;
		case ValueType::FloatArray:
			variable->setFloatArray(floatArrayValue.data(), floatArrayValue.size());
			break;
		case ValueType::Int:
			variable->setFloat(intValue);
			break;
		case ValueType::IntArray:
			variable->setIntArray(intArrayValue.data(), intArrayValue.size());
			break;
		case ValueType::Vector2:
			variable->setVector2Array(vector2ArrayValue.data(), vector2ArrayValue.size());
			break;
		case ValueType::Vector3:
			variable->setVector3Array(vector3ArrayValue.data(), vector3ArrayValue.size());
			break;
		case ValueType::Vector4:
			variable->setVector4Array(vector4ArrayValue.data(), vector4ArrayValue.size());
			break;
		case ValueType::Matrix:
			variable->setMatrixArray(matrixArrayValue.data(), matrixArrayValue.size());
			break;
		case ValueType::Texture:
			variable->setTexture(textureValue);
			break;
		case ValueType::TextureArray:
			variable->setTextureArray(textureArrayValue, textureArrayValue.size());
			break;
		case ValueType::Func:
			func(variable, context);
			break;
		case ValueType::None:
		default:
			break;
	}
}


void MaterialParameter::clearValue()
{
	switch (type)
	{
		case ValueType::FloatArray:
			floatArrayValue.clear();
			break;
		case ValueType::IntArray:
			intArrayValue.clear();
			break;
		case ValueType::Vector2Array:
			vector2ArrayValue.clear();
			break;
		case ValueType::Vector3Array:
			vector3ArrayValue.clear();
			break;
		case ValueType::Vector4Array:
			vector4ArrayValue.clear();
			break;
		case ValueType::MatrixArray:
			matrixArrayValue.clear();
			break;
		case ValueType::Texture:
			textureValue = nullptr;
			break;
		case ValueType::TextureArray:
			textureArrayValue.clear();
			break;
		case ValueType::Func:
			func = nullptr;
			break;
		case ValueType::None:
		default:
			break;
	}
	type = ValueType::None;
}


void MaterialParameter::setWorldMatrix(EffectVariable *variable, const RenderContext& context)
{
	variable->setMatrix(context.nodeTransform->getWorldMatrix());
}


void MaterialParameter::setViewMatrix(EffectVariable *variable, const RenderContext& context)
{
	variable->setMatrix(context.camera->getViewMatrix());
}


void MaterialParameter::setProjectionMatrix(EffectVariable *variable, const RenderContext& context)
{
	variable->setMatrix(context.camera->getProjectionMatrix());
}


void MaterialParameter::setWorldViewMatrix(EffectVariable *variable, const RenderContext& context)
{
	variable->setMatrix(context.nodeTransform->getWorldViewMatrix(context.camera));
}


void MaterialParameter::setViewProjectionMatrix(EffectVariable *variable, const RenderContext& context)
{
	variable->setMatrix(context.camera->getViewProjectionMatrix());
}


void MaterialParameter::setWorldViewProjectionMatrix(EffectVariable *variable, const RenderContext& context)
{
	variable->setMatrix(context.nodeTransform->getWorldViewProjectionMatrix(context.camera));
}


void MaterialParameter::setInverseTransposedWorldViewMatrix(EffectVariable *variable, const RenderContext& context)
{
	variable->setMatrix(context.nodeTransform->getInverseTransposedWorldViewMatrix(context.camera));
}


void MaterialParameter::setInverseTransposedWorldMatrix(EffectVariable *variable, const RenderContext& context)
{
	variable->setMatrix(context.nodeTransform->getInverseTransposedWorldMatrix());
}


void MaterialParameter::setCameraWorldPosition(EffectVariable *variable, const RenderContext& context)
{
	variable->setVector3(context.cameraTransform->getWorldPosition());
}


void MaterialParameter::bindValue(AutoBinding autoBinding)
{
	switch (autoBinding)
	{
		case AutoBinding::WorldMatrix:
			setFunction(&MaterialParameter::setWorldMatrix);
			break;
		case AutoBinding::ViewMatrix:
			setFunction(&MaterialParameter::setViewMatrix);
			break;
		case AutoBinding::ProjectionMatrix:
			setFunction(&MaterialParameter::setProjectionMatrix);
			break;
		case AutoBinding::ViewProjectionMatrix:
			setFunction(&MaterialParameter::setViewProjectionMatrix);
			break;
		case AutoBinding::WorldViewProjectionMatrix:
			setFunction(&MaterialParameter::setWorldViewProjectionMatrix);
			break;
		case AutoBinding::InverseTransposedWorldMatrix:
			setFunction(&MaterialParameter::setInverseTransposedWorldMatrix);
			break;
		case AutoBinding::InverseTransposedWorldViewMatrix:
			setFunction(&MaterialParameter::setInverseTransposedWorldViewMatrix);
			break;
		case AutoBinding::CameraWorldPosition:
			setFunction(&MaterialParameter::setCameraWorldPosition);
			break;
		case AutoBinding::None:
		default:
			break;
	}
}
