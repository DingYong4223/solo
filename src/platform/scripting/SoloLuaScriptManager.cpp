#include <vector>
#include <map>
#include "SoloLuaScriptManager.h"
#include "SoloLuaScriptComponent.h"
#include "SoloLuaEngineCallback.h"
#include "SoloEngine.h"
#include "SoloDevice.h"
#include "SoloScene.h"
#include "SoloResourceManager.h"
#include "SoloFileSystem.h"
#include "SoloNode.h"
#include "SoloVector2.h"
#include "SoloVector3.h"
#include "SoloVector4.h"
#include "SoloQuaternion.h"
#include "SoloMatrix.h"
#include "SoloPlane.h"
#include "SoloFrustum.h"
#include "SoloBoundingBox.h"
#include "SoloBoundingSphere.h"
#include "SoloRay.h"
#include "SoloTransform.h"
#include "SoloCamera.h"
#include "SoloRenderTarget.h"

#define REGISTER_VARIABLE(binding, klass, name) binding.addVariable(#name, &klass::name, true)
#define REGISTER_METHOD(binding, klass, name) binding.addFunction(#name, &klass::name)
#define REGISTER_METHOD_RENAMED(binding, klass, name, nameStr) binding.addFunction(nameStr, &klass::name)
#define REGISTER_OVERLOADED_METHOD(binding, klass, name, nameStr, resultType, modifier, ...) \
	binding.addFunction(nameStr, static_cast<resultType(klass::*)(__VA_ARGS__)modifier>(&klass::name))
#define REGISTER_STATIC_METHOD(binding, klass, name) binding.addStaticFunction(#name, &klass::name)
#define REGISTER_STATIC_OVERLOADED_METHOD(binding, klass, name, nameStr, resultType, modifier, ...) \
	binding.addStaticFunction(nameStr, static_cast<resultType(*)(__VA_ARGS__)modifier>(&klass::name))

namespace LuaIntf
{
	LUA_USING_SHARED_PTR_TYPE(std::shared_ptr)
	LUA_USING_LIST_TYPE(std::vector)
	LUA_USING_MAP_TYPE(std::map)
}


using namespace solo;
using namespace LuaIntf;


LuaScriptManager::LuaScriptManager()
{
	lua = LuaState::newState();
	lua.openLibs();
	registerApi();
}


LuaScriptManager::~LuaScriptManager()
{
	lua.close();
}


void registerVector3(CppBindModule& module)
{
	auto vector3 = module.beginClass<Vector3>("Vector3");
	vector3.addConstructor(LUA_ARGS(float, float, float));
	REGISTER_STATIC_OVERLOADED_METHOD(vector3, Vector3, dot, "dot", float, , const Vector3&, const Vector3&);
	REGISTER_VARIABLE(vector3, Vector3, x);
	REGISTER_VARIABLE(vector3, Vector3, y);
	REGISTER_VARIABLE(vector3, Vector3, z);
	REGISTER_STATIC_METHOD(vector3, Vector3, zero);
	REGISTER_STATIC_METHOD(vector3, Vector3, unit);
	REGISTER_STATIC_METHOD(vector3, Vector3, unitX);
	REGISTER_STATIC_METHOD(vector3, Vector3, unitY);
	REGISTER_STATIC_METHOD(vector3, Vector3, unitZ);
	REGISTER_STATIC_METHOD(vector3, Vector3, angle);
	REGISTER_STATIC_METHOD(vector3, Vector3, cross);
	REGISTER_OVERLOADED_METHOD(vector3, Vector3, dot, "dot", float, const, const Vector3&);
	REGISTER_METHOD(vector3, Vector3, distance);
	REGISTER_METHOD(vector3, Vector3, distanceSquared);
	REGISTER_METHOD(vector3, Vector3, clamp);
	REGISTER_METHOD(vector3, Vector3, length);
	REGISTER_METHOD(vector3, Vector3, lengthSquared);
	REGISTER_METHOD(vector3, Vector3, isUnit);
	REGISTER_METHOD(vector3, Vector3, isZero);
	REGISTER_METHOD(vector3, Vector3, normalize);
	REGISTER_METHOD(vector3, Vector3, normalized);
	REGISTER_METHOD(vector3, Vector3, normalized);
	REGISTER_OVERLOADED_METHOD(vector3, Vector3, operator+, "plusScalar", Vector3, const, float);
	REGISTER_OVERLOADED_METHOD(vector3, Vector3, operator+, "plusVector3", Vector3, const, const Vector3&);
	REGISTER_OVERLOADED_METHOD(vector3, Vector3, operator+=, "addScalar", Vector3&,, float);
	REGISTER_OVERLOADED_METHOD(vector3, Vector3, operator+=, "addVector3", Vector3&,, const Vector3&);
	vector3.endClass();
}


