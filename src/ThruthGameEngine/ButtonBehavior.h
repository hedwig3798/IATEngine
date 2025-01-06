#pragma once
#include "Headers.h"
#include "EventHandler.h"

namespace Truth
{
	class Managers;
	class UI;
	class Entity;
	class TextUI;
}
namespace Truth
{
	class ButtonBehavior
		: public EventHandler
	{
		GENERATE_CLASS_TYPE_INFO(ButtonBehavior);

	protected:
		std::weak_ptr<Managers> m_managers;
		std::weak_ptr<UI> m_UI;
		std::weak_ptr<TextUI> m_TextUI;
		std::weak_ptr<Entity> m_owner;

	public:
		std::string m_name;

	public:
		ButtonBehavior();
		virtual ~ButtonBehavior();

	public:
		virtual void Initialize(std::weak_ptr<Managers> _managers, std::weak_ptr<UI> _UI, std::weak_ptr<Entity> _owner);
		virtual void Initialize(std::weak_ptr<Managers> _managers, std::weak_ptr<TextUI> _UI, std::weak_ptr<Entity> _owner);
		virtual void OnMouseClick() {};
		virtual void OnMouseUp() {};
		virtual void OnMouseOver() {};
		virtual void Update() {};
		virtual void Awake() {};
		virtual void Start() {};
	};
}
