#include "PointLight.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Entity.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::PointLight)

Truth::PointLight::PointLight()
	: Component()
	, m_isRendering(true)
	, m_pointLight(nullptr)
	, m_position{ 0.0f, 0.0f, 0.0f }
	, m_radius(1.0f)
	, m_lightColor{ 1.0f, 1.0f, 1.0f, 1.0f }
	, m_intensity(1.0f)
	, m_layer(1)
	, m_isShadow(true)
{
	m_name = "PointLight";
}

Truth::PointLight::~PointLight()
{

}

void Truth::PointLight::SetLight()
{
	m_pointLight = m_managers.lock()->Graphics()->CreatePointLight();
}

void Truth::PointLight::SetIntensity()
{
	m_pointLight->SetIntensity(m_intensity);
}

void Truth::PointLight::SetColor()
{
	m_pointLight->SetLightColor(m_lightColor);
}

void Truth::PointLight::SetRange()
{
	m_pointLight->SetRange(m_radius);
}

void Truth::PointLight::SetPosition()
{
	m_pointLight->SetPosition(m_position);
}

void Truth::PointLight::SetPosition(const Vector3& _position)
{
	m_position = _position;
	SetPosition();
}

void Truth::PointLight::SetLayer()
{
	m_pointLight->ChangeLayerBitMask(static_cast<uint32>(m_layer));
}

void Truth::PointLight::SetShadow()
{
	m_pointLight->SetShadowCasting(m_isShadow);
}

void Truth::PointLight::Initialize()
{
	SetLight();
	SetIntensity();
	SetColor();
	SetRange();
	SetPosition();
	SetLayer();
	SetShadow();
}

void Truth::PointLight::Destroy()
{
	m_managers.lock()->Graphics()->DeletePointLight(m_pointLight);
	// m_pointLight.reset();
}

#ifdef EDITOR_MODE
void Truth::PointLight::EditorSetValue()
{
	SetIntensity();
	SetColor();
	SetRange();
	SetPosition();
	SetShadow();
	SetLayer();
}
#endif // _DEBUG

