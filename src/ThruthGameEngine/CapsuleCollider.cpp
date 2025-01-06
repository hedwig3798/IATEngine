#include "CapsuleCollider.h"
#include "Entity.h"
#include "RigidBody.h"
#include "PhysicsManager.h"
#include "MathUtil.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::CapsuleCollider)

/// <summary>
/// 캡슐 형태의 콜라이더
/// </summary>
/// <param name="_isTrigger">트리거 여부</param>
Truth::CapsuleCollider::CapsuleCollider(bool _isTrigger)
	: Collider(_isTrigger)
	, m_radius(0.5f)
	, m_height(1.0f)
{
	m_shape = ColliderShape::CAPSULE;
	m_name = "Capsule Collider";

}

/// <summary>
/// 캡슐 형태의 콜라이더
/// </summary>
/// <param name="_radius">반지름</param>
/// <param name="_height">높이</param>
/// <param name="_isTrigger">트리거 여부</param>
Truth::CapsuleCollider::CapsuleCollider(float _radius, float _height, bool _isTrigger)
	: Collider(_isTrigger)
{
	m_shape = ColliderShape::CAPSULE;

	m_name = "Capsule Collider";

	SetRadius(_radius);
	SetHeight(_height);
}

/// <summary>
/// 캡슐 형태의 콜라이더
/// </summary>
/// <param name="_pos">위치</param>
/// <param name="_radius">반지름</param>
/// <param name="_height">높이</param>
/// <param name="_isTrigger">트리거 여부</param>
Truth::CapsuleCollider::CapsuleCollider(Vector3 _pos, float _radius, float _height, bool _isTrigger)
	: Collider(_pos, _isTrigger)
{
	m_shape = ColliderShape::CAPSULE;

	m_name = "Capsule Collider";

	SetRadius(_radius);
	SetHeight(_height);
}

/// <summary>
/// 반지름 조정
/// </summary>
/// <param name="_radius">반지름</param>
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
/// 높이 조정
/// </summary>
/// <param name="_radius">높이</param>
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
/// 초기화 해당 Component가 생성 될 때 한번 실행됨
/// </summary>
void Truth::CapsuleCollider::Initialize()
{
	Collider::Initialize();
}