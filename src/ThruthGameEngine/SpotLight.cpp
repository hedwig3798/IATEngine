#include "SpotLight.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Entity.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::SpotLight)

Truth::SpotLight::SpotLight()
	: Component()
	, m_isRendering(true)
	, m_spotLight(nullptr)
	, m_position{ 0.0f, 0.0f, 0.0f }
	, m_direction{ 0.0f, -1.0f, 0.0f }
	, m_angle(10.f)
	, m_range(10.f)
	, m_intensity(10.f)
	, m_softness(0.0f)
	, m_lightColor{ 1.0f, 1.0f, 1.0f, 1.0f }
{
	m_name = "SpotLight";
}

Truth::SpotLight::~SpotLight()
{
}

void Truth::SpotLight::SetLight()
{
	if (m_position.x < 1 && m_position.x > -1 &&
		m_position.y < 1 && m_position.y > -1 &&
		m_position.z < 1 && m_position.z > -1)
	{
		int a = 1;
	}
	m_spotLight = m_managers.lock()->Graphics()->CreateSpotLight();
}

void Truth::SpotLight::SetIntensity()
{
	m_spotLight->SetIntensity(m_intensity);
}

void Truth::SpotLight::SetColor()
{
	m_spotLight->SetLightColor(m_lightColor);
}

void Truth::SpotLight::SetAngle()
{
	m_spotLight->SetSpotAngle(m_angle);
}

void Truth::SpotLight::SetRange()
{
	m_spotLight->SetRange(m_range);
}

void Truth::SpotLight::SetSoftness()
{
	m_spotLight->SetSoftness(m_softness);
}

void Truth::SpotLight::SetPosition()
{
	m_spotLight->SetPosition(m_position);

}

void Truth::SpotLight::SetDirection()
{
	Vector3 dir;
	m_direction.Normalize(dir);

	m_spotLight->SetDirection(dir);
}

void Truth::SpotLight::Initialize()
{
	SetLight();
	SetIntensity();
	SetColor();
	SetSoftness();
	SetRange();
	SetAngle();
	SetPosition();
	SetDirection();
}

void Truth::SpotLight::Destroy()
{
	m_managers.lock()->Graphics()->DeleteSpotLight(m_spotLight);
}

#ifdef EDITOR_MODE
void Truth::SpotLight::EditorSetValue()
{
	SetIntensity();
	SetColor();
	SetSoftness();
	SetRange();
	SetAngle();
	SetPosition();
	SetDirection();
}
#endif // _DEBUG

