#pragma once
#include "Headers.h"
#include "Component.h"

namespace Truth
{
	class Transform :
		public Truth::Component
	{
		GENERATE_CLASS_TYPE_INFO(Transform);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);


	public:
		Vector3 m_worldPosition;
		Vector3 m_worldScale;
		Quaternion m_worldRotation;

		PROPERTY(position);
		Vector3 m_position;
		PROPERTY(scale);
		Vector3 m_scale;
		PROPERTY(rotation);
		Quaternion m_rotation;


		Matrix m_localTM;
		Matrix m_globalTM;

		Vector3 m_look;

	public:
		Transform();
		virtual ~Transform();

		// 변환 내용 적용
		void ApplyTransform();

	public:

#pragma region Inline
#pragma region Transform
		// 변환 작업 (더하기)
		inline void Translate(const Vector3& _movement)
		{
			m_position += _movement;
			m_localTM.Translation(_movement);
		}

		inline void AddRotate(const Vector3& _dgree)
		{
			m_rotation *= Quaternion::CreateFromYawPitchRoll(_dgree);
			ApplyTransform();
		}
		inline void AddRotate(const Quaternion& _dgree)
		{
			m_rotation *= _dgree;
			ApplyTransform();
		}

		inline void AddScale(const Vector3& _rate)
		{
			m_scale.x *= _rate.x; m_scale.y *= _rate.y; m_scale.z *= _rate.z;
			ApplyTransform();
		}
		inline void AddScale(float4 _rate)
		{
			m_scale *= _rate;
			ApplyTransform();
		}

		// 변환 작업 (고정값)
		inline void SetPosition(const Vector3& _position)
		{
			m_position = _position;
			ApplyTransform();
		}

		inline void SetRotate(const Vector3& _dgree)
		{
			m_rotation = Quaternion::CreateFromYawPitchRoll(_dgree);
			ApplyTransform();
		}
		inline void SetRotate(const Quaternion& _dgree)
		{
			m_rotation = _dgree;
			ApplyTransform();
		}

		inline void SetScale(const Vector3& _rate)
		{
			m_scale = _rate;
			ApplyTransform();
		}
		inline void SetScale(float4 _rate)
		{
			m_scale = { _rate, _rate, _rate };
			ApplyTransform();
		}

		inline void SetLocalTM(Matrix _tm)
		{
			m_localTM = _tm;
			_tm.Decompose(m_scale, m_rotation, m_position);
			// ApplyTransform();
		}

		inline void SetWorldRotation(const Quaternion& _rotation)
		{

			Vector3 pos, sca;
			m_worldRotation = _rotation;
			m_globalTM = Matrix::CreateScale(m_worldScale);
			m_globalTM *= Matrix::CreateFromQuaternion(m_worldRotation);
			m_globalTM *= Matrix::CreateTranslation(m_worldPosition);

			if (HasParent())
				m_localTM = m_globalTM * GetParentMatrix().Invert();
			else
				m_localTM = m_globalTM;

			m_localTM.Decompose(sca, m_rotation, pos);
		}
#pragma endregion Transform

		// 오일러각 제공 
		inline Vector3 GetEulerRotation() const
		{
			return m_rotation.ToEuler();
		}
#pragma endregion Inline
	};

	template<class Archive>
	void Truth::Transform::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_position;
		_ar& m_scale;
		_ar& m_rotation;
		_ar& m_localTM;
		_ar& m_look;
		if (file_version >= 1)
		{
			_ar& m_globalTM;
		}
	}

	template<class Archive>
	void Truth::Transform::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_position;
		_ar& m_scale;
		_ar& m_rotation;
		_ar& m_localTM;
		_ar& m_look;

		_ar& m_globalTM;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::Transform)
BOOST_CLASS_VERSION(Truth::Transform, 1)