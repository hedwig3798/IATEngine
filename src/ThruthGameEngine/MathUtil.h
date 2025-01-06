#pragma once
#include "Headers.h"
#include "PhysicsManager.h"

namespace MathUtil
{
	physx::PxVec3 Convert(const Vector3& _val);
	physx::PxExtendedVec3 ConvertEx(const Vector3& _val);
	physx::PxVec2 Convert(const Vector2& _val);

	Vector3 Convert(const physx::PxVec3& _val);
	Vector3 Convert(const physx::PxExtendedVec3& _val);

	Quaternion Convert(const physx::PxQuat& _val);
	physx::PxQuat Convert(const Quaternion& _val);

	Matrix Convert(const physx::PxTransform& _val);
	physx::PxTransform Convert(Matrix _val);

	bool DecomposeNonSRT(Vector3& _outScale, Quaternion& _outRot, Vector3& _outPos, const Matrix& _mat);

	Vector3 GetBezjePoint(std::vector<Vector3>& _controlPoints, float _ratio);
	Vector3 GetCatmullPoint(std::vector<Vector3>& _controlPoints, float _ratio);
	Vector3 GetHermitePoint3(std::vector<Vector3>& _controlPoints, float _ratio);
};

