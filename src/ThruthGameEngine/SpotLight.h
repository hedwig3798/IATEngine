#pragma once
#include "Component.h"
namespace Truth
{
	class SpotLight :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(SpotLight);

		std::shared_ptr<Ideal::ISpotLight> m_spotLight;
	public:

		PROPERTY(isRendering);
		bool m_isRendering;

		PROPERTY(position);
		Vector3 m_position;
		PROPERTY(direction);
		Vector3 m_direction;

		PROPERTY(angle);
		float m_angle;
		PROPERTY(range);
		float m_range;

		PROPERTY(intensity);
		float m_intensity;

		PROPERTY(softness);
		float m_softness;

		PROPERTY(lightColor);
		Color m_lightColor;

	public:
		SpotLight();
		virtual ~SpotLight();

		void SetLight();
		void SetIntensity();
		void SetColor();
		void SetAngle();
		void SetRange();
		void SetSoftness();
		void SetPosition();
		void SetDirection();

		METHOD(Initialize);
		void Initialize();

		METHOD(Destroy);
		void Destroy();

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE
	};
}

