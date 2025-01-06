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