void registerVector2(CppBindModule& module)
{
	auto vector2 = module.beginClass<Vector2>("Vector2");
	vector2.addConstructor(LUA_ARGS(float, float));
	REGISTER_STATIC_OVERLOADED_METHOD(vector2, Vector2, dot, "dot", float, , const Vector2&, const Vector2&);
	REGISTER_STATIC_METHOD(vector2, Vector2, zero);
	REGISTER_STATIC_METHOD(vector2, Vector2, unit);
	REGISTER_STATIC_METHOD(vector2, Vector2, unitX);
	REGISTER_STATIC_METHOD(vector2, Vector2, unitY);
	REGISTER_STATIC_METHOD(vector2, Vector2, angle);
	REGISTER_VARIABLE(vector2, Vector2, x);
	REGISTER_VARIABLE(vector2, Vector2, y);
	REGISTER_METHOD(vector2, Vector2, distance);
	REGISTER_METHOD(vector2, Vector2, distanceSquared);
	REGISTER_OVERLOADED_METHOD(vector2, Vector2, dot, "dot", float, const, const Vector2&);
	REGISTER_METHOD(vector2, Vector2, clamp);
	REGISTER_METHOD(vector2, Vector2, length);
	REGISTER_METHOD(vector2, Vector2, lengthSquared);
	REGISTER_METHOD(vector2, Vector2, isUnit);
	REGISTER_METHOD(vector2, Vector2, isZero);
	REGISTER_METHOD(vector2, Vector2, normalize);
	REGISTER_METHOD(vector2, Vector2, normalized);
	REGISTER_OVERLOADED_METHOD(vector2, Vector2, operator+, "plusScalar", Vector2, const, float);
	REGISTER_OVERLOADED_METHOD(vector2, Vector2, operator+, "plusVector2", Vector2, const, const Vector2&);
	REGISTER_OVERLOADED_METHOD(vector2, Vector2, operator+=, "addScalar", Vector2&,, float);
	REGISTER_OVERLOADED_METHOD(vector2, Vector2, operator+=, "addVector2", Vector2&,, const Vector2&);
	vector2.endClass();
}

void registerVector4(CppBindModule& module)
{
	module.beginClass<Vector4>("Vector4")
		.addConstructor(LUA_ARGS(float, float, float, float))
		.addStaticFunction("zero", &Vector4::zero)
		.addStaticFunction("unit", &Vector4::unit)
		.addStaticFunction("unitX", &Vector4::unitX)
		.addStaticFunction("unitY", &Vector4::unitY)
		.addStaticFunction("unitZ", &Vector4::unitZ)
		.addStaticFunction("unitW", &Vector4::unitW)
		.addStaticFunction("angle", &Vector4::angle)
		.addStaticFunction("dot", static_cast<float(*)(const Vector4&, const Vector4&)>(&Vector4::dot))
		.addFunction("distance", &Vector4::distance)
		.addFunction("distanceSquared", &Vector4::distanceSquared)
		.addFunction("dot", static_cast<float(Vector4::*)(const Vector4&)const>(&Vector4::dot))
		.addFunction("clamp", &Vector4::clamp)
		.addFunction("length", &Vector4::length)
		.addFunction("lengthSquared", &Vector4::lengthSquared)
		.addFunction("isUnit", &Vector4::isUnit)
		.addFunction("isZero", &Vector4::isZero)
		.addFunction("normalize", &Vector4::normalize)
		.addFunction("normalized", &Vector4::normalized)
		.addVariable("x", &Vector4::x, true)
		.addVariable("y", &Vector4::y, true)
		.addVariable("z", &Vector4::z, true)
		.addVariable("w", &Vector4::w, true)
		.addFunction("plusScalar", static_cast<Vector4(Vector4::*)(float)const>(&Vector4::operator+))
		.addFunction("plusVector4", static_cast<Vector4(Vector4::*)(const Vector4&)const>(&Vector4::operator+))
		.addFunction("addScalar", static_cast<Vector4&(Vector4::*)(float)>(&Vector4::operator+=))
		.addFunction("addVector4", static_cast<Vector4&(Vector4::*)(const Vector4&)>(&Vector4::operator+=))
	.endClass();
}


