#pragma once
#include "Component.h"
#include "ShapeType.h"

// #include "PhysicsManager.h"

namespace physx
{
	class PxShape;
	class PxRigidDynamic;
	class PxRigidActor;
	class PxRigidStatic;
	struct PxFilterData;
}

namespace Truth
{
	class RigidBody;
	class Entity;
}

namespace Truth
{
	class Collider abstract 
		: public Component
	{
		GENERATE_CLASS_TYPE_INFO(Collider);

	protected:
		ColliderShape m_shape;
		Vector3 m_debugMeshSize;

	public:
		PROPERTY(isTrigger);
		bool m_isTrigger;

		PROPERTY(center);
		Vector3 m_center;

		PROPERTY(size);
		Vector3 m_size;

		PROPERTY(enable);
		bool m_enable;

		Matrix m_localTM;
		Matrix m_globalTM;

		inline static uint32 m_colliderIDGenerator = 0;
		uint32 m_colliderID;
		physx::PxShape* m_collider;

		physx::PxRigidActor* m_body;
		std::weak_ptr<RigidBody> m_rigidbody;

		bool m_isController;

		uint32 m_collisionMask;
		uint32 m_collisionGroup;

#ifdef EDITOR_MODE
		std::shared_ptr<Ideal::IMeshObject> m_debugMesh;
#endif // EDITOR_MODE

	protected:

	public:
		Collider(bool _isTrigger = true);
		Collider(Vector3 _pos, bool _isTrigger = true);
		virtual ~Collider();

		METHOD(Destroy);
		void Destroy();

		METHOD(Awake);
		void Awake();

		METHOD(FixedUpdate);
		void FixedUpdate();

		METHOD(SetCenter);
		void SetCenter(Vector3 _pos);

		METHOD(SetSize);
		void SetSize(Vector3 _size);

		METHOD(SetActive);
		void SetActive();

		void SetGroup(uint32 _group);
		void SetMask(uint32 _mask);

#ifdef EDITOR_MODE
		METHOD(ApplyTransform);
		void ApplyTransform();

		void EditorSetValue();
#endif // EDITOR_MODE

	protected:
		physx::PxShape* CreateCollider(ColliderShape _shape, const Vector3& _args);
		physx::PxRigidDynamic* GetDefaultDynamic();
		physx::PxRigidStatic* GetDefaultStatic();

		void Initialize(const std::wstring& _path = L"");
		
	public:
		void SetUpFiltering();

		void ChangeGroup(uint32 _group);
		void ChangeMask(uint32 _mask);

	private:
	};

	struct Collision
	{
		std::weak_ptr<Collider> m_collA;
		std::weak_ptr<Collider> m_collB;
	};
}
