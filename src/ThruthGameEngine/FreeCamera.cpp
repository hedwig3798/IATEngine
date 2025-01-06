#include "FreeCamera.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "ICamera.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::FreeCamera)

Truth::FreeCamera::FreeCamera()
	: Camera()
	, m_speed(50.0f)
{
	m_name = "Free Camera";
}

Truth::FreeCamera::~FreeCamera()
{

}

void Truth::FreeCamera::Start()
{
	SetMainCamera();
}

void Truth::FreeCamera::Update()
{
	float dt = GetDeltaTime();
	if (GetKey(KEY::W))
	{
		m_camera->Walk(dt * m_speed);
	}
	if (GetKey(KEY::S))
	{
		m_camera->Walk(-dt * m_speed);
	}
	if (GetKey(KEY::A))
	{
		m_camera->Strafe(-dt * m_speed);
	}
	if (GetKey(KEY::D))
	{
		m_camera->Strafe(dt * m_speed);
	}

	if (GetKey(KEY::E))
	{
		m_camera->SetPosition(m_camera->GetPosition() + Vector3(0.0f, dt * m_speed, 0.0f));
	}
	if (GetKey(KEY::Q))
	{
		m_camera->SetPosition(m_camera->GetPosition() + Vector3(0.0f, -dt * m_speed, 0.0f));
	}

	if (GetKey(MOUSE::RMOUSE))
	{
		Pitch(MouseDy() * 0.003f);
		RotateY(MouseDx() * 0.003f);
	}

	// CompleteCamera();
}