void registerMatrix(CppBindModule& module)
{
	module.beginClass<Matrix>("Matrix")
		.addConstructor(LUA_ARGS())
		.addStaticFunction("identity", &Matrix::identity)
		.addStaticFunction("zero", &Matrix::zero)
		.addStaticFunction("createLookAt", static_cast<Matrix(*)(const Vector3&, const Vector3&, const Vector3&)>(&Matrix::createLookAt))
		.addStaticFunction("createLookAt2", static_cast<Matrix(*)(float, float, float, float, float, float, float, float, float)>(&Matrix::createLookAt))
		.addStaticFunction("createPerspective", &Matrix::createPerspective)
		.addStaticFunction("createOrthographic", &Matrix::createOrthographic)
		.addStaticFunction("createOrthographicOffCenter", &Matrix::createOrthographicOffCenter)
		.addStaticFunction("createBillboard", static_cast<Matrix(*)(const Vector3&, const Vector3&, const Vector3&)>(&Matrix::createBillboard))
		.addStaticFunction("createBillboard2", static_cast<Matrix(*)(const Vector3&, const Vector3&, const Vector3&, const Vector3&)>(&Matrix::createBillboard))
		.addStaticFunction("createReflection", &Matrix::createReflection)
		.addStaticFunction("createScale", static_cast<Matrix(*)(const Vector3&)>(&Matrix::createScale))
		.addStaticFunction("createScale2", static_cast<Matrix(*)(float, float, float)>(&Matrix::createScale))
		.addStaticFunction("createRotation", static_cast<Matrix(*)(const Quaternion&)>(&Matrix::createRotation))
		.addStaticFunction("createRotationAxisAngle", static_cast<Matrix(*)(const Vector3&, float)>(&Matrix::createRotation))
		.addStaticFunction("createRotationX", &Matrix::createRotationX)
		.addStaticFunction("createRotationY", &Matrix::createRotationY)
		.addStaticFunction("createRotationZ", &Matrix::createRotationZ)
		.addStaticFunction("createTranslation", static_cast<Matrix(*)(const Vector3&)>(&Matrix::createTranslation))
		.addStaticFunction("createTranslation2", static_cast<Matrix(*)(float, float, float)>(&Matrix::createTranslation))
		.addFunction("decompose", &Matrix::decompose)
		.addFunction("getDeterminant", &Matrix::getDeterminant)
		.addFunction("getScale", &Matrix::getScale)
		.addFunction("getRotation", &Matrix::getRotation)
		.addFunction("getTranslation", &Matrix::getTranslation)
		.addFunction("getUpVector", &Matrix::getUpVector)
		.addFunction("getDownVector", &Matrix::getDownVector)
		.addFunction("getLeftVector", &Matrix::getLeftVector)
		.addFunction("getRightVector", &Matrix::getRightVector)
		.addFunction("getForwardVector", &Matrix::getForwardVector)
		.addFunction("getBackVector", &Matrix::getBackVector)
		.addFunction("invert", &Matrix::invert)
		.addFunction("transpose", &Matrix::transpose)
		.addFunction("rotate", static_cast<void(Matrix::*)(const Quaternion&)>(&Matrix::rotate))
		.addFunction("rotateAxisAngle", static_cast<void(Matrix::*)(const Vector3&, float)>(&Matrix::rotate))
		.addFunction("rotateX", &Matrix::rotateX)
		.addFunction("rotateY", &Matrix::rotateY)
		.addFunction("rotateZ", &Matrix::rotateZ)
		.addFunction("scale", static_cast<void(Matrix::*)(float)>(&Matrix::scale))
		.addFunction("scale2", static_cast<void(Matrix::*)(float, float, float)>(&Matrix::scale))
		.addFunction("scale3", static_cast<void(Matrix::*)(const Vector3&)>(&Matrix::scale))
		.addFunction("translate", static_cast<void(Matrix::*)(float, float, float)>(&Matrix::translate))
		.addFunction("translate2", static_cast<void(Matrix::*)(const Vector3&)>(&Matrix::translate))
		.addFunction("setIdentity", &Matrix::setIdentity)
		.addFunction("setZero", &Matrix::setZero)
		.addFunction("transformPoint", &Matrix::transformPoint)
		.addFunction("transformVector3Direction", static_cast<Vector3(Matrix::*)(const Vector3&)const>(&Matrix::transformDirection))
		.addFunction("transformVector4Direction", static_cast<Vector4(Matrix::*)(const Vector4&)const>(&Matrix::transformDirection))
		.addFunction("plusScalar", static_cast<Matrix(Matrix::*)(float)const>(&Matrix::operator+))
		.addFunction("plusMatrix", static_cast<Matrix(Matrix::*)(const Matrix&)const>(&Matrix::operator+))
		.addFunction("addScalar", static_cast<Matrix&(Matrix::*)(float)>(&Matrix::operator+=))
		.addFunction("addMatrix", static_cast<Matrix&(Matrix::*)(const Matrix&)>(&Matrix::operator+=))
		.addFunction("negative", static_cast<Matrix(Matrix::*)()const>(&Matrix::operator-))
		.addFunction("minusScalar", static_cast<Matrix(Matrix::*)(float)const>(&Matrix::operator-))
		.addFunction("minusMatrix", static_cast<Matrix(Matrix::*)(const Matrix&)const>(&Matrix::operator-))
		.addFunction("substractScalar", static_cast<Matrix&(Matrix::*)(float)>(&Matrix::operator-=))
		.addFunction("substractMatrix", static_cast<Matrix&(Matrix::*)(const Matrix&)>(&Matrix::operator-=))
		.addFunction("productScalar", static_cast<Matrix(Matrix::*)(float)const>(&Matrix::operator*))
		.addFunction("productMatrix", static_cast<Matrix(Matrix::*)(const Matrix&)const>(&Matrix::operator*))
		.addFunction("multScalar", static_cast<Matrix&(Matrix::*)(float)>(&Matrix::operator*=))
		.addFunction("multMatrix", static_cast<Matrix&(Matrix::*)(const Matrix&)>(&Matrix::operator*=))
	.endClass();
}


