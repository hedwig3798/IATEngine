#include "SoundManager.h"
#include "fmod.hpp"

Truth::SoundManager::SoundManager()
	: m_system(nullptr)
	, m_channel{}
{
	FMOD_RESULT result;
	result = FMOD::System_Create(&m_system);
	assert(result == FMOD_OK && "cannnot create fmod system");
}

Truth::SoundManager::~SoundManager()
{

}

void Truth::SoundManager::Initalize()
{
	FMOD_RESULT result;
	result = m_system->init(MAX_CHANNEL, FMOD_INIT_NORMAL, 0);
	assert(result == FMOD_OK && "cannnot initalize fmod system");

}

void Truth::SoundManager::Update()
{
	m_system->update();
}

void Truth::SoundManager::Finalize()
{
	for (auto& s : m_soundMap)
	{
		s.second->release();
	}
	m_system->release();
}

void Truth::SoundManager::CreateSound(fs::path _path, bool _isLoop)
{
	try
	{
		std::unordered_map<fs::path, FMOD::Sound*>::iterator itr = m_soundMap.find(_path);
		if (itr != m_soundMap.end())
			return;

		FMOD_RESULT result;
		if (_isLoop == true)
		{
			result = m_system->createSound(_path.generic_string().c_str(), FMOD_LOOP_NORMAL, 0, &m_soundMap[_path]);
		}
		else
			result = m_system->createSound(_path.generic_string().c_str(), FMOD_LOOP_OFF, 0, &m_soundMap[_path]);
	}
	catch (std::exception& e)
	{
		DEBUG_PRINT(e.what());
	}
}

void Truth::SoundManager::SetVolum(int _channel, float _vol)
{
	try
	{
		FMOD_RESULT result;

		result = m_channel[_channel]->setVolume(_vol);
	}
	catch (std::exception& e)
	{
		DEBUG_PRINT(e.what());
	}
}

void Truth::SoundManager::Play(fs::path _path, bool _canReduplication, int _channel)
{
	try
	{
		std::unordered_map<fs::path, FMOD::Sound*>::iterator itr = m_soundMap.find(_path);
		if (itr == m_soundMap.end())
			return;
		FMOD_RESULT result;

		bool isPlaying = false;
		m_channel[_channel]->isPlaying(&isPlaying);

		if (_canReduplication)
			result = m_system->playSound((*itr).second, nullptr, false, &m_channel[_channel]);
		else if (!_canReduplication && !isPlaying)
			result = m_system->playSound((*itr).second, nullptr, false, &m_channel[_channel]);
	}
	catch (std::exception& e)
	{
		DEBUG_PRINT(e.what());
	}
}

void Truth::SoundManager::Pause(int _channel)
{
	FMOD_RESULT result;

	try
	{
		bool isPaused;
		result = m_channel[_channel]->getPaused(&isPaused);
		if (!isPaused)
		{
			result = m_channel[_channel]->setPaused(true);
		}
	}
	catch (std::exception& e)
	{
		DEBUG_PRINT(e.what());
	}
}

void Truth::SoundManager::Resume(int _channel)
{
	FMOD_RESULT result;

	try
	{
		bool isPaused;
		result = m_channel[_channel]->getPaused(&isPaused);
		if (isPaused)
		{
			result = m_channel[_channel]->setPaused(false);
		}
	}
	catch (std::exception& e)
	{
		DEBUG_PRINT(e.what());
	}
}

void Truth::SoundManager::Stop(int _channel)
{
	FMOD_RESULT result;

	try
	{
		bool isPlay;
		m_channel[_channel]->isPlaying(&isPlay);
		if (isPlay)
		{
			result = m_channel[_channel]->stop();
		}
	}
	catch (std::exception& e)
	{
		DEBUG_PRINT(e.what());
	}
}

void Truth::SoundManager::CheckResult(FMOD_RESULT _result)
{
	if (_result != FMOD_RESULT::FMOD_OK)
		__debugbreak();
}