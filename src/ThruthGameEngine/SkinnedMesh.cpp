#include "SkinnedMesh.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "ISkinnedMeshObject.h"
#include "IAnimation.h"
#include "Entity.h"
#include "IBone.h"
#include "Imesh.h"
#include "IMaterial.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::SkinnedMesh)

/// <summary>
/// ��θ� ������ ������
/// </summary>
/// <param name="_path">�Ž� ������ �ִ� ���</param>
Truth::SkinnedMesh::SkinnedMesh(std::wstring _path)
	: Component()
	, m_path(_path)
	, m_isRendering(true)
	, m_skinnedMesh()
	, m_currentFrame(0)
	, m_isAnimationPlaying(false)
	, m_isAnimationEnd(false)
	, m_isAnimationChanged(false)
	, m_animationMaxFrame(0)
	, m_oldFrame(0)
{
	m_name = "Skinned Mesh Filter";
}

/// <summary>
/// ��ΰ� �������� ���� ������
/// �⺻ ī���� �Ž��� �������ش�.
/// </summary>
Truth::SkinnedMesh::SkinnedMesh()
	: Component()
	, m_path(L"Kachujin/Mesh")
	, m_isRendering(true)
	, m_skinnedMesh()
	, m_currentFrame(0)
	, m_animationMaxFrame(0)
	, m_isAnimationPlaying(false)
	, m_isAnimationEnd(false)
	, m_isAnimationChanged(false)
	, m_oldFrame(0)
{
	m_name = "Skinned Mesh Filter";
}

/// <summary>
/// �Ҹ���
/// </summary>
Truth::SkinnedMesh::~SkinnedMesh()
{
	DeleteMesh();
}

/// <summary>
/// �Ž� ���� �Լ�
/// ��ο� �ִ� �Ž� �����ͷ� �Ž��� �������ش�.
/// </summary>
/// <param name="_path">�Ž� ������ ���</param>
void Truth::SkinnedMesh::SetSkinnedMesh(std::wstring _path)
{
	m_path = _path;
	fs::path meshPath = m_path;

	// �̹� �Ž��� �ִٸ� ����� ���� �����.
	if (!m_skinnedMesh.expired())
	{
		DeleteMesh();
		m_skinnedMesh.reset();
	}
	// �Ž� ����
	m_skinnedMesh = m_managers.lock()->Graphics()->CreateSkinnedMesh(_path);

	// �� ���� ���ε�
	m_boneMap.clear();
	uint32 boneSize = m_skinnedMesh.lock()->GetBonesSize();
	for (uint32 i = 0; i < boneSize; i++)
	{
		std::weak_ptr<Ideal::IBone> bone = m_skinnedMesh.lock()->GetBoneByIndex(i);
		m_boneMap[bone.lock()->GetName()] = bone;
	}

	// ���׸��� ������ ����Ǿ����� �ʴٸ�
	// �⺻ ���׸����� ������ �����Ͽ� �����Ѵ�.
	if (m_matPath.empty())
	{
		for (size_t i = 0; i < m_skinnedMesh.lock()->GetMeshesSize(); i++)
		{
			std::string matName = m_skinnedMesh.lock()->GetMeshByIndex(static_cast<uint32>(i)).lock()->GetFBXMaterialName();
			fs::path matPath = "../Resources/Matarial" / meshPath.filename();

			if (!fs::exists(matPath))
				fs::create_directories(matPath);

			matPath = matPath.filename() / matName;

			auto material = m_managers.lock()->Graphics()->CreateMaterial(matPath.generic_string());
			m_mat.push_back(material);

			m_skinnedMesh.lock()->GetMeshByIndex(static_cast<uint32>(i)).lock()->SetMaterialObject(material->m_material);

			if (material->m_alphaCulling)
			{
				m_skinnedMesh.lock()->AlphaClippingCheck();
			}
		}
	}
	// ����Ǿ� �ִ� ���׸��� ������ �ҷ��´�.
	else
	{
		for (size_t i = 0; i < m_skinnedMesh.lock()->GetMeshesSize(); i++)
		{
			auto material = m_managers.lock()->Graphics()->CreateMaterial(m_matPath[i], false);
			m_mat.push_back(material);

			m_skinnedMesh.lock()->GetMeshByIndex(static_cast<uint32>(i)).lock()->SetMaterialObject(material->m_material);
			if (material->m_alphaCulling)
			{
				m_skinnedMesh.lock()->AlphaClippingCheck();
			}
		}
	}
}

