#include "Component.h"
#include "TimeManager.h"
#include "EventManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Entity.h"
#include "Transform.h"
#include "IBone.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Component)

uint32 Truth::Component::m_IDGenerater = 0;

Truth::Component::Component()
	: m_canMultiple(false)
	, m_managers()
	, m_owner()
	, m_index(-1)
	, m_ID(++m_IDGenerater)
{
	m_name = typeid(*this).name();
}


Truth::Component::~Component()
{
	if (!m_managers.expired())
	{
		m_managers.lock()->Event()->RemoveListener(this);
	}
}

void Truth::Component::Translate(Vector3& _val)
{
	m_owner.lock()->m_transform->Translate(_val);
}

void Truth::Component::SetPosition(const Vector3& _pos) const
{
	m_owner.lock()->m_transform->SetPosition(_pos);
}

void Truth::Component::SetRotation(const Quaternion& _val)
{
	m_owner.lock()->m_transform->SetRotate(_val);
}

void Truth::Component::SetWorldRotation(const Quaternion& _val)
{
	m_owner.lock()->SetWorldRotation(_val);
}

void Truth::Component::SetScale(const Vector3& _scale) const
{
	m_owner.lock()->m_transform->SetScale(_scale);
}

const DirectX::SimpleMath::Vector3& Truth::Component::GetPosition() const
{
	return m_owner.lock()->m_transform->m_position;
}

const DirectX::SimpleMath::Quaternion& Truth::Component::GetRotation() const
{
	return m_owner.lock()->m_transform->m_rotation;
}

const DirectX::SimpleMath::Vector3& Truth::Component::GetScale() const
{
	return m_owner.lock()->m_transform->m_scale;
}

const DirectX::SimpleMath::Matrix& Truth::Component::GetWorldTM() const
{
	return m_owner.lock()->m_transform->m_globalTM;
}

const DirectX::SimpleMath::Matrix& Truth::Component::GetParentWorldTM() const
{
	return m_owner.lock()->GetParentMatrix();
}

const DirectX::SimpleMath::Matrix& Truth::Component::GetLocalTM() const
{
	return m_owner.lock()->m_transform->m_localTM;
}

void Truth::Component::SetLocalTM(const Matrix& _tm)
{
	m_owner.lock()->m_transform->SetLocalTM(_tm);
}

void Truth::Component::AddEmptyEntity()
{
	m_managers.lock()->Scene()->m_currentScene->AddEntity(std::make_shared<Entity>());
}

void Truth::Component::AddEntity(std::shared_ptr<Entity> _entity)
{
	m_managers.lock()->Scene()->m_currentScene->AddEntity(_entity);
}

bool Truth::Component::HasParent()
{
	return m_owner.lock()->HasParent();
}

bool Truth::Component::IsLinked()
{
	return m_owner.lock()->IsLinked();
}

const DirectX::SimpleMath::Matrix& Truth::Component::GetParentMatrix()
{
	return m_owner.lock()->GetParentMatrix();
}

const DirectX::SimpleMath::Matrix& Truth::Component::GetParentLinkedMatrix()
{
	return m_owner.lock()->m_linkedBone.lock()->GetTransform();
}