void registerQuaternion(CppBindModule& module)
{
	module.beginClass<Quaternion>("Quaternion")
		.addConstructor(LUA_ARGS())
		.addStaticFunction("zero", &Quaternion::zero)
		.addStaticFunction("identity", &Quaternion::identity)
		.addStaticFunction("createFromAxisAngle", &Quaternion::createFromAxisAngle)
		.addStaticFunction("createFromRotationMatrix", &Quaternion::createFromRotationMatrix)
		.addStaticFunction("lerp", &Quaternion::lerp)
		.addStaticFunction("slerp", &Quaternion::slerp)
		.addStaticFunction("squad", &Quaternion::squad)
		.addFunction("isIdentity", &Quaternion::isIdentity)
		.addFunction("isZero", &Quaternion::isZero)
		.addFunction("conjugate", &Quaternion::conjugate)
		.addFunction("inverse", &Quaternion::inverse)
		.addFunction("normalized", &Quaternion::normalized)
		.addFunction("normalize", &Quaternion::normalize)
		.addFunction("toAxisAngle", &Quaternion::toAxisAngle)
		.addFunction("product", &Quaternion::operator*)
		.addFunction("mult", &Quaternion::operator*=)
		.addVariable("x", &Quaternion::x)
		.addVariable("y", &Quaternion::y)
		.addVariable("z", &Quaternion::z)
		.addVariable("w", &Quaternion::w)
	.endClass();
}


