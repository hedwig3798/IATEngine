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
/// 초기화
/// </summary>
void Truth::EventManager::Initialize(std::weak_ptr<TimeManager> _timeManager, std::weak_ptr<Truth::PhysicsManager> _physicsManager)
{
	m_timeManager = _timeManager;
	m_physicsManager = _physicsManager;
}

/// <summary>
/// 업데이트
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
/// 초기화
/// </summary>
void Truth::EventManager::Finalize()
{
	RemoveAllEvents();
	RemoveAllSubscribes();
}

/// <summary>
/// 이벤트 발행
/// </summary>
/// <param name="_eventID">이벤트 이름</param>
/// <param name="_param">이벤트 함수의 인자 혹은 인자 묶음. 기본 없음</param>
/// <param name="_delayed">딜레이 시간 기본 0.0f</param>
void Truth::EventManager::PublishEvent(std::string _eventID, const void* _param /*= nullptr*/, const float& _delayed /*= 0.0f*/)
{
	m_customEvents.push(Event{ _eventID, _param, _delayed });
}

/// <summary>
/// 이벤트 구독
/// </summary>
/// <param name="_eventID">구독 할 이벤트 아이디</param>
/// <param name="_listenerInfo">이벤트를 구독할 클래스 정보 구조체</param>
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
/// 특정 클래스가 특정 이벤트를 구독 해제
/// </summary>
/// <param name="_eventID">구독 해제할 아이디</param>
/// <param name="_listener">구독 해제할 클래스 정보 구조체</param>
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
/// 특정 리스너를 모든 이벤트에서 제거
/// </summary>
/// <param name="_listener">제거할 클래스</param>
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
/// 특정 이벤트에서 모든 리스너를 제거
/// </summary>
/// <param name="_eventID">제거할 이벤트</param>
void Truth::EventManager::RemoveListenersAtEvent(std::string _eventID)
{
	m_listeners[_eventID].clear();
}

/// <summary>
/// 모든 이벤트 제거
/// </summary>
void Truth::EventManager::RemoveAllEvents()
{
	while (!m_customEvents.empty())
	{
		m_customEvents.pop();
	}
}

/// <summary>
/// 모든 구독자 제거
/// </summary>
void Truth::EventManager::RemoveAllSubscribes()
{
	m_listeners.clear();
	m_eventHandlerInfo.clear();
}

/// <summary>
/// 이벤트 실행
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
/// 이벤트 함수의 실행
/// </summary>
/// <param name="_event">이벤트 정보</param>
void Truth::EventManager::DispatchEvent(const Event& _event)
{
	for (auto& listner : m_listeners[_event.m_eventID])
	{
		listner.EventFunction(_event.m_parameter);
	}
}

/// <summary>
/// 구독 정보 확인
/// </summary>
/// <param name="_eventID">이벤트 아이디</param>
/// <param name="_listener">리스너 클래스</param>
/// <returns>구독 여부</returns>
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
