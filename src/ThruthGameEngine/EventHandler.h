#pragma once
#include "EventManager.h"
namespace Truth
{
	class EventHandler
	{
	public:
		// EventSystem �� ����ϱ� ���� Callable ����
		template <typename T>
		ListenerInfo MakeListenerInfo(void (T::* func)(const void*))
		{
			// ��ȿ�� �˻�
			static_assert(std::is_base_of_v<EventHandler, T>, "T must be derived from EventHandler");
			// Callable ��ȯ (���� ������ ListenerInfo ����ü ����)
			return
				ListenerInfo(this, [this, func](const void* handler)
					{
						(static_cast<T*>(this)->*func)(handler);
					}
			);
		}
	};
}

