#pragma once

#include "SoloBase.h"
#include "SoloComponent.h"
#include "SoloDirty.h"
#include "SoloMatrix.h"
#include "SoloVector3.h"
#include "SoloQuaternion.h"

namespace solo
{
	class Camera;

	class Transform: public ComponentBase<Transform>, Dirty
	{
	public:
		virtual ~Transform() override {}
		
		static ptr<Transform> create(size_t node);

		void addChild(ptr<Transform> child);
		void removeChild(ptr<Transform> child);
		void removeChildren();

		const Matrix& getMatrix();
		const Matrix& getWorldMatrix();
		const Matrix& getInverseTransposedWorldMatrix();

		Matrix getWorldViewMatrix(ptr<Camera> camera);
		Matrix getWorldViewProjectionMatrix(ptr<Camera> camera);
		Matrix getInverseTransposedWorldViewMatrix(ptr<Camera> camera);

	private:
		Transform(size_t node);

		ptr<Transform> _parent;
		std::list<ptr<Transform>> _children;

		Vector3 _translation;
		Vector3 _scale;
		Quaternion _rotation;
		Matrix _matrix;
		Matrix _worldMatrix;
		Matrix _inverseTransposedWorldMatrix;
		Matrix _inverseTransposedViewMatrix;
	};
}
