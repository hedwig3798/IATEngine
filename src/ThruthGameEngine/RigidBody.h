#pragma once
#include "Headers.h"
#include "Component.h"

namespace physx
{
	class PxRigidDynamic;
	class PxController;
}
namespace Truth
{
	class Controller;
}


namespace Truth
{
	class RigidBody :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(RigidBody);

		friend class Controller;

	public:
		PROPERTY(mass);
		float m_mass;
		PROPERTY(drag);
		float m_drag;
		PROPERTY(angularDrag);
		float m_angularDrag;

		PROPERTY(useGravity);
		bool m_useGravity;
		PROPERTY(isKinematic);
		bool m_isKinematic;

		PROPERTY(freezePosition);
		std::vector<bool> m_freezePosition;
		PROPERTY(freezeRotation);
		std::vector<bool> m_freezeRotation;

		Vector3 m_velocity;
		physx::PxRigidDynamic* m_body;

		Matrix m_localTM;
		Matrix m_globalTM;

	private:
		float m_speed;
		Vector3 m_angularVelocity;
		Vector3 m_inertiaTensor;
		Vector3 m_inertiaTensorRotation;
		Vector3 m_localMassCenter;
		Vector3 m_worldMassCenter;

		std::weak_ptr<Transform> m_transform;

		std::vector<std::weak_ptr<Collider>> m_colliders;

		bool m_isChanged;

		bool m_isController;
		physx::PxController* m_controller;


	public:
		RigidBody();
		virtual ~RigidBody();

		METHOD(Update);
		void Update();

		void AddImpulse(Vector3& _force);
		void SetLinearVelocity(Vector3& _val);
		Vector3 GetLinearVelocity() const;

		void InitalizeMassAndInertia();

		METHOD(Initialize);
		void Initialize();

		METHOD(ApplyTransform);
		void ApplyTransform();

		bool IsController() const { return m_isController; }

		Quaternion GetRotation();

		physx::PxController* GetController() { return m_controller; }

	private:
		METHOD(Awake);
		void Awake();

		METHOD(Start);
		void Start();

		METHOD(Destroy);
		void Destroy();

		void CalculateMassCenter();
	};
}