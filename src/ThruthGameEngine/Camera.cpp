#include "Camera.h"
#include "Transform.h"
#include "Managers.h"
#include "ICamera.h"
#include "InputManager.h"
#include "GraphicsManager.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Camera)

Truth::Camera::Camera()
	: Component()
	, m_fov(0.25f * 3.141592f)
	, m_aspect(1)
	, m_nearZ(1)
	, m_farZ(1000)
	, m_look(0.f,0.f,1.f)
{
	m_name = "Camera";
}

Truth::Camera::~Camera()
{

}


void Truth::Camera::LateUpdate()
{
}

void Truth::Camera::SetLens(float _fovY, float _aspect, float _nearZ, float _farZ)
{
	m_camera->SetLens(_fovY, _aspect, _nearZ, _farZ);
	m_fov = _fovY;
	m_aspect = _aspect;
	m_nearZ = _nearZ;
	m_farZ = _farZ;
}

void Truth::Camera::SetMainCamera()
{
	m_managers.lock()->Graphics()->SetMainCamera(this);
}

void Truth::Camera::SetLook(Vector3 _val)
{
	m_camera->SetLook(_val);
}

void Truth::Camera::Pitch(float angle)
{
	m_camera->Pitch(angle);
}

void Truth::Camera::RotateY(float angle)
{
	m_camera->RotateY(angle);
}

void Truth::Camera::DefaultUpdate()
{
	float m_speed = 1.0f;
	if (GetKey(KEY::UP))
	{
		m_camera->Walk(m_speed);
	}
	if (GetKey(KEY::DOWN))
	{
		m_camera->Walk(-m_speed);
	}
	if (GetKey(KEY::LEFT))
	{
		m_camera->Strafe(-m_speed);
	}
	if (GetKey(KEY::RIGHT))
	{
		m_camera->Strafe(m_speed);
	}

	if (GetKey(MOUSE::LMOUSE))
	{
		m_camera->Pitch(MouseDy() * 0.3f);
		m_camera->RotateY(MouseDx() * 0.3f);
	}
}

void Truth::Camera::Initialize()
{
	m_camera = m_managers.lock()->Graphics()->CreateCamera();
	//m_camera->SetPosition(Vector3(0.f, 0.f, -150.f));
	m_position = { 0.f, 0.f, 0.f };
	SetLens(0.25f * 3.141592f, m_managers.lock()->Graphics()->GetAspect(), 1.f, 100000.f);
	// SetMainCamera();
}

void Truth::Camera::CompleteCamera()
{
	m_camera->SetPosition(m_owner.lock()->m_transform->m_position);
	m_camera->SetLook(m_look);
}

#ifdef EDITOR_MODE
void Truth::Camera::EditorSetValue()
{
	m_camera->SetLens(m_fov, m_aspect, m_nearZ, m_farZ);
}
#endif // EDITOR_MODE
