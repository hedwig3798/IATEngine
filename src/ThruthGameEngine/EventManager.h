#pragma once
#include "Headers.h"

namespace Truth
{
	class EventHandler;
	class TimeManager;
	class PhysicsManager;
	class Entity;
}

/// 이벤트 실행을 위해 사용되는 구조체
namespace Truth
{
	struct Event
	{
		std::string m_eventID;
		const void* m_parameter;
		float m_delayedTime;

		Event(std::string _id, const void* _param, const float& _delayed)
			: m_eventID(_id), m_parameter(_param), m_delayedTime(_delayed) {}
	};
}

/// <summary>
/// 이벤트 리스너 구조체
/// 리스너 클래스 : 이벤트가 발생 할 때 영향을 받을 리스너 클래스
/// 이벤트 함수 : 이벤트 발생 시 실행할 함수. 리턴값은 항상 void
/// 이벤트 함수는 private 여도 상관 없다
/// </summary>
namespace Truth
{
	struct ListenerInfo
	{
		const EventHandler* m_listener = nullptr;
		std::function<void(const void*)> EventFunction;

		ListenerInfo(const Truth::EventHandler* _listener, const std::function<void(const void*)>& _func)
			: m_listener(_listener), EventFunction(_func) {}
	};
}

/// <summary>
/// 이벤트를 관리하는 매니저
/// 동적으로 이벤트를 생성하고 붙인다.
/// 
/// 이벤트 : 실행 함수
/// 이벤트ID : 해당 이벤트의 이름
/// 리스너 : 해당 이벤트를 구독한 클래스 (컴포넌트)
/// 구독 : 해당 이벤트가 발생할 때 특정 행동을 함
/// </summary>


namespace Truth
{
	class EventManager
	{
	private:
		// 이벤트의 빠른 검색을 위한 자료구조
		struct EventSearch
		{
			std::list<Truth::ListenerInfo>& m_listRef;
			std::list<Truth::ListenerInfo>::iterator m_itr;
			const std::string m_eventID;
		};

		std::queue<Event> m_customEvents;
		std::queue<std::shared_ptr<Entity>> m_createdEntity;
		std::queue<std::shared_ptr<Entity>> m_deletedEntity;

		// 이벤트와 그 이벤트를 구독한 리스너
		std::unordered_map<std::string, std::list<Truth::ListenerInfo>> m_listeners;

		// 특정 리스너의 제거를 위해 어떤 리스너가 어디에 위치해 있는지 기록
		std::unordered_map<const Truth::EventHandler*, std::list<EventSearch>> m_eventHandlerInfo;

		std::weak_ptr<Truth::TimeManager> m_timeManager;
		std::weak_ptr<Truth::PhysicsManager> m_physicsManager;

	public:
		EventManager();
		~EventManager();

	public:
		// 생성자
		void Initialize(std::weak_ptr<Truth::TimeManager> _timeManager, std::weak_ptr<Truth::PhysicsManager> _physicsManager);
		// 업데이트
		void Update();
		void LateUpdate() {}
		void FixedUpdate();
		// 소멸
		void Finalize();

	public:
		// 이벤트 발행 (_eventID에 해당하는 이벤트가 발생했다고 알림)
		void PublishEvent(std::string _eventID, const void* _param = nullptr, const float& _delayed = 0.0f);
		// 이벤트 구독(특정 이벤트가 발생하는 것을 보고있음)
		void Subscribe(std::string _eventID, const Truth::ListenerInfo& _listenerInfo);

		// 구독 취소
		void Unsubscribe(std::string _eventID, const Truth::EventHandler* _listener);
		// 모든 이벤트에서 특정 리스너 제거
		void RemoveListener(const Truth::EventHandler* _listener);
		// 특정 이벤트에서 모든 리스너 제거
		void RemoveListenersAtEvent(std::string _eventID);
		// 모든 이벤트 제거
		void RemoveAllEvents();
		// 모든 구독 클래스 제거
		void RemoveAllSubscribes();

	private:
		// 이벤트 실행
		void ProcessEvent();
		// 이벤트 함수 실행
		void DispatchEvent(const Event& _event);
		// 구독 여부 확인
		bool CheckSubscribe(std::string _eventID, const Truth::EventHandler* _listener);
	};
}

