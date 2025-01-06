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

	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

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

	template<class Archive>
	void Truth::Camera::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_fov;
		_ar& m_aspect;
		_ar& m_nearZ;
		_ar& m_farZ;
	}

	template<class Archive>
	void Truth::Camera::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_fov;
		_ar& m_aspect;
		_ar& m_nearZ;
		_ar& m_farZ;
	}
}
BOOST_CLASS_EXPORT_KEY(Truth::Camera)
BOOST_CLASS_VERSION(Truth::Camera, 0)