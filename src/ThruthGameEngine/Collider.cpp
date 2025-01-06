#include "Collider.h"
#include "PhysicsManager.h"
#include "MathUtil.h"
#include "RigidBody.h"
#include "Component.h"
#include "GraphicsManager.h"
#include "RigidBody.h"
#include "Controller.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Collider)

/// <summary>
/// �ݶ��̴� ������
/// </summary>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::Collider::Collider(bool _isTrigger /*= true*/)
	: Component()
	, m_isTrigger(_isTrigger)
	, m_center{ 0.0f, 0.0f, 0.0f }
	, m_collider(nullptr)
	, m_body(nullptr)
	, m_colliderID(m_colliderIDGenerator++)
	, m_rigidbody()
#ifdef EDITOR_MODE
	, m_debugMesh(nullptr)
#endif // EDITOR_MODE
	, m_shape()
	, m_enable(true)
	, m_isController(false)
	, m_collisionGroup(0)
	, m_collisionMask(0)

{
	m_center = { 0.0f, 0.0f, 0.0f };
	m_size = { 1.0f, 1.0f, 1.0f };
}

/// <summary>
/// �ݶ��̴� ������
/// </summary>
/// <param name="_pos">�߽���</param>
/// <param name="_isTrigger">Ʈ���� ����</param>
Truth::Collider::Collider(Vector3 _pos, bool _isTrigger /*= true*/)
	: m_center(_pos)
	, m_isTrigger(_isTrigger)
	, m_collider(nullptr)
	, m_body(nullptr)
	, m_colliderID(m_colliderIDGenerator++)
	, m_rigidbody()
	, m_shape()
	, m_enable(true)
	, m_isController(false)
#ifdef EDITOR_MODE
	, m_debugMesh(nullptr)
#endif // EDITOR_MODE
{
	m_size = { 1.0f, 1.0f, 1.0f };
}

/// <summary>
/// �Ҹ���
/// �ݶ��̴��� physx scene�� ��� �ִٸ� ��ȯ�Ѵ�.
/// </summary>
Truth::Collider::~Collider()
{
	if (m_collider != nullptr)
	{
		m_collider->release();
		m_collider->userData = nullptr;
		m_collider = nullptr;
	}

#ifdef EDITOR_MODE
	if (m_debugMesh != nullptr)
	{
		m_managers.lock()->Graphics()->DeleteDebugMeshObject(m_debugMesh);
		m_debugMesh = nullptr;
	}
#endif // EDITOR_MODE
}

/// <summary>
/// ����� �Ҹ���
/// Entity�� game scene���� ���� �� �� ����ȴ�.
/// </summary>
void Truth::Collider::Destroy()
{
	if (m_collider)
	{
		m_collider->userData = nullptr;
		m_body->detachShape(*m_collider);
	}

#ifdef EDITOR_MODE
// 	if (m_debugMesh != nullptr)
// 	{
// 		m_managers.lock()->Graphics()->DeleteMeshObject(m_debugMesh);
// 		m_debugMesh = nullptr;
// 	}
#endif // EDITOR_MODE
}

