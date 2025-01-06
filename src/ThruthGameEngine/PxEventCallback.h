#pragma once
#include "Headers.h"
#include "PhysicsManager.h"
namespace Truth
{
	class PxEventCallback
		: public physx::PxSimulationEventCallback
	{
	public:
		PxEventCallback();
		virtual ~PxEventCallback();
		virtual void onContact(const physx::PxContactPairHeader& _pairHeader, const physx::PxContactPair* _pairs, physx::PxU32 _nbPairs);
		virtual void onTrigger(physx::PxTriggerPair* _pairs, physx::PxU32 _count);
		virtual void onWake(physx::PxActor** _actors, physx::PxU32 _count);
		virtual void onSleep(physx::PxActor** _actors, physx::PxU32 _count);
		virtual void onAdvance(const physx::PxRigidBody* const* _bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 _count);
		virtual void onConstraintBreak(physx::PxConstraintInfo* _constraints, physx::PxU32 _count);
	};
}


