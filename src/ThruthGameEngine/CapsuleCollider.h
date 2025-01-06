#pragma once
#include "Collider.h"
#include "Headers.h"

namespace Truth
{
	class CapsuleCollider
		: public Collider
	{
		GENERATE_CLASS_TYPE_INFO(CapsuleCollider);
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
		PROPERTY(height);
		float m_height;

	public:
		CapsuleCollider(bool _isTrigger = true);
		CapsuleCollider(float _radius, float _height, bool _isTrigger = true);
		CapsuleCollider(Vector3 _pos, float _radius, float _height, bool _isTrigger = true);

		METHOD(SetRadius);
		void SetRadius(float _radius);
		METHOD(SetHeight);
		void SetHeight(float _height);

		METHOD(Initialize);
		void Initialize();
	};

	template<class Archive>
	void Truth::CapsuleCollider::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Collider>(*this);
		_ar& m_radius;
		_ar& m_height;
	}

	template<class Archive>
	void Truth::CapsuleCollider::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Collider>(*this);
		_ar& m_radius;
		_ar& m_height;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::CapsuleCollider)
BOOST_CLASS_VERSION(Truth::CapsuleCollider, 0)