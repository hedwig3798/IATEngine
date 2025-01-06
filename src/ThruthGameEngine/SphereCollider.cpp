#include "SphereCollider.h"
#include "Entity.h"
#include "RigidBody.h"
#include "PhysicsManager.h"
#include "MathUtil.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::SphereCollider)

/// <summary>
/// �� ������ �ݶ��̴�
/// </summary>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::SphereCollider::SphereCollider(bool _isTrigger)
	: Collider(_isTrigger)
	, m_radius(0.5f)
{
	m_name = "Sphere Collider";
	m_shape = ColliderShape::SPHERE;

}

/// <summary>
/// �� ������ �ݶ��̴�
/// </summary>
/// <param name="_radius">������</param>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::SphereCollider::SphereCollider(float _radius, bool _isTrigger /*= true*/)
	: Collider(_isTrigger)
{
	m_shape = ColliderShape::SPHERE;

	m_name = "Sphere Collider";

	SetRadius(_radius);
}

/// <summary>
/// �� ������ �ݶ��̴�
/// </summary>
/// <param name="_pos">�߽���</param>
/// <param name="_radius">������</param>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::SphereCollider::SphereCollider(Vector3 _pos, float _radius, bool _isTrigger /*= true*/)
	: Collider(_pos, _isTrigger)
{
	m_shape = ColliderShape::SPHERE;

	m_name = "Sphere Collider";

	SetRadius(_radius);
}

/// <summary>
/// ������ ����
/// </summary>
/// <param name="_radius">������</param>
void Truth::SphereCollider::SetRadius(float _radius)
{
	m_radius = _radius;

	// ���� �ٵ� �ִٸ� �ش� �ٵ��� �ݶ��̴��� �����ϰ� ���� �����Ͽ� �ٿ��ش�.
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
/// �ʱ�ȭ �ش� Component�� ���� �� �� �ѹ� �����
/// </summary>
void Truth::SphereCollider::Initialize()
{
	Collider::Initialize();
}
