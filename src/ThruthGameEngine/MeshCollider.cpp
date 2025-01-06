#include "MeshCollider.h"
#include "TFileUtils.h"
#include "PhysicsManager.h"
#include "MathUtil.h"
#include "RigidBody.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::MeshCollider)

Truth::MeshCollider::MeshCollider()
	: Collider(false)
{
	m_size = Vector3{ 1.0f, 1.0f, 1.0f };
	m_path = L"TestMap/navTestMap";
	m_shape = ColliderShape::MESH;
}

Truth::MeshCollider::MeshCollider(std::string _path, bool _isConvex)
{
	m_size = Vector3{ 1.0f, 1.0f, 1.0f };
	USES_CONVERSION;
	m_path = A2W(_path.c_str());

	m_shape = ColliderShape::MESH;
	m_isConvex = _isConvex;
}

Truth::MeshCollider::~MeshCollider()
{

}

void Truth::MeshCollider::Initialize()
{
	Collider::Initialize(m_path);
}

void Truth::MeshCollider::Awake()
{
	Vector3 onwerSize = m_owner.lock()->GetLocalScale();

	GetPoints();
	if (m_points.empty())
		return;

	if (m_isConvex)
		m_meshCollider = m_managers.lock()->Physics()->CreateConvexMeshCollider((m_size * onwerSize), m_points);
	else
		m_meshCollider = m_managers.lock()->Physics()->CreateMeshCollider((m_size * onwerSize), m_points, m_index);

	m_isTrigger = false;

	physx::PxFilterData filterData;
	filterData.word0 = m_collisionGroup;
	filterData.word1 = m_collisionMask;
	m_meshCollider[0]->setSimulationFilterData(filterData);
	m_meshCollider[0]->setQueryFilterData(filterData);

	m_meshCollider[0]->userData = this;

	m_meshCollider[0]->setLocalPose(physx::PxTransform(
		MathUtil::Convert(m_center)
	));

	m_meshCollider[0]->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
	m_meshCollider[0]->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);

	m_rigidbody = m_owner.lock()->GetComponent<RigidBody>();

	if (m_rigidbody.expired())
	{
		m_body = m_managers.lock()->Physics()->CreateDefaultRigidStatic();

		m_body->attachShape(*m_meshCollider[0]);

		physx::PxTransform t(
			MathUtil::Convert(m_owner.lock()->GetLocalPosition()),
			MathUtil::Convert(m_owner.lock()->GetLocalRotation())
		);
		m_body->setGlobalPose(t);
		m_managers.lock()->Physics()->AddScene(m_body);
		m_managers.lock()->Physics()->RsetFiltering(m_body);
	}

}

/// <summary>
/// 경로에서 정점 데이터를 가져온다
/// </summary>
void Truth::MeshCollider::GetPoints()
{
	std::shared_ptr<TFileUtils> file = std::make_shared<TFileUtils>();
	std::wstring prefix = L"../Resources/Models/";
	file->Open(prefix + m_path + L".pos", FileMode::Read);

	unsigned int meshNum = file->Read<unsigned int>();
	for (unsigned int i = 0; i < meshNum; i++)
	{
		m_points.push_back(std::vector<Vector3>());
		unsigned int verticesNum = file->Read<unsigned int>();
		for (unsigned int j = 0; j < verticesNum; j++)
		{
			Vector3 p;
			p.x = file->Read<float>();
			p.y = file->Read<float>();
			p.z = file->Read<float>();
			m_points[i].push_back(p);
		}
		m_index.push_back(std::vector<int>());
		unsigned int temp = file->Read<unsigned int>();
		for (unsigned int j = 0; j < temp; j++)
		{
			m_index[i].push_back(file->Read<uint32>());
		}
	}
}