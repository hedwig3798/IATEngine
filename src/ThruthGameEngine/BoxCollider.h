#pragma once
#include "Collider.h"

namespace physx
{
	class PxShape;
}

namespace Truth
{
	class BoxCollider :
		public Collider
	{
		GENERATE_CLASS_TYPE_INFO(BoxCollider);

	public:
		BoxCollider(bool _isTrigger = true);
		BoxCollider(Vector3 _size, bool _isTrigger = true);
		BoxCollider(Vector3 _pos, Vector3 _size, bool _isTrigger = true);

		METHOD(Initialize);
		void Initialize();
	};
}
