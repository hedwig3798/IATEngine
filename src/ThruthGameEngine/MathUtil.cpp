#include "MathUtil.h"

physx::PxVec3 MathUtil::Convert(const Vector3& _val)
{
	return physx::PxVec3(_val.x, _val.y, _val.z);
}

bool MathUtil::DecomposeNonSRT(Vector3& _outScale, Quaternion& _outRot, Vector3& _outPos, const Matrix& _mat)
{
	float def = _mat.Determinant();
	if (abs(def) <= 0.00001f)
	{
		return false;
	}
	DirectX::XMMATRIX mat = DirectX::XMMATRIX(_mat);

	_outPos = mat.r[3];

	_outScale.x = DirectX::XMVectorGetX(DirectX::XMVector3Length(mat.r[0])); // X축 스케일
	_outScale.y = DirectX::XMVectorGetY(DirectX::XMVector3Length(mat.r[1])); // Y축 스케일
	_outScale.z = DirectX::XMVectorGetZ(DirectX::XMVector3Length(mat.r[2])); // Z축 스케일

	DirectX::XMMATRIX rotationMatrix;
	rotationMatrix.r[0] = Vector3(mat.r[0]) / _outScale.x;
	rotationMatrix.r[1] = Vector3(mat.r[1]) / _outScale.y;
	rotationMatrix.r[2] = Vector3(mat.r[2]) / _outScale.z;
	rotationMatrix.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);  // 나머지 행렬 성분

	rotationMatrix.r[0] = DirectX::XMVector3Normalize(rotationMatrix.r[0]);
	rotationMatrix.r[1] = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(rotationMatrix.r[1], Vector3(DirectX::XMVector3Dot(rotationMatrix.r[0], rotationMatrix.r[1])) * Vector3(rotationMatrix.r[0])));
	rotationMatrix.r[2] = DirectX::XMVector3Cross(rotationMatrix.r[0], rotationMatrix.r[1]);

	_outRot = XMQuaternionRotationMatrix(rotationMatrix);

	return true;
}

physx::PxExtendedVec3 MathUtil::ConvertEx(const Vector3& _val)
{
	return physx::PxExtendedVec3(_val.x, _val.y, _val.z);
}

DirectX::SimpleMath::Vector3 MathUtil::Convert(const physx::PxExtendedVec3& _val)
{
	return Vector3
	(
		static_cast<float>(_val.x),
		static_cast<float>(_val.y),
		static_cast<float>(_val.z)
	);
}

physx::PxTransform MathUtil::Convert(Matrix _val)
{
	Vector3 pos;
	Quaternion rot;
	Vector3 scale;

	_val.Decompose(scale, rot, pos);

	physx::PxTransform result;
	result.p = Convert(pos);
	result.q = Convert(rot);
	return result;
}

physx::PxQuat MathUtil::Convert(const Quaternion& _val)
{
	return physx::PxQuat(_val.x, _val.y, _val.z, _val.w);
}

DirectX::SimpleMath::Quaternion MathUtil::Convert(const physx::PxQuat& _val)
{
	return Quaternion(_val.x, _val.y, _val.z, _val.w);
}

DirectX::SimpleMath::Matrix MathUtil::Convert(const physx::PxTransform& _val)
{
	Vector3 pos = Convert(_val.p);
	Quaternion rot = Convert(_val.q);

	Matrix result = Matrix::Identity;
	result.Translation(pos);
	result.CreateFromQuaternion(rot);
	return result;
}

DirectX::SimpleMath::Vector3 MathUtil::Convert(const physx::PxVec3& _val)
{
	return Vector3(_val.x, _val.y, _val.z);
}

physx::PxVec2 MathUtil::Convert(const Vector2& _val)
{
	return physx::PxVec2(_val.x, _val.y);
}

DirectX::SimpleMath::Vector3 MathUtil::GetBezjePoint(std::vector<Vector3>& _controlPoints, float _ratio)
{
	// 점 2개 미만이면 선이 되지 않는다.
	if (_controlPoints.size() < 2)
	{
		assert(false && "cannot get bezje point. need more control point");
		return Vector3::Zero;
	}

	// 점이 단 2개라면 곡선이 되지 않는다.
	if (_controlPoints.size() == 2)
	{
		return  Vector3::Lerp(_controlPoints[0], _controlPoints[1], _ratio);
	}
	else
	{
		// 모든 직선에 대한 보간을 한다.
		std::vector<Vector3> midterms(_controlPoints.size() - 1);
		for (size_t i = 0; i < _controlPoints.size() - 1; ++i)
		{
			midterms[i] = Vector3::Lerp(_controlPoints[i], _controlPoints[i + 1], _ratio);
		}
		// 보간된 점을 새로은 변환점으로 삼아 곡선을 구한다.
		return GetBezjePoint(midterms, _ratio);
	}
}

DirectX::SimpleMath::Vector3 MathUtil::GetCatmullPoint(std::vector<Vector3>& _controlPoints, float _ratio)
{
	// 점 2개 미만이면 선이 되지 않는다.
	if (_controlPoints.size() < 2)
	{
		assert(false && "cannot get Catmull point. need more control point");
		return Vector3::Zero;
	}

	// 점이 단 2개라면 곡선이 되지 않는다.
	if (_controlPoints.size() == 2)
	{
		return  Vector3::Lerp(_controlPoints[0], _controlPoints[1], _ratio);
	}

	// 모든 점을 구할 필요 없다. 현재 어느 점이 시작점인지 구한다.
	float framCount = 1.0f / static_cast<float>(_controlPoints.size() - 1);
	// 현재 점에서 다음 점까지의 보간값
	float realRatio = _ratio / framCount;
	// 현재점
	int32 panel = static_cast<int32>(realRatio);
	realRatio = realRatio - panel;

	// 켓멀 곡선을 구하기 위한 변환점 4개
	std::vector<Vector3> midterms(4, Vector3::Zero);

	// 현재 점이 시작점
	midterms[0] = _controlPoints[panel];

	// 시작점이 0이 아니면
	if (panel != 0)
	{
		midterms[1] = (_controlPoints[panel + 1] - _controlPoints[panel - 1]) / 2;
	}
	// 끝점이 -2가 아니면
	if (panel != _controlPoints.size() - 2)
	{
		midterms[2] = (_controlPoints[panel + 2] - _controlPoints[panel]) / 2;
	}

	// 끝점
	midterms[3] = _controlPoints[panel + 1];

	// 벡터를 점으로 표현
	midterms[1] = midterms[1] + midterms[0];
	midterms[2] = midterms[3] - midterms[2];

	// 허밋 곡선 구하기
	return GetBezjePoint(midterms, realRatio);
}

DirectX::SimpleMath::Vector3 MathUtil::GetHermitePoint3(std::vector<Vector3>& _controlPoints, float _ratio)
{
	if (_controlPoints.size() != 4)
	{
		assert(false && "cannot get Hermite 3 point. point count error");
		return Vector3::Zero;
	}

	return GetBezjePoint(_controlPoints, _ratio);
}