void registerPlane(CppBindModule& module)
{
	module.beginClass<Plane>("Plane")
		.addConstructor(LUA_ARGS(const Vector3&, float))
		.addConstant("Intersection_Intersecting", PlaneIntersection::Intersecting)
		.addConstant("Intersection_Front", PlaneIntersection::Front)
		.addConstant("Intersection_Back", PlaneIntersection::Back)
		.addStaticFunction("intersection", &Plane::intersection)
		.addFunction("getNormal", &Plane::getNormal)
		.addFunction("setNormal", static_cast<void(Plane::*)(const Vector3&)>(&Plane::setNormal))
		.addFunction("getDistance", &Plane::getDistance)
		.addFunction("getDistanceToPoint", &Plane::getDistanceToPoint)
		.addFunction("setDistance", &Plane::setDistance)
		.addFunction("isParallel", &Plane::isParallel)
		.addFunction("transform", &Plane::transform)
	.endClass();
}


void registerFrustum(CppBindModule& module)
{
	module.beginClass<Frustum>("Frustum")
		.addConstructor(LUA_ARGS())
		.addFunction("getNearPlane", &Frustum::getNearPlane)
		.addFunction("getFarPlane", &Frustum::getFarPlane)
		.addFunction("getLeftPlane", &Frustum::getLeftPlane)
		.addFunction("getRightPlane", &Frustum::getRightPlane)
		.addFunction("getTopPlane", &Frustum::getTopPlane)
		.addFunction("getBottomPlane", &Frustum::getBottomPlane)
		.addFunction("getMatrix", &Frustum::getMatrix)
		.addFunction("setMatrix", &Frustum::setMatrix)
		.addFunction("getCorners", &Frustum::getCorners)
		.addFunction("getNearCorners", &Frustum::getNearCorners)
		.addFunction("getFarCorners", &Frustum::getFarCorners)
		.addFunction("intersectsPoint", &Frustum::intersectsPoint)
		.addFunction("intersectsBoundingBox", &Frustum::intersectsBoundingBox)
		.addFunction("intersectsBoundingSphere", &Frustum::intersectsBoundingSphere)
		.addFunction("intersectsRay", &Frustum::intersectsRay)
		.addFunction("getPlaneIntersection", &Frustum::getPlaneIntersection)
	.endClass();
}


void registerBoundingBox(CppBindModule& module)
{
	module.beginClass<BoundingBox>("BoundingBox")
		.addConstructor(LUA_ARGS(const Vector3&, const Vector3&))
		.addStaticFunction("empty", &BoundingBox::empty)
		.addFunction("getCenter", &BoundingBox::getCenter)
		.addFunction("getCorners", &BoundingBox::getCorners)
		.addFunction("intersectsBoundingSphere", &BoundingBox::intersectsBoundingSphere)
		.addFunction("intersectsBoundingBox", &BoundingBox::intersectsBoundingBox)
		.addFunction("intersectsFrustum", &BoundingBox::intersectsFrustum)
		.addFunction("getRayIntersection", &BoundingBox::getRayIntersection)
		.addFunction("getPlaneIntersection", &BoundingBox::getPlaneIntersection)
		.addFunction("isEmpty", &BoundingBox::isEmpty)
		.addFunction("mergeBoundingBox", &BoundingBox::mergeBoundingBox)
		.addFunction("mergeBoundingSphere", &BoundingBox::mergeBoundingSphere)
		.addFunction("transform", &BoundingBox::transform)
		.addVariable("min", &BoundingBox::min)
		.addVariable("max", &BoundingBox::max)
	.endClass();
}


void registerRay(CppBindModule& module)
{
	module.beginClass<Ray>("Ray")
		.addConstructor(LUA_ARGS(const Vector3&, const Vector3&))
	.endClass();
}


