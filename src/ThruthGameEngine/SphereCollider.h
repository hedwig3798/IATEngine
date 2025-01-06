#pragma once
#include "Collider.h"
#include "Headers.h"

namespace Truth
{
	class SphereCollider :
		public Collider
	{
		GENERATE_CLASS_TYPE_INFO(SphereCollider);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);
	public:
		PROPERTY(radius);
		float m_radius;

	public:
		SphereCollider(bool _isTrigger = true);
		SphereCollider(float _radius, bool _isTrigger = true);
		SphereCollider(Vector3 _pos, float _radius, bool _isTrigger = true);

		METHOD(SetRadius);
		void SetRadius(float _radius);

		METHOD(Initialize);
		void Initialize();
	};

	template<class Archive>
	void Truth::SphereCollider::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Collider>(*this);
		_ar& m_radius;
	}

	template<class Archive>
	void Truth::SphereCollider::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Collider>(*this);
		_ar& m_radius;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::SphereCollider)
BOOST_CLASS_VERSION(Truth::SphereCollider, 0)