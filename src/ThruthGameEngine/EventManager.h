#pragma once
#include "Headers.h"

namespace Truth
{
	class EventHandler;
	class TimeManager;
	class PhysicsManager;
	class Entity;
}

/// �̺�Ʈ ������ ���� ���Ǵ� ����ü
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
/// �̺�Ʈ ������ ����ü
/// ������ Ŭ���� : �̺�Ʈ�� �߻� �� �� ������ ���� ������ Ŭ����
/// �̺�Ʈ �Լ� : �̺�Ʈ �߻� �� ������ �Լ�. ���ϰ��� �׻� void
/// �̺�Ʈ �Լ��� private ���� ��� ����
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
/// �̺�Ʈ�� �����ϴ� �Ŵ���
/// �������� �̺�Ʈ�� �����ϰ� ���δ�.
/// 
/// �̺�Ʈ : ���� �Լ�
/// �̺�ƮID : �ش� �̺�Ʈ�� �̸�
/// ������ : �ش� �̺�Ʈ�� ������ Ŭ���� (������Ʈ)
/// ���� : �ش� �̺�Ʈ�� �߻��� �� Ư�� �ൿ�� ��
/// </summary>


namespace Truth
{
	class EventManager
	{
	private:
		// �̺�Ʈ�� ���� �˻��� ���� �ڷᱸ��
		struct EventSearch
		{
			std::list<Truth::ListenerInfo>& m_listRef;
			std::list<Truth::ListenerInfo>::iterator m_itr;
			const std::string m_eventID;
		};

		std::queue<Event> m_customEvents;
		std::queue<std::shared_ptr<Entity>> m_createdEntity;
		std::queue<std::shared_ptr<Entity>> m_deletedEntity;

		// �̺�Ʈ�� �� �̺�Ʈ�� ������ ������
		std::unordered_map<std::string, std::list<Truth::ListenerInfo>> m_listeners;

		// Ư�� �������� ���Ÿ� ���� � �����ʰ� ��� ��ġ�� �ִ��� ���
		std::unordered_map<const Truth::EventHandler*, std::list<EventSearch>> m_eventHandlerInfo;

		std::weak_ptr<Truth::TimeManager> m_timeManager;
		std::weak_ptr<Truth::PhysicsManager> m_physicsManager;

	public:
		EventManager();
		~EventManager();

	public:
		// ������
		void Initialize(std::weak_ptr<Truth::TimeManager> _timeManager, std::weak_ptr<Truth::PhysicsManager> _physicsManager);
		// ������Ʈ
		void Update();
		void LateUpdate() {}
		void FixedUpdate();
		// �Ҹ�
		void Finalize();

	public:
		// �̺�Ʈ ���� (_eventID�� �ش��ϴ� �̺�Ʈ�� �߻��ߴٰ� �˸�)
		void PublishEvent(std::string _eventID, const void* _param = nullptr, const float& _delayed = 0.0f);
		// �̺�Ʈ ����(Ư�� �̺�Ʈ�� �߻��ϴ� ���� ��������)
		void Subscribe(std::string _eventID, const Truth::ListenerInfo& _listenerInfo);

		// ���� ���
		void Unsubscribe(std::string _eventID, const Truth::EventHandler* _listener);
		// ��� �̺�Ʈ���� Ư�� ������ ����
		void RemoveListener(const Truth::EventHandler* _listener);
		// Ư�� �̺�Ʈ���� ��� ������ ����
		void RemoveListenersAtEvent(std::string _eventID);
		// ��� �̺�Ʈ ����
		void RemoveAllEvents();
		// ��� ���� Ŭ���� ����
		void RemoveAllSubscribes();

	private:
		// �̺�Ʈ ����
		void ProcessEvent();
		// �̺�Ʈ �Լ� ����
		void DispatchEvent(const Event& _event);
		// ���� ���� Ȯ��
		bool CheckSubscribe(std::string _eventID, const Truth::EventHandler* _listener);
	};
}