void registerBoundingSphere(CppBindModule& module)
{
	module.beginClass<BoundingSphere>("BoundingSphere")
		.addConstructor(LUA_ARGS(const Vector3&, float))
		.addStaticFunction("empty", &BoundingSphere::empty)
		.addFunction("intersectsBoundingSphere", &BoundingSphere::intersectsBoundingSphere)
		.addFunction("intersectsBoundingBox", &BoundingSphere::intersectsBoundingBox)
		.addFunction("intersectsFrustum", &BoundingSphere::intersectsFrustum)
		.addFunction("getRayIntersection", &BoundingSphere::getRayIntersection)
		.addFunction("getPlaneIntersection", &BoundingSphere::getPlaneIntersection)
		.addFunction("isEmpty", &BoundingSphere::isEmpty)
		.addFunction("mergeBoundingBox", &BoundingSphere::mergeBoundingBox)
		.addFunction("mergeBoundingSphere", &BoundingSphere::mergeBoundingSphere)
		.addFunction("transform", &BoundingSphere::transform)
		.addVariable("center", &BoundingSphere::center)
		.addVariable("radius", &BoundingSphere::radius)
	.endClass();
}


void registerDevice(CppBindModule& module)
{
	module
		.addConstant("KeyCode_A", KeyCode::A)
		.addConstant("KeyCode_B", KeyCode::B)
		.addConstant("KeyCode_C", KeyCode::C)
		.addConstant("KeyCode_D", KeyCode::D)
		.addConstant("KeyCode_E", KeyCode::E)
		.addConstant("KeyCode_F", KeyCode::F)
		.addConstant("KeyCode_G", KeyCode::G)
		.addConstant("KeyCode_H", KeyCode::H)
		.addConstant("KeyCode_I", KeyCode::I)
		.addConstant("KeyCode_J", KeyCode::J)
		.addConstant("KeyCode_K", KeyCode::K)
		.addConstant("KeyCode_L", KeyCode::L)
		.addConstant("KeyCode_M", KeyCode::M)
		.addConstant("KeyCode_N", KeyCode::N)
		.addConstant("KeyCode_O", KeyCode::O)
		.addConstant("KeyCode_P", KeyCode::P)
		.addConstant("KeyCode_Q", KeyCode::Q)
		.addConstant("KeyCode_R", KeyCode::R)
		.addConstant("KeyCode_S", KeyCode::S)
		.addConstant("KeyCode_T", KeyCode::T)
		.addConstant("KeyCode_U", KeyCode::U)
		.addConstant("KeyCode_V", KeyCode::V)
		.addConstant("KeyCode_W", KeyCode::W)
		.addConstant("KeyCode_X", KeyCode::X)
		.addConstant("KeyCode_Y", KeyCode::Y)
		.addConstant("KeyCode_Z", KeyCode::Z)
		.addConstant("KeyCode_LeftArrow", KeyCode::LeftArrow)
		.addConstant("KeyCode_RightArrow", KeyCode::RightArrow)
		.addConstant("KeyCode_UpArrow", KeyCode::UpArrow)
		.addConstant("KeyCode_DownArrow", KeyCode::DownArrow)
		.addConstant("KeyCode_Escape", KeyCode::Escape);

	module
		.addConstant("MouseButton_Left", MouseButton::Left)
		.addConstant("MouseButton_Middle", MouseButton::Middle)
		.addConstant("MouseButton_Right", MouseButton::Right);

	module.beginClass<Device>("Device")
		.addFunction("getWindowTitle", &Device::getWindowTitle)
		.addFunction("setWindowTitle", &Device::setWindowTitle)
		.addFunction("setCursorCaptured", &Device::setCursorCaptured)
		.addFunction("getCanvasSize", &Device::getCanvasSize)
		.addFunction("getLifetime", &Device::getLifetime)
		.addFunction("isKeyPressed", &Device::isKeyPressed)
		.addFunction("isKeyReleased", &Device::isKeyReleased)
		.addFunction("getMouseMotion", &Device::getMouseMotion)
		.addFunction("isMouseButtonDown", &Device::isMouseButtonDown)
		.addFunction("isMouseButtonReleased", &Device::isMouseButtonReleased)
		.addFunction("getTimeDelta", &Device::getTimeDelta)
		.addFunction("requestShutdown", &Device::requestShutdown)
		.addFunction("shutdownRequested", &Device::shutdownRequested)
	.endClass();
}


