#pragma once
#include "Camera.h"

namespace Ideal
{
	class ICamera;
}

namespace Truth
{
    class FreeCamera :
        public Camera
    {
		GENERATE_CLASS_TYPE_INFO(FreeCamera);
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

	private:
		float m_speed;

	public:
		FreeCamera();
		virtual ~FreeCamera();

	private:
		METHOD(Start);
		void Start();
		METHOD(Update);
		void Update();
    };
}

template<class Archive>
void Truth::FreeCamera::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(Truth::FreeCamera);