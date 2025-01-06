#include "CapsuleCollider.h"
#include "Entity.h"
#include "RigidBody.h"
#include "PhysicsManager.h"
#include "MathUtil.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::CapsuleCollider)

/// <summary>
/// ĸ�� ������ �ݶ��̴�
/// </summary>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::CapsuleCollider::CapsuleCollider(bool _isTrigger)
	: Collider(_isTrigger)
	, m_radius(0.5f)
	, m_height(1.0f)
{
	m_shape = ColliderShape::CAPSULE;
	m_name = "Capsule Collider";

}

/// <summary>
/// ĸ�� ������ �ݶ��̴�
/// </summary>
/// <param name="_radius">������</param>
/// <param name="_height">����</param>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::CapsuleCollider::CapsuleCollider(float _radius, float _height, bool _isTrigger)
	: Collider(_isTrigger)
{
	m_shape = ColliderShape::CAPSULE;

	m_name = "Capsule Collider";

	SetRadius(_radius);
	SetHeight(_height);
}

/// <summary>
/// ĸ�� ������ �ݶ��̴�
/// </summary>
/// <param name="_pos">��ġ</param>
/// <param name="_radius">������</param>
/// <param name="_height">����</param>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::CapsuleCollider::CapsuleCollider(Vector3 _pos, float _radius, float _height, bool _isTrigger)
	: Collider(_pos, _isTrigger)
{
	m_shape = ColliderShape::CAPSULE;

	m_name = "Capsule Collider";

	SetRadius(_radius);
	SetHeight(_height);
}

/// <summary>
/// ������ ����
/// </summary>
/// <param name="_radius">������</param>
void Truth::CapsuleCollider::SetRadius(float _radius)
{
	m_radius = _radius;
	if (m_body != nullptr)
	{
		m_body->detachShape(*m_collider);
		m_collider->release();

		m_collider = CreateCollider(ColliderShape::CAPSULE, m_size);
		// SetUpFiltering(m_owner.lock()->m_layer);
		m_collider->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
		m_collider->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
		m_body->attachShape(*m_collider);
	}
}

/// <summary>
/// ���� ����
/// </summary>
/// <param name="_radius">����</param>
void Truth::CapsuleCollider::SetHeight(float _height)
{
	m_height = _height;
	if (m_body != nullptr)
	{
		m_body->detachShape(*m_collider);
		m_collider->release();

		m_collider = CreateCollider(ColliderShape::CAPSULE, m_size);
		// SetUpFiltering(m_owner.lock()->m_layer);
		m_collider->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
		m_collider->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
		m_body->attachShape(*m_collider);
	}
}

/// <summary>
/// �ʱ�ȭ �ش� Component�� ���� �� �� �ѹ� �����
/// </summary>
void Truth::CapsuleCollider::Initialize()
{
	Collider::Initialize();
}