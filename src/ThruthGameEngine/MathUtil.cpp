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

	_outScale.x = DirectX::XMVectorGetX(DirectX::XMVector3Length(mat.r[0])); // X�� ������
	_outScale.y = DirectX::XMVectorGetY(DirectX::XMVector3Length(mat.r[1])); // Y�� ������
	_outScale.z = DirectX::XMVectorGetZ(DirectX::XMVector3Length(mat.r[2])); // Z�� ������

	DirectX::XMMATRIX rotationMatrix;
	rotationMatrix.r[0] = Vector3(mat.r[0]) / _outScale.x;
	rotationMatrix.r[1] = Vector3(mat.r[1]) / _outScale.y;
	rotationMatrix.r[2] = Vector3(mat.r[2]) / _outScale.z;
	rotationMatrix.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);  // ������ ��� ����

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
	// �� 2�� �̸��̸� ���� ���� �ʴ´�.
	if (_controlPoints.size() < 2)
	{
		assert(false && "cannot get bezje point. need more control point");
		return Vector3::Zero;
	}

	// ���� �� 2����� ��� ���� �ʴ´�.
	if (_controlPoints.size() == 2)
	{
		return  Vector3::Lerp(_controlPoints[0], _controlPoints[1], _ratio);
	}
	else
	{
		// ��� ������ ���� ������ �Ѵ�.
		std::vector<Vector3> midterms(_controlPoints.size() - 1);
		for (size_t i = 0; i < _controlPoints.size() - 1; ++i)
		{
			midterms[i] = Vector3::Lerp(_controlPoints[i], _controlPoints[i + 1], _ratio);
		}
		// ������ ���� ������ ��ȯ������ ��� ��� ���Ѵ�.
		return GetBezjePoint(midterms, _ratio);
	}
}

DirectX::SimpleMath::Vector3 MathUtil::GetCatmullPoint(std::vector<Vector3>& _controlPoints, float _ratio)
{
	// �� 2�� �̸��̸� ���� ���� �ʴ´�.
	if (_controlPoints.size() < 2)
	{
		assert(false && "cannot get Catmull point. need more control point");
		return Vector3::Zero;
	}

	// ���� �� 2����� ��� ���� �ʴ´�.
	if (_controlPoints.size() == 2)
	{
		return  Vector3::Lerp(_controlPoints[0], _controlPoints[1], _ratio);
	}

	// ��� ���� ���� �ʿ� ����. ���� ��� ���� ���������� ���Ѵ�.
	float framCount = 1.0f / static_cast<float>(_controlPoints.size() - 1);
	// ���� ������ ���� �������� ������
	float realRatio = _ratio / framCount;
	// ������
	int32 panel = static_cast<int32>(realRatio);
	realRatio = realRatio - panel;

	// �ϸ� ��� ���ϱ� ���� ��ȯ�� 4��
	std::vector<Vector3> midterms(4, Vector3::Zero);

	// ���� ���� ������
	midterms[0] = _controlPoints[panel];

	// �������� 0�� �ƴϸ�
	if (panel != 0)
	{
		midterms[1] = (_controlPoints[panel + 1] - _controlPoints[panel - 1]) / 2;
	}
	// ������ -2�� �ƴϸ�
	if (panel != _controlPoints.size() - 2)
	{
		midterms[2] = (_controlPoints[panel + 2] - _controlPoints[panel]) / 2;
	}

	// ����
	midterms[3] = _controlPoints[panel + 1];

	// ���͸� ������ ǥ��
	midterms[1] = midterms[1] + midterms[0];
	midterms[2] = midterms[3] - midterms[2];

	// ��� � ���ϱ�
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