/// <summary>
/// Component ���� �� �۵��ϴ� �Լ�
/// </summary>
void Truth::Collider::Awake()
{
	SetCenter(m_center);
	SetSize(m_size);

	const Matrix& ownerTM = m_owner.lock()->GetWorldTM();
	Matrix tempSzie = Matrix::CreateScale(m_debugMeshSize);
	m_globalTM = m_localTM * ownerTM;

	if (m_shape == ColliderShape::MESH)
	{
		return;
	}

	Vector3 finalSize = {};
	Vector3 finalPos = {};
	Quaternion temp = {};

	bool isSRT = (m_localTM * m_owner.lock()->GetWorldTM()).Decompose(finalSize, temp, finalPos);

	if (!isSRT)
	{
		MathUtil::DecomposeNonSRT(finalSize, temp, finalPos, (m_localTM * m_owner.lock()->GetWorldTM()));
	}

	// 	if (finalSize == Vector3::Zero)
	// 	{
	// 		(m_owner.lock()->GetWorldTM() * m_localTM * Matrix::CreateScale(Vector3{0.1f, 0.1f, 0.1f})).Decompose(finalSize, temp, finalPos);
	// 	}

	Vector3 onwerSize = m_owner.lock()->GetWorldScale();
	m_collider = CreateCollider(m_shape, (finalSize) / 2);
	m_collider->userData = this;

	m_collider->setLocalPose(physx::PxTransform(
		MathUtil::Convert(m_center)
	));

	// SetUpFiltering(m_owner.lock()->m_layer);

	bool active = m_enable && m_owner.lock()->m_isActive;

	m_collider->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
	m_collider->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);

	m_rigidbody = m_owner.lock()->GetComponent<RigidBody>();
	auto con = m_owner.lock()->GetComponent<Controller>();

	if (!con.expired())
	{
		m_rigidbody = con.lock()->GetRigidbody();
		m_body = m_rigidbody.lock()->m_body;
		m_body->attachShape(*m_collider);

		auto af = m_body->getActorFlags();
		auto rf = m_body->getBaseFlags();
		auto sf = m_collider->getFlags();
		return;
	}

	if (m_rigidbody.expired())
	{
		m_body = m_managers.lock()->Physics()->CreateDefaultRigidStatic();
		m_body->attachShape(*m_collider);
		physx::PxTransform t(
			MathUtil::Convert(m_owner.lock()->GetWorldPosition()),
			MathUtil::Convert(m_owner.lock()->GetWorldRotation())
		);
		m_body->setGlobalPose(t);
		m_managers.lock()->Physics()->AddScene(m_body);
		m_body->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, !active);
		SetUpFiltering();
		m_managers.lock()->Physics()->RsetFiltering(m_body);
		return;
	}

	m_body = m_rigidbody.lock()->m_body;
	m_body->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, !active);

	Vector3 p = m_owner.lock()->GetWorldPosition();
	Quaternion r = m_owner.lock()->GetWorldRotation();

	physx::PxTransform t(
		MathUtil::Convert(m_owner.lock()->GetWorldPosition()),
		MathUtil::Convert(m_owner.lock()->GetWorldRotation())
	);
	m_body->setGlobalPose(t);

}

void Truth::Collider::FixedUpdate()
{
	if (m_body)
	{
		physx::PxTransform t(
			MathUtil::Convert(m_owner.lock()->GetWorldPosition()),
			MathUtil::Convert(m_owner.lock()->GetWorldRotation())
		);
		m_body->setGlobalPose(t);
	}
}

/// <summary>
/// Collider�� �߽��� ���Ѵ�
/// </summary>
/// <param name="_pos">�߽���</param>
void Truth::Collider::SetCenter(Vector3 _pos)
{
	m_center = _pos;
	m_localTM = Matrix::CreateScale(m_size);
	m_localTM *= Matrix::CreateTranslation(m_center);
}

/// <summary>
/// Collider�� ũ�⸦ ���Ѵ� (���ũ��)
/// </summary>
/// <param name="_size">ũ��</param>
void Truth::Collider::SetSize(Vector3 _size)
{
	m_size = _size;
	m_localTM = Matrix::CreateScale(m_size);
	m_localTM *= Matrix::CreateTranslation(m_center);
}

void Truth::Collider::SetActive()
{
	bool active = m_enable && m_owner.lock()->m_isActive;
	if (m_body)
		m_body->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, !active);

#ifdef EDITOR_MODE
	if (m_debugMesh && !active)
	{
		m_managers.lock()->Graphics()->DeleteDebugMeshObject(m_debugMesh);
		m_debugMesh.reset();
	}
	else if (!m_debugMesh && active)
	{
		switch (m_shape)
		{
		case Truth::ColliderShape::BOX:
		{
			m_debugMesh = m_managers.lock()->Graphics()->CreateDebugMeshObject(L"DebugObject/debugCube");
			m_debugMeshSize = { 50, 50, 50 };
			break;
		}
		case Truth::ColliderShape::SPHERE:
		{
			m_debugMesh = m_managers.lock()->Graphics()->CreateDebugMeshObject(L"DebugObject/debugSphere");
			m_debugMeshSize = { 0.5, 0.5, 0.5 };
			break;
		}
		default:
			break;
		}
	}
#endif // EDITOR_MODE

	//	if (m_collider)
	// 	{
	// 		m_collider->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger && active);
	// 		m_collider->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger && active);
	// 	}
}

void Truth::Collider::SetGroup(uint32 _group)
{
	m_collisionGroup = _group;
}

void Truth::Collider::SetMask(uint32 _mask)
{
	m_collisionMask = _mask;
}