void registerResourceManager(CppBindModule& module)
{
	module.beginClass<ResourceManager>("ResourceManager")
		// TODO
	.endClass();
}


void registerNode(CppBindModule& module)
{
	module.beginClass<Node>("Node")
		.addFunction("getScene", &Node::getScene)
		.addFunction("getId", &Node::getId)
		.addFunction("findComponent", LuaScriptComponent::getFindComponentFunc(module.state()))
		.addFunction("findStandardComponent", &LuaScriptComponent::findStandardComponent)
		.addFunction("addComponent", &LuaScriptComponent::addComponent)
		.addFunction("addStandardComponent", &LuaScriptComponent::addStandardComponent)
		.addFunction("removeComponent", &LuaScriptComponent::removeComponent)
		.addFunction("removeAllComponents", &Node::removeAllComponents)
	.endClass();
}


void registerComponent(CppBindModule& module)
{
	module.beginClass<Component>("Component")
		// TODO
	.endClass();
}


void registerRenderTarget(CppBindModule& module)
{
	module.beginClass<RenderTarget>("RenderTarget")
	// TODO
	.endClass();
}


void registerCamera(CppBindModule& module)
{
	auto camera = module.beginExtendClass<Camera, Component>("Camera");
	REGISTER_METHOD(camera, Camera, getRenderTarget);
	REGISTER_METHOD(camera, Camera, setRenderTarget);
	REGISTER_METHOD(camera, Camera, setClearColor);
	REGISTER_METHOD(camera, Camera, getViewport);
	REGISTER_METHOD(camera, Camera, setViewport);
	REGISTER_METHOD(camera, Camera, resetViewport);
	REGISTER_METHOD(camera, Camera, setPerspective);
	REGISTER_METHOD(camera, Camera, isPerspective);
	REGISTER_METHOD(camera, Camera, getNear);
	REGISTER_METHOD(camera, Camera, getFar);
	REGISTER_METHOD(camera, Camera, getFOV);
	REGISTER_METHOD(camera, Camera, getWidth);
	REGISTER_METHOD(camera, Camera, getHeight);
	REGISTER_METHOD(camera, Camera, getAspectRatio);
	camera.endClass();
}


void registerTransform(CppBindModule& module)
{
	module
		.addConstant("TransformSpace_Parent", TransformSpace::Parent)
		.addConstant("TransformSpace_Self", TransformSpace::Self)
		.addConstant("TransformSpace_World", TransformSpace::World);

	auto transform = module.beginExtendClass<Transform, Component>("Transform");
	REGISTER_METHOD(transform, Transform, getParent);
	REGISTER_METHOD(transform, Transform, setParent);
	REGISTER_METHOD(transform, Transform, getChild);
	REGISTER_METHOD(transform, Transform, getChildrenCount);
	REGISTER_METHOD(transform, Transform, removeChildren);
	REGISTER_METHOD(transform, Transform, getWorldScale);
	REGISTER_METHOD(transform, Transform, getLocalScale);
	REGISTER_METHOD(transform, Transform, getWorldRotation);
	REGISTER_METHOD(transform, Transform, getLocalRotation);
	REGISTER_METHOD(transform, Transform, getWorldPosition);
	REGISTER_METHOD(transform, Transform, getLocalPosition);
	REGISTER_METHOD(transform, Transform, getLocalUp);
	REGISTER_METHOD(transform, Transform, getLocalDown);
	REGISTER_METHOD(transform, Transform, getLocalLeft);
	REGISTER_METHOD(transform, Transform, getLocalRight);
	REGISTER_METHOD(transform, Transform, getLocalForward);
	REGISTER_METHOD(transform, Transform, getLocalBack);
	REGISTER_METHOD(transform, Transform, translateLocal);
		
	transform.addFunction("rotate", static_cast<void(Transform::*)(const Quaternion&, TransformSpace)>(&Transform::rotate))
		.addFunction("rotateAxisAngle", static_cast<void(Transform::*)(const Vector3&, float, TransformSpace)>(&Transform::rotate))
		.addFunction("scaleLocal", &Transform::scaleLocal)
		.addFunction("setLocalPosition", &Transform::setLocalPosition)
		.addFunction("setLocalRotation", static_cast<void(Transform::*)(const Quaternion&)>(&Transform::setLocalRotation))
		.addFunction("setLocalRotationAxisAngle", static_cast<void(Transform::*)(const Vector3&, float)>(&Transform::setLocalRotation))
		.addFunction("setLocalScale", &Transform::setLocalScale)
		.addFunction("lookAt", &Transform::lookAt)
		.addFunction("getMatrix", &Transform::getMatrix)
		.addFunction("getWorldMatrix", &Transform::getWorldMatrix)
		.addFunction("getWorldViewMatrix", &Transform::getWorldViewMatrix)
		.addFunction("getWorldViewProjectionMatrix", &Transform::getWorldViewProjectionMatrix)
		.addFunction("getInverseTransposedWorldViewMatrix", &Transform::getInverseTransposedWorldViewMatrix)
		.addFunction("getInverseTransposedWorldMatrix", &Transform::getInverseTransposedWorldMatrix)
		.addFunction("transformPoint", &Transform::transformPoint)
		.addFunction("transformDirection", &Transform::transformDirection)
	.endClass();
}


