#include "PxEventCallback.h"
#include "Collider.h"
#include "Entity.h"

Truth::PxEventCallback::PxEventCallback()
{
}

Truth::PxEventCallback::~PxEventCallback()
{
}

void Truth::PxEventCallback::onContact(const physx::PxContactPairHeader& _pairHeader, const physx::PxContactPair* _pairs, physx::PxU32 _nbPairs)
{
	for (uint32 i = 0; i < _nbPairs; i++)
	{
		const physx::PxContactPair& contactPair = _pairs[i];

		if (contactPair.flags & (physx::PxContactPairFlag::eREMOVED_SHAPE_0 | physx::PxContactPairFlag::eREMOVED_SHAPE_1))
			continue;

		Collider* a = static_cast<Collider*>(contactPair.shapes[0]->userData);
		Collider* b = static_cast<Collider*>(contactPair.shapes[1]->userData);

		if (contactPair.events & (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND))
		{
			if (a && b)
			{
				a->GetOwner().lock()->OnCollisionEnter(b);
				b->GetOwner().lock()->OnCollisionEnter(a);
			}
		}
		if (contactPair.events & (physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS))
		{
			if (a && b)
			{
				a->GetOwner().lock()->OnCollisionStay(b);
				b->GetOwner().lock()->OnCollisionStay(a);
			}
		}
		if (contactPair.events & (physx::PxPairFlag::eNOTIFY_TOUCH_LOST))
		{
			if (a && b)
			{
				a->GetOwner().lock()->OnCollisionExit(b);
				b->GetOwner().lock()->OnCollisionExit(a);
			}
		}
	}
}


void Truth::PxEventCallback::onTrigger(physx::PxTriggerPair* _pairs, physx::PxU32 _count)
{
	for (uint32 i = 0; i < _count; i++)
	{
		const physx::PxTriggerPair& triggerPair = _pairs[i];

		if (triggerPair.flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		Collider* a = nullptr;
		Collider* b = nullptr;

		if (triggerPair.triggerShape)
   			a = static_cast<Collider*>(triggerPair.triggerShape->userData);

		if (triggerPair.otherShape)
			b = static_cast<Collider*>(triggerPair.otherShape->userData);


		if (triggerPair.status & (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND))
		{
			if (a && b)
			{
				a->GetOwner().lock()->OnTriggerEnter(b);
				b->GetOwner().lock()->OnTriggerEnter(a);
			}
		}
		else if (triggerPair.status & (physx::PxPairFlag::eNOTIFY_TOUCH_LOST))
		{
			if (a && b)
			{
  				a->GetOwner().lock()->OnTriggerExit(b);
				b->GetOwner().lock()->OnTriggerExit(a);
			}
		}
	}
}

void Truth::PxEventCallback::onWake(physx::PxActor** _actors, physx::PxU32 _count)
{
}

void Truth::PxEventCallback::onSleep(physx::PxActor** _actors, physx::PxU32 _count)
{
}

void Truth::PxEventCallback::onAdvance(const physx::PxRigidBody* const* _bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 _count)
{
}

void Truth::PxEventCallback::onConstraintBreak(physx::PxConstraintInfo* _constraints, physx::PxU32 _count)
{
}
