/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "SoloMatrix.h"
#include "SoloMath.h"
#include "SoloRay.h"
#include "SoloCommon.h"
#include "SoloQuaternion.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace solo;

Matrix::Matrix()
{
    *this = identity();
}

Matrix::Matrix(const glm::mat4x4 &data):
	data(data)
{
}

Matrix::Matrix(
    float m11, float m12, float m13, float m14,
    float m21, float m22, float m23, float m24,
    float m31, float m32, float m33, float m34,
    float m41, float m42, float m43, float m44)
{
    data[0][0] = m11;
    data[0][1] = m21;
    data[0][2] = m31;
    data[0][3] = m41;
    data[1][0] = m12;
    data[1][1] = m22;
    data[1][2] = m32;
    data[1][3] = m42;
    data[2][0] = m13;
    data[2][1] = m23;
    data[2][2] = m33;
    data[2][3] = m43;
    data[3][0] = m14;
    data[3][1] = m24;
    data[3][2] = m34;
    data[3][3] = m44;
}

Matrix::Matrix(const Matrix &copy)
{
	data = copy.data;
}

auto Matrix::identity() -> Matrix
{
    static Matrix m(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
    return m;
}

auto Matrix::getDeterminant() const -> float
{
	return glm::determinant(data);
}

void Matrix::invert()
{
	data = glm::inverse(data);
}

bool Matrix::isIdentity() const
{
	return
		glm::all(glm::epsilonEqual(data[0], {1.0f, 0.0f, 0.0f, 0.0}, glm::epsilon<float>())) &&
		glm::all(glm::epsilonEqual(data[1], {0.0f, 1.0f, 0.0f, 0.0}, glm::epsilon<float>())) &&
		glm::all(glm::epsilonEqual(data[2], {0.0f, 0.0f, 1.0f, 0.0}, glm::epsilon<float>())) &&
		glm::all(glm::epsilonEqual(data[3], {0.0f, 0.0f, 0.0f, 1.0}, glm::epsilon<float>()));
}

auto Matrix::createLookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up) -> Matrix
{
    const auto zaxis = (eye - target).normalized();
    const auto xaxis = up.normalized().cross(zaxis).normalized();
    const auto yaxis = zaxis.cross(xaxis).normalized();

    // Matrix is built already transposed
    return Matrix(
        xaxis.x(), yaxis.x(), zaxis.x(), 0,
        xaxis.y(), yaxis.y(), zaxis.y(), 0,
        xaxis.z(), yaxis.z(), zaxis.z(), 0,
        -xaxis.dot(eye), -yaxis.dot(eye), -zaxis.dot(eye), 1
    );
}

auto Matrix::createPerspective(const Radians &fieldOfView, float aspectRatio, float znear, float zfar) -> Matrix
{
	return glm::perspective(fieldOfView.toRawRadians(), aspectRatio, znear, zfar);
}

auto Matrix::createOrthographic(float width, float height, float near, float far) -> Matrix
{
    const auto halfWidth = width / 2.0f;
    const auto halfHeight = height / 2.0f;
    const auto left = -halfWidth;
    const auto right = halfWidth;
    const auto top = halfHeight;
    const auto bottom = -halfHeight;
	return glm::ortho(left, right, bottom, top, near, far);
}

auto Matrix::createScale(const Vector3 &scale) -> Matrix
{
    Matrix result;
    result.data[0][0] = scale.x();
    result.data[1][1] = scale.y();
    result.data[2][2] = scale.z();
    return result;
}

auto Matrix::createRotationFromQuaternion(const Quaternion &q) -> Matrix
{
    const auto x2 = q.x() + q.x();
    const auto y2 = q.y() + q.y();
    const auto z2 = q.z() + q.z();

    const auto xx2 = q.x() * x2;
    const auto yy2 = q.y() * y2;
    const auto zz2 = q.z() * z2;
    const auto xy2 = q.x() * y2;
    const auto xz2 = q.x() * z2;
    const auto yz2 = q.y() * z2;
    const auto wx2 = q.w() * x2;
    const auto wy2 = q.w() * y2;
    const auto wz2 = q.w() * z2;

    Matrix result;

    result.data[0][0] = 1.0f - yy2 - zz2;
    result.data[0][1] = xy2 + wz2;
    result.data[0][2] = xz2 - wy2;
    result.data[0][3] = 0.0f;

    result.data[1][0] = xy2 - wz2;
    result.data[1][1] = 1.0f - xx2 - zz2;
    result.data[1][2] = yz2 + wx2;
    result.data[1][3] = 0.0f;

    result.data[2][0] = xz2 + wy2;
    result.data[2][1] = yz2 - wx2;
    result.data[2][2] = 1.0f - xx2 - yy2;
    result.data[2][3] = 0.0f;

    result.data[3][0] = 0.0f;
    result.data[3][1] = 0.0f;
    result.data[3][2] = 0.0f;
    result.data[3][3] = 1.0f;

    return result;
}

