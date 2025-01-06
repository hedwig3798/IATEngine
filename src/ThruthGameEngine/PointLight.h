#pragma once
#include "Component.h"
namespace Truth
{
	class PointLight :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(PointLight);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

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
	template<class Archive>
	void Truth::PointLight::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_position;
		_ar& m_radius;
		_ar& m_intensity;
		_ar& m_lightColor;
		if (file_version >= 1)
		{
			_ar& m_layer;
		}
		if (file_version >= 2)
		{
			_ar& m_isShadow;
		}
	}

	template<class Archive>
	void Truth::PointLight::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_position;
		_ar& m_radius;
		_ar& m_intensity;
		_ar& m_lightColor;
		_ar& m_layer;
		_ar& m_isShadow;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::PointLight)
BOOST_CLASS_VERSION(Truth::PointLight, 2)