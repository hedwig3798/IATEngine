#pragma once
#include "Component.h"

namespace physx
{
	class PxController;
	class PxControllerFilters;
	struct PxFilterData;
}

namespace Truth
{
	class RigidBody;
}



namespace Truth
{
	class Controller
		: public Component
	{
		GENERATE_CLASS_TYPE_INFO(Controller);

	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	private:
		bool m_CCTPass = false;

		physx::PxController* m_controller;
		physx::PxControllerFilters* m_controllerFilter;
		physx::PxFilterData* m_filterData;

		std::shared_ptr<RigidBody> m_rigidbody;
		std::shared_ptr<Collider> m_collider;

		const float m_minmumDistance;

		Vector3 m_impulse;

		PROPERTY(height);
		float m_height;

		PROPERTY(contactOffset);
		float m_contactOffset;

		PROPERTY(stepOffset);
		float m_stepOffset;

		PROPERTY(radius);
		float m_radius;

		PROPERTY(upDirection);
		Vector3 m_upDirection;

		PROPERTY(material);
		Vector3 m_material;

		PROPERTY(climbingmode);
		uint32 m_climbingmode;


		uint32 m_flag;

	public:
		Controller();
		~Controller();

		METHOD(Initialize);
		virtual void Initialize() override;

		METHOD(Awake);
		virtual void Awake() override;

		METHOD(Start);
		virtual void Start() override;

		METHOD(FixedUpdate);
		virtual void FixedUpdate() override;

		void Move(Vector3& _disp);
		void AddImpulse(Vector3& _disp);
		bool SetPosition(Vector3& _disp);
		void SetRotation(Quaternion& _val);
		bool IsCollisionDown();
		bool IsCollisionUp();
		bool IsCollisionSide();

		void PhysxAwake();

		void SetUpFiltering();
		void SetGroup(uint32 _group);
		void SetMask(uint32 _mask);

		void ChangeGroup(uint32 _group);
		void ChangeMask(uint32 _mask);

		void SetUserData(bool _data);

		std::shared_ptr<Truth::RigidBody> GetRigidbody() const { return m_rigidbody; }
	};

	template<class Archive>
	void Truth::Controller::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);

		_ar& m_height;
		_ar& m_contactOffset;
		_ar& m_stepOffset;
		_ar& m_radius;
		_ar& m_upDirection;
		_ar& m_material;
		_ar& m_climbingmode;
	}

	template<class Archive>
	void Truth::Controller::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		if (file_version > 0)
		{
			_ar& m_height;
			_ar& m_contactOffset;
			_ar& m_stepOffset;
			_ar& m_radius;
			_ar& m_upDirection;
			_ar& m_material;
			_ar& m_climbingmode;
		}
	}
}
BOOST_CLASS_EXPORT_KEY(Truth::Controller)
BOOST_CLASS_VERSION(Truth::Controller, 1)