#ifdef EDITOR_MODE
/// <summary>
/// ����� �Ž��� �׸��� ���� �Լ�
/// </summary>
void Truth::Collider::ApplyTransform()
{
	if (!m_debugMesh)
	{
		return;
	}
	const Matrix& ownerTM = m_owner.lock()->GetWorldTM();
	Matrix tempSzie = Matrix::CreateScale(m_debugMeshSize);
	m_globalTM = m_localTM * ownerTM;
	m_debugMesh->SetTransformMatrix(tempSzie * m_globalTM);

}

/// <summary>
/// �����Ϳ��� ������ ���� �����Ѵ� 
/// </summary>
void Truth::Collider::EditorSetValue()
{
	SetCenter(m_center);
	SetSize(m_size);

	SetActive();
}
#endif // EDITOR_MODE

/// <summary>
/// �ݶ��̴��� physx�� ���� �����Ѵ�
/// ������ �� �� phsyx scene�� ���ε� �Ǵ°��� �ƴϴ�.
/// </summary>
/// <param name="_shape">��� (ť��, ��, ĸ��)</param>
/// <param name="_args">�� ��翡 �´� �Ķ���� (���� ����, ����, ������ ����)</param>
/// <returns></returns>
physx::PxShape* Truth::Collider::CreateCollider(ColliderShape _shape, const Vector3& _args)
{
	return m_managers.lock()->Physics()->CreateCollider(_shape, _args);
}

/// <summary>
/// ����Ʈ ���̳��� �ٵ� ����
/// Rigidbody�� ���� �ݶ��̴��� ��� �ش� �ٵ� ���
/// </summary>
/// <returns></returns>
physx::PxRigidDynamic* Truth::Collider::GetDefaultDynamic()
{
	return m_managers.lock()->Physics()->CreateDefaultRigidDynamic();
}

/// <summary>
/// ����Ʈ ����ƽ �ٵ� ����
/// Rigidbody�� ���� �ݶ��̴��� ��� �ش� �ٵ� ���
/// </summary>
/// <returns></returns>
physx::PxRigidStatic* Truth::Collider::GetDefaultStatic()
{
	return m_managers.lock()->Physics()->CreateDefaultRigidStatic();
}

/// <summary>
/// �ʱ�ȭ
/// </summary>
/// <param name="_path">����� �Ž����</param>
void Truth::Collider::Initialize(const std::wstring& _path /*= L""*/)
{
#ifdef EDITOR_MODE
 	if (m_debugMesh != nullptr)
 	{
 		return;
 	}
 	switch (m_shape)
 	{
 	case Truth::ColliderShape::BOX:
 	{
 		m_debugMesh = m_managers.lock()->Graphics()->CreateDebugMeshObject(L"DebugObject/debugCube");
 		m_debugMeshSize = { 50, 50, 50 };
 		break;
 	}
 	case Truth::ColliderShape::SPHERE:
 	{
 		m_debugMesh = m_managers.lock()->Graphics()->CreateDebugMeshObject(L"DebugObject/debugSphere");
 		m_debugMeshSize = { 0.5, 0.5, 0.5 };
 		break;
 	}
 	default:
 		break;
 	}

#endif // EDITOR_MODE

	m_localTM = Matrix::CreateScale(m_size);
	m_localTM *= Matrix::CreateTranslation(m_center);
}

/// <summary>
/// ���� ����
/// �浹 ó���� ���� ���� ���� ��Ÿ���� ���� ������ ����
/// </summary>
/// <param name="_filterGroup"></param>
void Truth::Collider::SetUpFiltering()
{
	physx::PxFilterData filterData;
	filterData.word0 = m_collisionGroup;
	filterData.word1 = m_collisionMask;
	m_collider->setSimulationFilterData(filterData);
	m_collider->setQueryFilterData(filterData);
}

void Truth::Collider::ChangeGroup(uint32 _group)
{
	m_collisionGroup = _group;
	SetUpFiltering();
	if (!m_managers.expired())
		m_managers.lock()->Physics()->RsetFiltering(m_collider->getActor());
}

void Truth::Collider::ChangeMask(uint32 _mask)
{
	m_collisionMask = _mask;
	SetUpFiltering();
	if (!m_managers.expired())
		m_managers.lock()->Physics()->RsetFiltering(m_collider->getActor());
}

