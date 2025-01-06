#pragma once
#include "Headers.h"
#include "ShapeType.h"

// 라이브러리에 경고가 너무 많다
// 나중에 이 부분 수정 예정
// 뭔가 다른 방법이 있겠지
#pragma warning(push)
#pragma warning(disable: 26495)
#pragma warning(disable: 33010)
#pragma warning(disable: 6297)
#include "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/include/PxConfig.h"
#include "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/include/PxPhysicsAPI.h"
#pragma warning(pop)

#ifdef _DEBUG
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/LowLevel_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/LowLevelAABB_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/LowLevelDynamics_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysX_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXCommon_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXCooking_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXExtensions_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXFoundation_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXTask_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/PhysXVehicle_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/SceneQuery_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/debug/lib/SimulationController_static_64.lib")
#else
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/LowLevel_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/LowLevelAABB_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/LowLevelDynamics_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysX_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXCommon_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXCooking_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXExtensions_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXFoundation_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXTask_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/PhysXVehicle_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/SceneQuery_static_64.lib")
#pragma comment(lib, "../packages/NVIDIA.PhysX.4.1.229882250/installed/x64-windows/lib/SimulationController_static_64.lib")
#endif // EDITOR_MODE

namespace Truth
{
	class PxEventCallback;
	class TruthPxQueryFilterCallback;
	class TruthPxCCTFilterCallback;
	enum class COLLISION_GROUP
	{
		ENV = 1 << 0,
		PLAYER = 1 << 1,
		ENEMY = 1 << 2,
		CAMERA = 1 << 3,
		ENEMY_PASS_PLAYER = 1 << 4,
		ENEMY_ATTACK = 1 << 5,
		PLAYER_ATTACK = 1 << 6,

		ALL = (ENV | PLAYER | ENEMY | CAMERA),
		PLAYER_MASK = (ENV | ENEMY_ATTACK),
		PLAYER_ATTACK_MASK = (ENEMY),
		ENEMY_MASK = (ENV | PLAYER | PLAYER_ATTACK),
		ENEMY_ATTACK_MASK = (PLAYER),
		ENEMY_PASS_MASK = (ENV | PLAYER_ATTACK),
		CAMERA_MASK = (ENV),
	};
}

/// <summary>
/// PhysX 라이브러리를 사용하는 매니저
/// 모든 PhysX 기능은 이 라이브러리를 통해 실행 될 것이다.
/// </summary>
namespace Truth
{
	class PhysicsManager
	{
	private:

		// Foundation 생성
		physx::PxFoundation* m_foundation;
		physx::PxDefaultAllocator m_allocator;
		physx::PxDefaultErrorCallback m_errorCallback;
	
		physx::PxPhysics* m_physics;

		physx::PxDefaultCpuDispatcher* m_dispatcher;
		physx::PxScene* m_scene;
		physx::PxMaterial* m_material;

		// 디버거
		physx::PxPvd* m_pvd;
		physx::PxPvdTransport* m_trasport;

		physx::PxReal m_stackZ = 20.0f;

		physx::PxControllerManager* m_CCTManager;
		// physx::PxOmniPvd* m_oPvd;

		physx::PxCooking* m_cooking;
		bool m_isInteractive = false;

	public:
		Truth::PxEventCallback* collisionCallback;
		TruthPxQueryFilterCallback* m_qCallback;
		TruthPxCCTFilterCallback* m_cCallback;

	public:
		PhysicsManager();
		~PhysicsManager();

		void Initalize();
		void Finalize();
		void Update();
		void FixedUpdate();

		void ResetPhysX();

		void AddScene(physx::PxActor* _actor);

		physx::PxMaterial* GetMaterial() { return m_material; };

		physx::PxRigidDynamic* CreateRigidDynamic(Vector3 _pos, Quaternion _rot);
		physx::PxRigidStatic* CreateRigidStatic(Vector3 _pos, Quaternion _rot);

		physx::PxRigidDynamic* CreateDefaultRigidDynamic();
		physx::PxRigidStatic* CreateDefaultRigidStatic();
		 
		physx::PxShape* CreateCollider(ColliderShape _shape, const Vector3& _args);
		std::vector<physx::PxShape*> CreateConvexMeshCollider(const Vector3& _args, const std::vector<std::vector<Vector3>>& _points = std::vector<std::vector<Vector3>>());
		std::vector<physx::PxShape*> CreateMeshCollider(const Vector3& _args, const std::vector<std::vector<Vector3>>& _points, const std::vector<std::vector<int>>& _index);

		physx::PxController* CreatePlayerController(const physx::PxCapsuleControllerDesc& _desc);

		physx::PxMaterial* CreateMaterial(Vector3 _val);

		void CreateMapCollider(const std::wstring& _path);

		Vector3 GetRayCastHitPoint(const Vector3& _start, const Vector3& _direction, float _range, uint32 _group, uint32 _mask);

		void RsetFiltering(physx::PxActor* _actor);

	private:
		void CreatePhysxScene();
		std::vector<std::vector<physx::PxVec3>> ConvertPointToVertex(const Vector3& _args, const std::vector<std::vector<Vector3>>& _points);

		void CreateMapCollider(const std::vector<physx::PxVec3>& _vers);
	};

	physx::PxFilterFlags FilterShaderExample(
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);
}

namespace Truth
{
	class TruthPxEventCallback
		: public physx::PxSimulationEventCallback
	{
	public:
		virtual void onContact(const physx::PxContactPairHeader& _pairHeader, const physx::PxContactPair* _pairs, physx::PxU32 _nbPairs) {};
		virtual void onTrigger(physx::PxTriggerPair* _pairs, physx::PxU32 _count) {};
		virtual void onWake(physx::PxActor** _actors, physx::PxU32 _count) {};
		virtual void onSleep(physx::PxActor** _actors, physx::PxU32 _count) {};
		virtual void onAdvance(const physx::PxRigidBody* const* _bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 _count) {};
		virtual void onConstraintBreak(physx::PxConstraintInfo* _constraints, physx::PxU32 _count) {};
	};

	class TruthPxQueryFilterCallback
		: public physx::PxQueryFilterCallback
	{
	public:
		physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData& filterData, const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags) override;
		physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit) override;

	};

	class TruthPxCCTFilterCallback
		: public physx::PxControllerFilterCallback
	{
	public:
		virtual bool filter(const physx::PxController& a, const physx::PxController& b) override;

	};
}

