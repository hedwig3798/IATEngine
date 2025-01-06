#pragma once
#include "Headers.h"
#include "Component.h"

/// <summary>
/// 디버깅용 테스트 컴포넌트 
/// </summary>
namespace Truth
{
	class TestComponent :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(TestComponent);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	public:
		PROPERTY(testF);
		float m_testF;

		PROPERTY(testV3);
		Vector3 m_testV3;

		PROPERTY(testS)
		std::string m_testS;

	public:
		TestComponent();
		virtual	~TestComponent();

		METHOD(Awake);
		void Awake();

	public:
		METHOD(Update);
		void Update();
	};

	template<class Archive>
	void Truth::TestComponent::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_testF;
	}

	template<class Archive>
	void Truth::TestComponent::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_testF;
	}
}
BOOST_CLASS_EXPORT_KEY(Truth::TestComponent)
BOOST_CLASS_VERSION(Truth::TestComponent, 0)