#include "Mesh.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "ISkinnedMeshObject.h"
#include "Entity.h"
#include "IMesh.h"
#include "Material.h"
#include "ITexture.h"
#include "Texture.h"
#include "IMaterial.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Mesh)

/// <summary>
/// 생성자
/// </summary>
/// <param name="_path">해당 매쉬가 있는 경로</param>
Truth::Mesh::Mesh(std::wstring _path)
	: Component()
	, m_path(_path)
	, m_isRendering(true)
	, m_mesh(nullptr)
	, m_isStatic(false)
{
	m_name = "Mesh Filter";
}

/// <summary>
/// 기본 생성자
/// </summary>
Truth::Mesh::Mesh()
	: Component()
	, m_path(L"DebugObject/debugCube")
	, m_isRendering(true)
	, m_mesh(nullptr)
	, m_isStatic(false)
{
	m_name = "Mesh Filter";
}

/// <summary>
/// 소멸자
/// </summary>
Truth::Mesh::~Mesh()
{
	DeleteMesh();
}

/// <summary>
/// 새로운 매쉬 지정
/// </summary>
/// <param name="_path">경로</param>
void Truth::Mesh::SetMesh(std::wstring _path)
{
	if (m_path == _path)
	{
		return;
	}

	m_path = _path;

	if (m_mesh != nullptr)
	{
		DeleteMesh();
	}

	m_mesh = m_managers.lock()->Graphics()->CreateMesh(_path);

	uint32 meshSize = m_mesh->GetMeshesSize();
	m_subMesh.clear();
	m_subMesh.resize(meshSize);
	m_mat.clear();
	if (m_matPath.empty())
	{
		for (uint32 i = 0; i < meshSize; i++)
		{
			m_subMesh[i] = m_mesh->GetMeshByIndex(i).lock();
			std::string s = m_subMesh[i]->GetFBXMaterialName();
			m_mat.push_back(m_managers.lock()->Graphics()->CreateMaterial(s));
		}
	}
	else
	{
		for (uint32 i = 0; i < meshSize; i++)
		{
			const auto& mat = m_managers.lock()->Graphics()->CreateMaterial(m_matPath[i], false);
			m_mat.push_back(mat);
			m_mesh->GetMeshByIndex(i).lock()->SetMaterialObject(mat->m_material);
		}
	}
}

void Truth::Mesh::SetMesh()
{
	if (m_mesh != nullptr)
	{
		return;
	}

	m_mesh = m_managers.lock()->Graphics()->CreateMesh(m_path);

	uint32 meshSize = m_mesh->GetMeshesSize();
	m_subMesh.clear();
	m_subMesh.resize(meshSize);
	m_mat.clear();
	if (m_matPath.empty())
	{
		for (uint32 i = 0; i < meshSize; i++)
		{
			m_subMesh[i] = m_mesh->GetMeshByIndex(i).lock();
			std::string s = m_subMesh[i]->GetFBXMaterialName();
			m_mat.push_back(m_managers.lock()->Graphics()->CreateMaterial(s));
		}
	}
	else
	{
		for (uint32 i = 0; i < meshSize; i++)
		{
			const auto& mat = m_managers.lock()->Graphics()->CreateMaterial(m_matPath[i], false);
			m_mat.push_back(mat);
			m_mesh->GetMeshByIndex(i).lock()->SetMaterialObject(mat->m_material);
		}
	}
}

void Truth::Mesh::SetRenderable(bool _isRenderable)
{
}

void Truth::Mesh::Initialize()
{
	SetMesh();
	ApplyTransform();
}

void Truth::Mesh::ApplyTransform()
{
	if (!m_owner.lock()->m_isStatic)
	{
		m_mesh->SetTransformMatrix(m_owner.lock()->GetWorldTM());
		m_mesh->SetDrawObject(m_isRendering);
	}
}

void Truth::Mesh::SetMeshTransformMatrix()
{
	m_mesh->SetTransformMatrix(m_owner.lock()->GetWorldTM());
}

void Truth::Mesh::SetMeshTransformMatrix(const Matrix& _m)
{
	m_mesh->SetTransformMatrix(_m);
}

void Truth::Mesh::Update()
{
	if (!m_owner.lock()->m_isStatic)
		ApplyTransform();
}

void Truth::Mesh::SetActive()
{
	if (m_mesh)
	{
		m_mesh->SetDrawObject(false);
	}
}

void Truth::Mesh::Destroy()
{
	DeleteMesh();
}

void Truth::Mesh::DeleteMesh()
{
	if (m_mesh)
	{
		m_managers.lock()->Graphics()->DeleteMeshObject(m_mesh);
		m_mesh.reset();
	}
}

void Truth::Mesh::SetMaterialByIndex(uint32 _index, std::string _material)
{
	if (_index >= m_subMesh.size())
		return;

	const auto& mat = m_managers.lock()->Graphics()->CreateMaterial(_material);
	m_subMesh[_index]->SetMaterialObject(mat->m_material);

	if (mat->m_alphaCulling || mat->m_transparent)
		m_mesh->AlphaClippingCheck();

	if (_index >= m_matPath.size())
		return;

	m_matPath[_index] = mat->m_path;
}

std::vector<uint64> Truth::Mesh::GetMaterialImagesIDByIndex(uint32 _index)
{
	if (_index >= m_subMesh.size())
	{
		return std::vector<uint64>();
	}
	std::vector<uint64> result;
	std::string matName = m_subMesh[_index]->GetFBXMaterialName();
	std::shared_ptr<Material> mat = m_managers.lock()->Graphics()->GetMaterial(matName);

	result.push_back(mat->m_baseMap->m_texture->GetImageID());
	result.push_back(mat->m_normalMap->m_texture->GetImageID());
	result.push_back(mat->m_maskMap->m_texture->GetImageID());

	return result;
}

DirectX::SimpleMath::Matrix Truth::Mesh::GetMeshLocalTM()
{
	if (m_mesh)
	{
		return m_mesh->GetLocalTransformMatrix();
	}
	return Matrix::Identity;
}

void Truth::Mesh::SetStatic(bool _isStatic)
{
	m_mesh->SetStaticWhenRunTime(_isStatic);
}

#ifdef EDITOR_MODE
void Truth::Mesh::EditorSetValue()
{
	m_matPath.clear();
	SetMesh(m_path);
}
#endif // EDITOR_MODE

