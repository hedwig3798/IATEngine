#pragma once
#include "Component.h"
namespace Truth
{
	class DirectionLight :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(DirectionLight);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

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

	template<class Archive>
	void Truth::DirectionLight::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		if (file_version >= 1)
		{
			_ar& m_direction;
		}
		if (file_version >= 3)
		{
 			_ar& m_intensity;
 			_ar& m_diffuseColor;
 			_ar& m_ambientColor;
		}
	}

	template<class Archive>
	void Truth::DirectionLight::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_direction;
 		_ar& m_intensity;
 		_ar& m_diffuseColor;
 		_ar& m_ambientColor;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::DirectionLight)
BOOST_CLASS_VERSION(Truth::DirectionLight, 3)