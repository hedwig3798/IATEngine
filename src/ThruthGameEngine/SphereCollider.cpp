#include "SphereCollider.h"
#include "Entity.h"
#include "RigidBody.h"
#include "PhysicsManager.h"
#include "MathUtil.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::SphereCollider)

/// <summary>
/// 구 형태의 콜라이더
/// </summary>
/// <param name="_isTrigger">트리거 여부</param>
Truth::SphereCollider::SphereCollider(bool _isTrigger)
	: Collider(_isTrigger)
	, m_radius(0.5f)
{
	m_name = "Sphere Collider";
	m_shape = ColliderShape::SPHERE;

}

/// <summary>
/// 구 형태의 콜라이더
/// </summary>
/// <param name="_radius">반지름</param>
/// <param name="_isTrigger">트리거 여부</param>
Truth::SphereCollider::SphereCollider(float _radius, bool _isTrigger /*= true*/)
	: Collider(_isTrigger)
{
	m_shape = ColliderShape::SPHERE;

	m_name = "Sphere Collider";

	SetRadius(_radius);
}

/// <summary>
/// 구 형태의 콜라이더
/// </summary>
/// <param name="_pos">중심점</param>
/// <param name="_radius">반지름</param>
/// <param name="_isTrigger">트리거 여부</param>
Truth::SphereCollider::SphereCollider(Vector3 _pos, float _radius, bool _isTrigger /*= true*/)
	: Collider(_pos, _isTrigger)
{
	m_shape = ColliderShape::SPHERE;

	m_name = "Sphere Collider";

	SetRadius(_radius);
}

/// <summary>
/// 반지름 조정
/// </summary>
/// <param name="_radius">반지름</param>
void Truth::SphereCollider::SetRadius(float _radius)
{
	m_radius = _radius;

	// 만일 바디가 있다면 해당 바디의 콜라이더를 제거하고 새로 생성하여 붙여준다.
	if (m_body != nullptr)
	{
		m_body->detachShape(*m_collider);
		m_collider->release();

		m_collider = CreateCollider(ColliderShape::SPHERE, m_size);
		// SetUpFiltering(m_owner.lock()->m_layer);
		m_collider->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
		m_collider->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);

		m_body->attachShape(*m_collider);
	}
	m_size = Vector3::One * (m_radius * 2);
}

/// <summary>
/// 초기화 해당 Component가 생성 될 때 한번 실행됨
/// </summary>
void Truth::SphereCollider::Initialize()
{
	Collider::Initialize();
}
