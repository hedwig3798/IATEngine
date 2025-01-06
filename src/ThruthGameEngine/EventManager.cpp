#include "EventManager.h"
#include "Managers.h"
#include "TimeManager.h"
#include "EventHandler.h"
#include "Entity.h"
#include "SceneManager.h"
#include "PhysicsManager.h"

Truth::EventManager::EventManager()
{
	DEBUG_PRINT("Create EventManager\n");
}

Truth::EventManager::~EventManager()
{
	DEBUG_PRINT("Finalize EventManager\n");
	Finalize();
}

/// <summary>
/// �ʱ�ȭ
/// </summary>
void Truth::EventManager::Initialize(std::weak_ptr<TimeManager> _timeManager, std::weak_ptr<Truth::PhysicsManager> _physicsManager)
{
	m_timeManager = _timeManager;
	m_physicsManager = _physicsManager;
}

/// <summary>
/// ������Ʈ
/// </summary>
void Truth::EventManager::Update()
{
	ProcessEvent();
}

void Truth::EventManager::FixedUpdate()
{
	ProcessEvent();
}

/// <summary>
/// �ʱ�ȭ
/// </summary>
void Truth::EventManager::Finalize()
{
	RemoveAllEvents();
	RemoveAllSubscribes();
}

/// <summary>
/// �̺�Ʈ ����
/// </summary>
/// <param name="_eventID">�̺�Ʈ �̸�</param>
/// <param name="_param">�̺�Ʈ �Լ��� ���� Ȥ�� ���� ����. �⺻ ����</param>
/// <param name="_delayed">������ �ð� �⺻ 0.0f</param>
void Truth::EventManager::PublishEvent(std::string _eventID, const void* _param /*= nullptr*/, const float& _delayed /*= 0.0f*/)
{
	m_customEvents.push(Event{ _eventID, _param, _delayed });
}

/// <summary>
/// �̺�Ʈ ����
/// </summary>
/// <param name="_eventID">���� �� �̺�Ʈ ���̵�</param>
/// <param name="_listenerInfo">�̺�Ʈ�� ������ Ŭ���� ���� ����ü</param>
void Truth::EventManager::Subscribe(std::string _eventID, const ListenerInfo& _listenerInfo)
{
	if (_listenerInfo.m_listener == nullptr ||
		CheckSubscribe(_eventID, _listenerInfo.m_listener))
	{
		return;
	}
	m_listeners[_eventID].push_front(_listenerInfo);

	m_eventHandlerInfo[_listenerInfo.m_listener].push_back(
		EventSearch
		{
			m_listeners[_eventID],
			m_listeners[_eventID].begin(),
			_eventID
		}
	);

	EventSearch es = { m_listeners[_eventID], m_listeners[_eventID].begin(), _eventID };
}

/// <summary>
/// Ư�� Ŭ������ Ư�� �̺�Ʈ�� ���� ����
/// </summary>
/// <param name="_eventID">���� ������ ���̵�</param>
/// <param name="_listener">���� ������ Ŭ���� ���� ����ü</param>
void Truth::EventManager::Unsubscribe(std::string _eventID, const EventHandler* _listener)
{
	for (auto& l : m_eventHandlerInfo[_listener])
	{
		if (l.m_eventID == _eventID)
		{
			l.m_listRef.erase(l.m_itr);
		}
	}
}

/// <summary>
/// Ư�� �����ʸ� ��� �̺�Ʈ���� ����
/// </summary>
/// <param name="_listener">������ Ŭ����</param>
void Truth::EventManager::RemoveListener(const EventHandler* _listener)
{
	for (auto& l : m_eventHandlerInfo[_listener])
	{
		DEBUG_PRINT("%s : event %s removed\n", l.m_eventID.c_str());
		l.m_listRef.erase(l.m_itr);
	}
	m_eventHandlerInfo[_listener].clear();
}

/// <summary>
/// Ư�� �̺�Ʈ���� ��� �����ʸ� ����
/// </summary>
/// <param name="_eventID">������ �̺�Ʈ</param>
void Truth::EventManager::RemoveListenersAtEvent(std::string _eventID)
{
	m_listeners[_eventID].clear();
}

/// <summary>
/// ��� �̺�Ʈ ����
/// </summary>
void Truth::EventManager::RemoveAllEvents()
{
	while (!m_customEvents.empty())
	{
		m_customEvents.pop();
	}
}

/// <summary>
/// ��� ������ ����
/// </summary>
void Truth::EventManager::RemoveAllSubscribes()
{
	m_listeners.clear();
	m_eventHandlerInfo.clear();
}

/// <summary>
/// �̺�Ʈ ����
/// </summary>
void Truth::EventManager::ProcessEvent()
{
	while (!m_customEvents.empty())
	{
		Event e = m_customEvents.front();
		m_customEvents.pop();

		e.m_delayedTime -= m_timeManager.lock()->GetDT();
		if (e.m_delayedTime <= 0.0f)
		{
			DispatchEvent(e);
		}
		else
		{
			m_customEvents.push(e);
		}
	}
}

/// <summary>
/// �̺�Ʈ �Լ��� ����
/// </summary>
/// <param name="_event">�̺�Ʈ ����</param>
void Truth::EventManager::DispatchEvent(const Event& _event)
{
	for (auto& listner : m_listeners[_event.m_eventID])
	{
		listner.EventFunction(_event.m_parameter);
	}
}

/// <summary>
/// ���� ���� Ȯ��
/// </summary>
/// <param name="_eventID">�̺�Ʈ ���̵�</param>
/// <param name="_listener">������ Ŭ����</param>
/// <returns>���� ����</returns>
bool Truth::EventManager::CheckSubscribe(std::string _eventID, const EventHandler* _listener)
{
	auto& eventList = m_eventHandlerInfo[_listener];

	for (auto& e : eventList)
	{
		if (_eventID == e.m_eventID)
		{
			return true;
		}
	}
	return false;
}
