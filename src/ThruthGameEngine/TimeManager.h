#pragma once
#include "Headers.h"

/// <summary>
/// time managing class
/// </summary>
namespace Truth
{
	class EventManager;
	class Managers;

	class TimeManager
	{
	private:
		// ���α׷� ���� �ð�
		float m_time;
		// �ð� ����
		float m_timeScale;
		// ������ �ð�
		float m_deltaTime;
		// ���� ������ �ð�
		float m_fixedDeltaTime;
		// ���� ������ ���� �ð�
		const float m_fixedTime = 1.0f / 60.0f;
		// ���� ������ �ð�
		float m_absDeltaTime;

		float m_waitUntilTime;
		float m_waitingTime;

		LARGE_INTEGER m_currentCount;
		LARGE_INTEGER m_prevCount;
		LARGE_INTEGER m_frequency;

		std::weak_ptr<EventManager> m_eventManager;
		std::weak_ptr<Managers> m_managers;

	public:
		TimeManager();
		~TimeManager();

		void Initalize(std::shared_ptr<Managers> _managers);
		void Update();
		void Finalize();

		float4 GetDT() const { return m_deltaTime; }
		float4 GetFDT() const { return m_fixedTime; }
		float4 GetADT() const { return m_absDeltaTime; }
		float4 GetProcessTime() const { return m_time; }
		void WaitForSecondsRealtime(float time);
		void SetTimeScaleForSeconds(float scale, float time);

		void RestartTime();
	};

}
