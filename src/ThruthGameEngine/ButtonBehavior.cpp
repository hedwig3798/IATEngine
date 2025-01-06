#include "ButtonBehavior.h"
BOOST_CLASS_EXPORT_IMPLEMENT(Truth::ButtonBehavior)

Truth::ButtonBehavior::ButtonBehavior()
	: m_managers()
	, m_UI()
	, m_TextUI()
	, m_owner()
{

}

Truth::ButtonBehavior::~ButtonBehavior()
{

}

void Truth::ButtonBehavior::Initialize(std::weak_ptr<Managers> _managers, std::weak_ptr<UI> _UI, std::weak_ptr<Entity> _owner)
{
	m_managers = _managers;
	m_UI = _UI;
	m_owner = _owner;
}

void Truth::ButtonBehavior::Initialize(std::weak_ptr<Managers> _managers, std::weak_ptr<TextUI> _UI, std::weak_ptr<Entity> _owner)
{
	m_managers = _managers;
	m_TextUI = _UI;
	m_owner = _owner;
}