/// <summary>
/// �Ž��� �ִϸ��̼� �߰�
/// </summary>
/// <param name="_name">�ִϸ��̼� �̸�</param>
/// <param name="_path">�ִϸ��̼� ������ ���</param>
/// <param name="_offset">�ִϸ��̼� ������ ��Ʈ����</param>
void Truth::SkinnedMesh::AddAnimation(std::string _name, std::wstring _path, const Matrix& _offset /*= Matrix::Identity*/)
{
	if (!m_animation.expired())
		m_animation.reset();

	m_animation = m_managers.lock()->Graphics()->CreateAnimation(_path, _offset);

	if (!m_skinnedMesh.expired())
		m_skinnedMesh.lock()->AddAnimation(_name, m_animation.lock());
}

/// <summary>
/// ���� ����� �ִϸ��̼� ����
/// </summary>
/// <param name="_name">�ִϸ��̼� �̸�</param>
/// <param name="WhenCurrentAnimationFinished">���� �ִϸ��̼��� ���� �ǰ� ������� ����</param>
void Truth::SkinnedMesh::SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished)
{
	if (!m_animation.expired())
	{
		m_isAnimationChanged = true;
		m_skinnedMesh.lock()->SetAnimation(_name, WhenCurrentAnimationFinished);
	}
}

/// <summary>
/// �ִϸ��̼� ��� �ӵ� ����
/// </summary>
/// <param name="Speed">�ӵ� (����)</param>
void Truth::SkinnedMesh::SetAnimationSpeed(float Speed)
{
	m_skinnedMesh.lock()->SetAnimationSpeed(Speed);
}

/// <summary>
/// �ִϸ��̼� ��� ���� ����
/// </summary>
/// <param name="playStop">��� ����</param>
void Truth::SkinnedMesh::SetPlayStop(bool playStop)
{
	m_skinnedMesh.lock()->SetPlayAnimation(playStop);
}

void Truth::SkinnedMesh::Initialize()
{
	SetSkinnedMesh(m_path);
	if (m_skinnedMesh.lock())
		m_skinnedMesh.lock()->SetDrawObject(m_owner.lock()->m_isActive);
}

void Truth::SkinnedMesh::Update()
{
	if (!m_isAnimationChanged)
		m_currentFrame = m_skinnedMesh.lock()->GetCurrentAnimationIndex();

	if (m_isAnimationChanged)
	{
		if (m_skinnedMesh.lock()->GetCurrentAnimationIndex() == 0)
		{
			m_currentFrame = 0;
			m_oldFrame = 0;
			m_isAnimationChanged = false;
		}
	}

	if (!m_animation.expired())
	{
		m_animationMaxFrame = m_skinnedMesh.lock()->GetCurrentAnimationMaxFrame();

		m_skinnedMesh.lock()->AnimationDeltaTime(GetDeltaTime());

		if (m_oldFrame > m_currentFrame)
		{
			m_isAnimationEnd = true;
			m_isAnimationPlaying = false;
		}
		else
		{
			m_isAnimationEnd = false;
			m_isAnimationPlaying = true;
		}

		m_oldFrame = m_currentFrame;
	}
}

void Truth::SkinnedMesh::ApplyTransform()
{
// 	if (m_owner.lock()->m_name == "MeleeEnemy")
// 	{
// 		auto t = m_owner.lock()->GetWorldTM();
// 		Vector3 pos, sca;
// 		Quaternion q;
// 		t.Decompose(sca, q, pos);
// 		Vector3 rot = q.ToEuler();
// 
// 		std::string temp = 
// 			"x: " + std::to_string(rot.x) + " y: " + std::to_string(rot.y) + " z: " + std::to_string(rot.z) + "\n";
// 		DEBUG_PRINT(temp.c_str());
// 	}
	m_skinnedMesh.lock()->SetTransformMatrix(m_owner.lock()->GetWorldTM());
	m_skinnedMesh.lock()->SetDrawObject(m_isRendering);
}

void Truth::SkinnedMesh::Destroy()
{
	DeleteMesh();
}

void Truth::SkinnedMesh::SetActive()
{
	if (!m_skinnedMesh.expired())
		m_skinnedMesh.lock()->SetDrawObject(m_owner.lock()->m_isActive);
}

/// <summary>
/// �Ž� ����
/// </summary>
void Truth::SkinnedMesh::DeleteMesh()
{
	if (!m_skinnedMesh.expired())
		m_managers.lock()->Graphics()->DeleteMeshObject(m_skinnedMesh.lock());
	m_skinnedMesh.reset();
}

/// <summary>
/// �̸����� ���� �˻��ؼ� �����Ѵ�.
/// </summary>
/// <param name="_name">�̸�</param>
/// <returns>��</returns>
std::weak_ptr<Ideal::IBone> Truth::SkinnedMesh::GetBone(const std::string& _name)
{
	if (m_boneMap.find(_name) == m_boneMap.end())
		return std::weak_ptr<Ideal::IBone>();
	return m_boneMap[_name];
}

#ifdef EDITOR_MODE
void Truth::SkinnedMesh::EditorSetValue()
{
	m_matPath.clear();
	SetSkinnedMesh(m_path);
}
#endif // EDITOR_MODE

