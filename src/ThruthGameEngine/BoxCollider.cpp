#include "BoxCollider.h"
#include "RigidBody.h"
#include "Component.h"
#include "Entity.h"
#include "PhysicsManager.h"
#include "MathUtil.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::BoxCollider)

/// <summary>
/// 큐브 형태의 콜라이더
/// </summary>
/// <param name="_isTrigger">트리거 여부</param>
Truth::BoxCollider::BoxCollider(bool _isTrigger)
	: Collider(_isTrigger)
{
	m_shape = ColliderShape::BOX;
	m_name = "Box Collider";
	m_size = { 1.0f, 1.0f, 1.0f };
}

/// <summary>
/// 박스 형태의 콜라이더
/// </summary>
/// <param name="_pos">생성 위치</param>
/// <param name="_size">사이즈</param>
/// <param name="_isTrigger">트리거 여부</param>
Truth::BoxCollider::BoxCollider(Vector3 _pos, Vector3 _size, bool _isTrigger)
	: Collider(_pos, _isTrigger)
{
	m_shape = ColliderShape::BOX;
	m_name = "Box Collider";
	m_size = _size;
}

/// <summary>
/// 박스 형태의 콜라이더
/// </summary>
/// <param name="_size">사이즈</param>
/// <param name="_isTrigger">트리거 여부</param>
Truth::BoxCollider::BoxCollider(Vector3 _size, bool _isTrigger)
	: Collider(_isTrigger)
{
	m_shape = ColliderShape::BOX;
	m_name = "Box Collider";
	m_size = _size;
}

/// <summary>
/// 초기화 해당 Component가 생성 될 때 한번 실행됨
/// </summary>
void Truth::BoxCollider::Initialize()
{
	Collider::Initialize();
}

