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

	private:
		friend class boost::serialization::access;
		friend class UI;

		BOOST_SERIALIZATION_SPLIT_MEMBER();

		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

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

	template<class Archive>
	void Truth::ButtonBehavior::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& m_name;
	}

	template<class Archive>
	void Truth::ButtonBehavior::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& m_name;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::ButtonBehavior)
BOOST_CLASS_VERSION(Truth::ButtonBehavior, 0)