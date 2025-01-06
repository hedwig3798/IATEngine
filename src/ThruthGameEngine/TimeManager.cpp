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

	// 현재 카운트
	::QueryPerformanceCounter(&m_prevCount);

	// 초당 카운트 횟수 (천만)
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
/// 시간 업데이트
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

	// 이전 프레임의 카운팅과 현재 프레임 카운팅 값의 차이를 구한다.
	::QueryPerformanceCounter(&m_currentCount);
	float delta = static_cast<float>(m_currentCount.QuadPart - m_prevCount.QuadPart) /
		static_cast<float>(m_frequency.QuadPart);

	// 디버깅시에는 최소 프레임 제한
#ifdef EDITOR_MODE
// 	if (delta > 0.16f)
// 	{
// 		delta = 0.016f;
// 	}
#endif // _DEBUG

	m_absDeltaTime = delta;

	// 현재 프레임 시간
	m_deltaTime = delta * m_timeScale;
	// 고정 프레임 시간
	m_fixedDeltaTime += delta * m_timeScale;

#ifdef EDITOR_MODE
	if (m_managers.lock()->m_isEdit)
		m_fixedDeltaTime = 0.0f;
#endif // _DEBUG

	// 만일 고정 프레임 시간이 단위를 넘기게 되면 이벤트를 발행한다.
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

		// 시간 조절
		m_fixedDeltaTime -= m_fixedTime;
	}

	// 전체 시간
	m_time += m_absDeltaTime;

	// 이전카운트 값을 현재값으로 갱신 (다음번에 계산을 위해서)
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
