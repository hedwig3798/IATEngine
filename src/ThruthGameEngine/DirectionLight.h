#pragma once
#include "Component.h"
namespace Truth
{
	class DirectionLight :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(DirectionLight);

		std::shared_ptr<Ideal::IDirectionalLight> m_directionalLight;
	public:

		PROPERTY(isRendering);
		bool m_isRendering;

		PROPERTY(direction);
		Vector3 m_direction;

		PROPERTY(intensity);
		float m_intensity;

		PROPERTY(diffuseColor);
		Color m_diffuseColor;

		PROPERTY(ambientColor);
		Color m_ambientColor;

	public:
		DirectionLight();
		virtual ~DirectionLight();

		void SetLight();
		void SetIntensity();
		void SetDiffuse();
		void SetAmbient();

		METHOD(Destroy);
		void Destroy();

		METHOD(Initialize);
		void Initialize();

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE

	};
}
