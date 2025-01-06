#include "TestComponent.h"
#include "Managers.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::TestComponent)

Truth::TestComponent::TestComponent()
	: Component()
{
	m_canMultiple = true;
	m_name = typeid(*this).name();
	m_testF = 10.0f;
	m_testV3 = { 1.0f, 2.0f, 3.0f };
	m_testS = "test";
}


Truth::TestComponent::~TestComponent()
{
}

void Truth::TestComponent::Awake()
{
}

void Truth::TestComponent::Update()
{
	int a = 1;
}

