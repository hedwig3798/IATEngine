#pragma once
#include "EditorDefine.h"

#ifdef EDITOR_MODE
#include "Headers.h"
#include "InputManager.h"

namespace Truth
{
	class Managers;
}

namespace Truth
{
	class EditorCamera
	{
	public:
		EditorCamera(Managers* _managers);
		~EditorCamera();


		void Update(float _dt);

		bool GetKey(KEY _key);
		bool GetKey(MOUSE _key);
		float MouseDy();
		float MouseDx();

		void SetMainCamera();

		std::shared_ptr<Ideal::ICamera> m_camera;
	public:

		float m_speed;

		Managers* m_managers;
	};
}
#endif // EDITOR_MODE