auto Matrix::createRotationFromAxisAngle(const Vector3 &axis, const Radians &angle) -> Matrix
{
    auto x = axis.x();
    auto y = axis.y();
    auto z = axis.z();

    // Make sure the input axis is normalized
    auto n = x * x + y * y + z * z;
    if (n != 1.0f)
    {
        // Not normalized
        n = std::sqrt(n);
        // Prevent division too close to zero.
        if (!math::isZero(n))
        {
            n = 1.0f / n;
            x *= n;
            y *= n;
            z *= n;
        }
    }

    const auto c = std::cos(angle.toRawRadians());
    const auto s = std::sin(angle.toRawRadians());

    const auto t = 1.0f - c;
    const auto tx = t * x;
    const auto ty = t * y;
    const auto tz = t * z;
    const auto txy = tx * y;
    const auto txz = tx * z;
    const auto tyz = ty * z;
    const auto sx = s * x;
    const auto sy = s * y;
    const auto sz = s * z;

    Matrix result;

    result.data[0][0] = c + tx * x;
    result.data[0][1] = txy + sz;
    result.data[0][2] = txz - sy;
    result.data[0][3] = 0.0f;

    result.data[1][0] = txy - sz;
    result.data[1][1] = c + ty * y;
    result.data[1][2] = tyz + sx;
    result.data[1][3] = 0.0f;

    result.data[2][0] = txz + sy;
    result.data[2][1] = tyz - sx;
    result.data[2][2] = c + tz * z;
    result.data[2][3] = 0.0f;

    result.data[3][0] = 0.0f;
    result.data[3][1] = 0.0f;
    result.data[3][2] = 0.0f;
    result.data[3][3] = 1.0f;

    return result;
}

auto Matrix::createTranslation(const Vector3 &translation) -> Matrix
{
    Matrix result;
    result.data[3][0] = translation.x();
    result.data[3][1] = translation.y();
    result.data[3][2] = translation.z();
    return result;
}

auto Matrix::columns() const -> const float*
{
	return glm::value_ptr(data);
}

auto Matrix::getScale() const -> Vector3
{
    Vector3 result;
    decompose(&result, nullptr, nullptr);
    return result;
}

auto Matrix::getRotation() const -> Quaternion
{
    Quaternion result;
    decompose(nullptr, &result, nullptr);
    return result;
}

auto Matrix::getTranslation() const -> Vector3
{
    Vector3 result;
    decompose(nullptr, nullptr, &result);
    return result;
}

auto Matrix::transformPoint(const Vector3 &point) const -> Vector3
{
    return {
        point.x() * data[0][0] + point.y() * data[1][0] + point.z() * data[2][0] + data[3][0],
		point.x() * data[0][1] + point.y() * data[1][1] + point.z() * data[2][1] + data[3][1],
		point.x() * data[0][2] + point.y() * data[1][2] + point.z() * data[2][2] + data[3][2]
    };
}

auto Matrix::transformDirection(const Vector3 &dir) const -> Vector3
{
    return {
        dir.x() * data[0][0] + dir.y() * data[1][0] + dir.z() * data[2][0],
		dir.x() * data[0][1] + dir.y() * data[1][1] + dir.z() * data[2][1],
		dir.x() * data[0][2] + dir.y() * data[1][2] + dir.z() * data[2][2]
    };
}

auto Matrix::transformRay(const Ray &ray) const -> Ray
{
    const auto origin = transformPoint(ray.getOrigin());
    const auto direction = transformDirection(ray.getDirection()).normalized();
    return {origin, direction};
}

