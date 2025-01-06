#include "TimeManager.h"
#include "Managers.h"
#include "EventManager.h"

Truth::TimeManager::TimeManager()
	: m_deltaTime(0.0f)
	, m_time(0.0f)
	, m_timeScale(1.0f)
	, m_fixedDeltaTime(0.0f)
	, m_currentCount{}
	, m_prevCount{}
	, m_frequency{}
	, m_eventManager()
	, m_managers()
	, m_absDeltaTime(0.0f)
	, m_waitUntilTime(0.0f)
	, m_waitingTime(0.0f)
{
	DEBUG_PRINT("Create TimeManager\n");

	// ���� ī��Ʈ
	::QueryPerformanceCounter(&m_prevCount);

	// �ʴ� ī��Ʈ Ƚ�� (õ��)
	::QueryPerformanceFrequency(&m_frequency);
}

Truth::TimeManager::~TimeManager()
{
	DEBUG_PRINT("Finalize TimeManager\n");
}

void Truth::TimeManager::Initalize(std::shared_ptr<Managers> _managers)
{
	m_managers = _managers;
	m_eventManager = m_managers.lock()->Event();
}

/// <summary>
/// �ð� ������Ʈ
/// </summary>
void Truth::TimeManager::Update()
{
	if (m_waitUntilTime > 0.f)
		m_waitingTime += m_absDeltaTime;
	if (m_waitUntilTime < m_waitingTime)
	{
		m_timeScale = 1.0f;
		m_waitUntilTime = 0.0f;
		m_waitingTime = 0.0f;
	}

	// ���� �������� ī���ð� ���� ������ ī���� ���� ���̸� ���Ѵ�.
	::QueryPerformanceCounter(&m_currentCount);
	float delta = static_cast<float>(m_currentCount.QuadPart - m_prevCount.QuadPart) /
		static_cast<float>(m_frequency.QuadPart);

	// �����ÿ��� �ּ� ������ ����
#ifdef EDITOR_MODE
// 	if (delta > 0.16f)
// 	{
// 		delta = 0.016f;
// 	}
#endif // _DEBUG

	m_absDeltaTime = delta;

	// ���� ������ �ð�
	m_deltaTime = delta * m_timeScale;
	// ���� ������ �ð�
	m_fixedDeltaTime += delta * m_timeScale;

#ifdef EDITOR_MODE
	if (m_managers.lock()->m_isEdit)
		m_fixedDeltaTime = 0.0f;
#endif // _DEBUG

	// ���� ���� ������ �ð��� ������ �ѱ�� �Ǹ� �̺�Ʈ�� �����Ѵ�.
	while (m_fixedDeltaTime >= m_fixedTime)
	{
// 		LARGE_INTEGER start, finish, frameCounter;
// 		::QueryPerformanceCounter(&start);
// 		::QueryPerformanceFrequency(&frameCounter);

		m_managers.lock()->FixedUpdate();

// 		::QueryPerformanceCounter(&finish);
// 		std::string temp = std::to_string(static_cast<float>(finish.QuadPart - start.QuadPart) / static_cast<float>(frameCounter.QuadPart));
// 		temp = std::string("Fixedupdate : ") + temp;
// 		temp += " / ";
// 		DEBUG_PRINT(temp.c_str());

		// �ð� ����
		m_fixedDeltaTime -= m_fixedTime;
	}

	// ��ü �ð�
	m_time += m_absDeltaTime;

	// ����ī��Ʈ ���� ���簪���� ���� (�������� ����� ���ؼ�)
	m_prevCount = m_currentCount;
}

void Truth::TimeManager::Finalize()
{

}

void Truth::TimeManager::WaitForSecondsRealtime(float time)
{
	if (time > 0.f && m_waitUntilTime == 0.f)
	{
		m_waitUntilTime = time;
		m_timeScale = 0.0f;
		m_waitingTime = 0.0f;
	}
}

void Truth::TimeManager::SetTimeScaleForSeconds(float scale, float time)
{
	if (time > 0.f && m_waitUntilTime == 0.f)
	{
		m_waitUntilTime = time;
		m_timeScale = scale;
		m_waitingTime = 0.0f;
	}
}

void Truth::TimeManager::RestartTime()
{
	::QueryPerformanceCounter(&m_prevCount);
}
