#include "Managers.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "Eventmanager.h"
#include "SceneManager.h"
#include "PhysicsManager.h"
#include "GraphicsManager.h"
#include "EditorCamera.h"
#include "ParticleManager.h"
#include "SoundManager.h"
#include "ComponentFactory.h"
#include <time.h>

fs::path Truth::Managers::ROOT_PATH = fs::path();

Truth::Managers::Managers()
{
	DEBUG_PRINT("Create Managers\n");
	ROOT_PATH = fs::current_path();
}

Truth::Managers::~Managers()
{
	DEBUG_PRINT("Finalize Managers\n");
}

void Truth::Managers::Initialize(HINSTANCE _hinstance, HWND _hwnd, uint32 _width, uint32 _height)
{
	CreateManagers();
	InitlizeManagers(_hinstance, _hwnd, _width, _height);
	m_componentFactory = std::make_unique<ComponentFactory>();
#ifdef EDITOR_MODE
	m_editorCamera = std::make_shared<EditorCamera>(this);
	m_editorCamera->SetMainCamera();
#endif // EDITOR_MODE
}

void Truth::Managers::Update()
{
	m_inputManager->Update();
#ifdef EDITOR_MODE
	if (m_inputManager->GetKeyState(KEY::F9) == KEY_STATE::DOWN)
	{
		if (m_isEdit)
		{
			EditToGame();
		}
		else
		{
			GameToEdit();
		}
	}
#endif // EDITOR_MODE
	m_timeManager->Update();
#ifdef EDITOR_MODE
	if (!m_isEdit)
	{
		m_soundManager->Update();
		m_physXManager->Update();
		m_sceneManager->Update(); 
		m_eventManager->Update();
		m_particleManager->Update();
	}
	else
	{
		m_editorCamera->SetMainCamera();
		m_editorCamera->Update(m_timeManager->GetDT());
	}
#else
	LARGE_INTEGER start, finish, frameCounter;
	::QueryPerformanceCounter(&start);
	::QueryPerformanceFrequency(&frameCounter);

	m_soundManager->Update();
	m_physXManager->Update();
	m_sceneManager->Update();
	m_eventManager->Update();
	m_particleManager->Update();

	::QueryPerformanceCounter(&finish);
	std::string temp = std::to_string(static_cast<float>(finish.QuadPart - start.QuadPart) / static_cast<float>(frameCounter.QuadPart));
	temp = std::string("Update : ") + temp;
	temp += " / ";
	DEBUG_PRINT(temp.c_str());
#endif // EDITOR_MODE

}

void Truth::Managers::LateUpdate() const
{
#ifdef EDITOR_MODE
	if (!m_isEdit)
	{
		m_sceneManager->LateUpdate();
		m_eventManager->LateUpdate();
	}
#else
	m_sceneManager->LateUpdate();
	m_eventManager->LateUpdate();
#endif // EDITOR_MODE
}

void Truth::Managers::FixedUpdate() const
{
	static float at = 0.0f;
	at += m_timeManager->GetADT();
	// DEBUG_PRINT(std::to_string(at).c_str());
	// DEBUG_PRINT("\n");
	m_sceneManager->FixedUpdate();
	m_physXManager->FixedUpdate();
	m_eventManager->FixedUpdate();
}

void Truth::Managers::Render() const
{
	LARGE_INTEGER start, finish, frameCounter;
	::QueryPerformanceCounter(&start);
	::QueryPerformanceFrequency(&frameCounter);

	m_sceneManager->ApplyTransform();
#ifdef EDITOR_MODE
	if (m_isEdit)
		m_graphicsManager->Render();
	else
		m_graphicsManager->Render();
#else
	m_graphicsManager->Render();
#endif // EDITOR_MODE
	::QueryPerformanceCounter(&finish);
	std::string temp = std::to_string(static_cast<float>(finish.QuadPart - start.QuadPart) / static_cast<float>(frameCounter.QuadPart));
	temp = std::string("Render : ") + temp;
	temp += " \n ";
	// DEBUG_PRINT(temp.c_str());

}

void Truth::Managers::Finalize()
{
	m_sceneManager->Finalize();
	m_sceneManager.reset();

	m_inputManager->Finalize();
	m_inputManager.reset();

	m_timeManager->Finalize();
	m_timeManager.reset();

	m_eventManager->Finalize();
	m_eventManager.reset();

	m_physXManager->Finalize();
	m_physXManager.reset();

	m_soundManager->Finalize();
	m_soundManager.reset();

	m_particleManager->Finalize();
	m_particleManager.reset();

	m_graphicsManager->Finalize();
	m_graphicsManager.reset();
}


void Truth::Managers::EndGame()
{
	::PostQuitMessage(0);
}

#ifdef EDITOR_MODE
void Truth::Managers::EditToGame()
{
	if (!m_isEdit)
	{
		return;
	}
	m_sceneManager->SaveCurrentScene();
	m_sceneManager->m_currentScene->Start();
	m_isEdit = false;
	m_inputManager->m_fpsMode = m_sceneManager->m_currentScene->m_useNavMesh;
}

void Truth::Managers::GameToEdit()
{
	if (m_isEdit)
	{
		return;
	}
	m_physXManager->ResetPhysX();
	m_sceneManager->ReloadSceneData();
	m_isEdit = true;
	m_inputManager->m_fpsMode = false;
	m_graphicsManager->ResetMainCamera();
}
#endif // EDITOR_MODE

void Truth::Managers::CreateManagers()
{
	// TimeManager* temp = new TimeManager();
	m_timeManager = std::make_shared<TimeManager>();
	m_inputManager = std::make_shared<InputManager>();
	m_eventManager = std::make_shared<EventManager>();
	m_sceneManager = std::make_shared<SceneManager>();
	m_physXManager = std::make_shared<PhysicsManager>();
	m_graphicsManager = std::make_shared<GraphicsManager>();
	m_particleManager = std::make_shared<ParticleManager>();
	m_soundManager = std::make_shared<SoundManager>();
}

void Truth::Managers::InitlizeManagers(HINSTANCE _hinstance, HWND _hwnd, uint32 _width, uint32 _height)
{
	m_soundManager->Initalize();
	m_eventManager->Initialize(m_timeManager, m_physXManager);
	m_timeManager->Initalize(shared_from_this());
	m_inputManager->Initalize(_hinstance, _hwnd, m_eventManager);
	m_graphicsManager->Initalize(_hwnd, _width, _height);
	m_sceneManager->Initalize(shared_from_this());
	m_physXManager->Initalize();
	m_particleManager->Initalize(m_graphicsManager, m_timeManager);
}
