#pragma once
#include "Component.h"

namespace Ideal
{
	class ICamra;
}

namespace Truth
{
	class Camera
		: public Component
	{
		GENERATE_CLASS_TYPE_INFO(Camera)

	private:
		friend class GraphicsManager;

	protected:
		std::shared_ptr<Ideal::ICamera> m_camera;

		PROPERTYM(fov, 0.02f, 1.0f);
		float m_fov;
		PROPERTY(aspect);
		float m_aspect;
		PROPERTY(nearZ);
		float m_nearZ;
		PROPERTY(farZ);
		float m_farZ;

	public:
		PROPERTY(position);
		Vector3 m_position;

		PROPERTY(look);
		Vector3 m_look;

	public:
		Camera();
		virtual ~Camera();

		METHOD(LateUpdate);
		virtual void LateUpdate();

		METHOD(SetLens);
		void SetLens(float _fovY, float _aspect, float _nearZ, float _farZ);

		void SetMainCamera();

		METHOD(SetLook);
		void SetLook(Vector3 _val);

		METHOD(Pitch);
		void Pitch(float angle);

		METHOD(RotateY);
		void RotateY(float angle);

		METHOD(Initialize);
		virtual void Initialize();

		void CompleteCamera();

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE

		void DefaultUpdate();
	};
}
