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

	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	public:
		BoxCollider(bool _isTrigger = true);
		BoxCollider(Vector3 _size, bool _isTrigger = true);
		BoxCollider(Vector3 _pos, Vector3 _size, bool _isTrigger = true);

		METHOD(Initialize);
		void Initialize();
	};

	template<class Archive>
	void Truth::BoxCollider::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Collider>(*this);
	}

	template<class Archive>
	void Truth::BoxCollider::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Collider>(*this);
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::BoxCollider)
BOOST_CLASS_VERSION(Truth::BoxCollider, 0)