#pragma once
#include "Component.h"
namespace Truth
{
	class PointLight :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(PointLight);
	private:

		std::shared_ptr<Ideal::IPointLight> m_pointLight;
	public:

		PROPERTY(isRendering);
		bool m_isRendering;

		PROPERTY(position);
		Vector3 m_position;

		PROPERTY(radius);
		float m_radius;

		PROPERTY(intensity);
		float m_intensity;

		PROPERTY(lightColor);
		Color m_lightColor;

		PROPERTY(layer);
		int m_layer;

		PROPERTY(isShadow);
		bool m_isShadow;

	public:
		PointLight();
		virtual ~PointLight();

		void SetLight();
		void SetIntensity();
		void SetColor();
		void SetRange();
		void SetPosition();
		void SetPosition(const Vector3& _position);
		void SetLayer();

		void SetShadow();

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE

	private:
		METHOD(Initialize);
		void Initialize();

		METHOD(Destroy);
		void Destroy();


	};
}

