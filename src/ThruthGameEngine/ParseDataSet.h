#pragma once
#include "Headers.h"

struct ComponentData
{
};

struct ColliderData
	: ComponentData
{
	Vector3 m_size;
	Vector3 m_center;
	uint32 type;
};

struct MeshData
	: ComponentData
{
	std::string m_meshPath = "";
};

struct TransformData
	: ComponentData
{
	Matrix m_localTM = Matrix::Identity;
	Matrix m_worldTM = Matrix::Identity;
};

struct GameObject
{
	bool m_isCollider = false;
	bool m_isMesh = false;

	std::vector<ComponentData> m_collider;

	std::string m_guid = "";
	std::string m_fileID = "";

	GameObject* m_parent = nullptr;
	std::vector<GameObject*> m_children;
};


