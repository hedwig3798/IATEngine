#include "EmptyEntity.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "Mesh.h"

EmptyEntity::EmptyEntity()
{
	m_name = "GameObject";
	m_layer = 1;
}

EmptyEntity::~EmptyEntity()
{

}

void EmptyEntity::Initailize()
{
	__super::Initailize();
	SetPosition(Vector3{ 0.0f, 0.0f, 0.0f });
	SetScale(Vector3(1.0f, 1.0f, 1.0f));
}
