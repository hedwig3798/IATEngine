#include "BoxCollider.h"
#include "RigidBody.h"
#include "Component.h"
#include "Entity.h"
#include "PhysicsManager.h"
#include "MathUtil.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::BoxCollider)

/// <summary>
/// ť�� ������ �ݶ��̴�
/// </summary>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::BoxCollider::BoxCollider(bool _isTrigger)
	: Collider(_isTrigger)
{
	m_shape = ColliderShape::BOX;
	m_name = "Box Collider";
	m_size = { 1.0f, 1.0f, 1.0f };
}

/// <summary>
/// �ڽ� ������ �ݶ��̴�
/// </summary>
/// <param name="_pos">���� ��ġ</param>
/// <param name="_size">������</param>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::BoxCollider::BoxCollider(Vector3 _pos, Vector3 _size, bool _isTrigger)
	: Collider(_pos, _isTrigger)
{
	m_shape = ColliderShape::BOX;
	m_name = "Box Collider";
	m_size = _size;
}

/// <summary>
/// �ڽ� ������ �ݶ��̴�
/// </summary>
/// <param name="_size">������</param>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::BoxCollider::BoxCollider(Vector3 _size, bool _isTrigger)
	: Collider(_isTrigger)
{
	m_shape = ColliderShape::BOX;
	m_name = "Box Collider";
	m_size = _size;
}

/// <summary>
/// �ʱ�ȭ �ش� Component�� ���� �� �� �ѹ� �����
/// </summary>
void Truth::BoxCollider::Initialize()
{
	Collider::Initialize();
}