bool Matrix::decompose(Vector3 *scale, Quaternion *rotation, Vector3 *translation) const
{
    if (translation)
    {
        translation->x() = data[3][0];
        translation->y() = data[3][1];
        translation->z() = data[3][2];
    }

    if (scale == nullptr && rotation == nullptr)
        return true;

    Vector3 xaxis(data[0][0], data[0][1], data[0][2]);
    const auto scaleX = xaxis.length();

    Vector3 yaxis(data[1][0], data[1][1], data[1][2]);
    const auto scaleY = yaxis.length();

    Vector3 zaxis(data[2][0], data[2][1], data[2][2]);
    auto scaleZ = zaxis.length();

    // Determine if we have a negative scale (true if determinant is less than zero).
    // In this case, we simply negate a single axis of the scale.
    const auto det = getDeterminant();
    if (det < 0)
        scaleZ = -scaleZ;

    if (scale)
    {
        scale->x() = scaleX;
        scale->y() = scaleY;
        scale->z() = scaleZ;
    }

    if (rotation == nullptr)
        return true;

    if (math::isZero(scaleX) || math::isZero(scaleY) || math::isZero(scaleZ))
        return false;

    auto rn = 1.0f / scaleX;
    xaxis.x() *= rn;
    xaxis.y() *= rn;
    xaxis.z() *= rn;

    rn = 1.0f / scaleY;
    yaxis.x() *= rn;
    yaxis.y() *= rn;
    yaxis.z() *= rn;

    rn = 1.0f / scaleZ;
    zaxis.x() *= rn;
    zaxis.y() *= rn;
    zaxis.z() *= rn;

    const auto trace = xaxis.x() + yaxis.y() + zaxis.z() + 1.0f;

    if (trace > FLT_EPSILON)
    {
        const auto s = 0.5f / sqrt(trace);
        rotation->w() = 0.25f / s;
        rotation->x() = (yaxis.z() - zaxis.y()) * s;
        rotation->y() = (zaxis.x() - xaxis.z()) * s;
        rotation->z() = (xaxis.y() - yaxis.x()) * s;
    }
    else
    {
        if (xaxis.x() > yaxis.y() && xaxis.x() > zaxis.z())
        {
            const auto s = 0.5f / sqrt(1.0f + xaxis.x() - yaxis.y() - zaxis.z());
            rotation->w() = (yaxis.z() - zaxis.y()) * s;
            rotation->x() = 0.25f / s;
            rotation->y() = (yaxis.x() + xaxis.y()) * s;
            rotation->z() = (zaxis.x() + xaxis.z()) * s;
        }
        else if (yaxis.y() > zaxis.z())
        {
            const auto s = 0.5f / sqrt(1.0f + yaxis.y() - xaxis.x() - zaxis.z());
            rotation->w() = (zaxis.x() - xaxis.z()) * s;
            rotation->x() = (yaxis.x() + xaxis.y()) * s;
            rotation->y() = 0.25f / s;
            rotation->z() = (zaxis.y() + yaxis.z()) * s;
        }
        else
        {
            const auto s = 0.5f / sqrt(1.0f + zaxis.z() - xaxis.x() - yaxis.y());
            rotation->w() = (xaxis.y() - yaxis.x()) * s;
            rotation->x() = (zaxis.x() + xaxis.z()) * s;
            rotation->y() = (zaxis.y() + yaxis.z()) * s;
            rotation->z() = 0.25f / s;
        }
    }

    return true;
}

auto Matrix::operator+=(float scalar) -> Matrix &
{
	data += scalar;
    return *this;
}

auto Matrix::operator+=(const Matrix &other) -> Matrix &
{
	data += other.data;
    return *this;
}

void Matrix::transpose()
{
	data = glm::transpose(data);
}

auto Matrix::operator*=(float scalar) -> Matrix &
{
	data *= scalar;
    return *this;
}

auto Matrix::operator*=(const Matrix &m2) -> Matrix &
{
	data *= m2.data;
    return *this;
}

auto Matrix::operator-=(float scalar) -> Matrix &
{
	data -= scalar;
    return *this;
}

auto Matrix::operator-=(const Matrix &m2) -> Matrix &
{
	data -= m2.data;
    return *this;
}

auto Matrix::operator-() const -> Matrix
{
	return -data;
}

void Matrix::scaleByScalar(float value)
{
    scaleByVector({value, value, value});
}

void Matrix::rotateByQuaternion(const Quaternion &q)
{
    *this *= createRotationFromQuaternion(q);
}

void Matrix::rotateByAxisAngle(const Vector3 &axis, const Radians &angle)
{
    *this *= createRotationFromAxisAngle(axis, angle);
}

void Matrix::scaleByVector(const Vector3 &s)
{
    *this *= createScale(s);
}

void Matrix::translate(const Vector3 &t)
{
    *this *= createTranslation(t);
}

auto Matrix::operator+(float scalar) const -> Matrix
{
	return data + scalar;
}

auto Matrix::operator+(const Matrix &m) const -> Matrix
{
	return data + m.data;
}

auto Matrix::operator-(float scalar) const -> Matrix
{
	return data - scalar;
}

auto Matrix::operator-(const Matrix &m) const -> Matrix
{
	return data - m.data;
}

auto Matrix::operator*(float scalar) const -> Matrix
{
	return data * scalar;
}

auto Matrix::operator*(const Matrix &m) const -> Matrix
{
	return data * m.data;
}
