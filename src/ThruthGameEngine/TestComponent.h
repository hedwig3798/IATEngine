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

}