void registerScene(CppBindModule& module)
{
	module.beginClass<Scene>("Scene")
		.addFunction("createNode", &Scene::createNode)
	.endClass();
}


void registerFileSystem(CppBindModule& module)
{
	module.beginClass<FileSystem>("FileSystem")
		// TODO
	.endClass();
}


void registerEngine(CppBindModule& module)
{
	module
		.addConstant("EngineMode_Stub", EngineMode::Stub)
		.addConstant("EngineMode_OpenGL", EngineMode::OpenGL);

	module.beginClass<EngineCreationArgs>("EngineCreationArgs")
		.addConstructor(LUA_ARGS(_opt<EngineMode>, _opt<int>, _opt<int>, _opt<int>, _opt<int>, _opt<bool>, _opt<std::string>))
		.addVariable("mode", &EngineCreationArgs::mode, true)
		.addVariable("bits", &EngineCreationArgs::bits, true)
		.addVariable("canvasHeight", &EngineCreationArgs::canvasHeight, true)
		.addVariable("canvasWidth", &EngineCreationArgs::canvasWidth, true)
		.addVariable("depth", &EngineCreationArgs::depth, true)
		.addVariable("fullScreen", &EngineCreationArgs::fullScreen, true)
		.addVariable("windowTitle", &EngineCreationArgs::windowTitle, true)
	.endClass();

	module.beginClass<Engine>("Engine")
		.addStaticFunction("create", &Engine::create)
		.addFunction("getDevice", &Engine::getDevice)
		.addFunction("getScene", &Engine::getScene)
		.addFunction("getResourceManager", &Engine::getResourceManager)
		.addFunction("getFileSystem", &Engine::getFileSystem)
		.addFunction("setCallback", &LuaEngineCallback::setCallback)
		.addFunction("getMode", &Engine::getMode)
		.addFunction("run", &Engine::run)
	.endClass();
}


void LuaScriptManager::registerApi()
{
	auto module = LuaBinding(lua).beginModule("solo");
	registerVector2(module);
	registerVector3(module);
	registerVector4(module);
	registerRay(module);
	registerBoundingBox(module);
	registerBoundingSphere(module);
	registerFrustum(module);
	registerQuaternion(module);
	registerPlane(module);
	registerMatrix(module);
	registerDevice(module);
	registerResourceManager(module);
	registerNode(module);
	registerComponent(module);
	registerTransform(module);
	registerCamera(module);
	registerScene(module);
	registerFileSystem(module);
	registerEngine(module);
	module.endModule();
}


void LuaScriptManager::execute(const std::string& code)
{
	lua.doString(code.c_str());
}


void LuaScriptManager::executeFile(const std::string& path)
{
	lua.doFile(path.c_str());
}