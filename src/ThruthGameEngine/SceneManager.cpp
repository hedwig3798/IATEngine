#include "SceneManager.h"
#include "Scene.h"
#include "ParticleManager.h"

/// <summary>
/// 생성자
/// </summary>
Truth::SceneManager::SceneManager()
{
	DEBUG_PRINT("Create Scene Manager\n");
}

/// <summary>
/// 소멸자
/// </summary>
Truth::SceneManager::~SceneManager()
{
	DEBUG_PRINT("Finalize SceneManager\n");
}

/// <summary>
/// 초기화
/// </summary>
/// <param name="_mangers"></param>
void Truth::SceneManager::Initalize(std::shared_ptr<Managers> _mangers)
{
	m_mangers = _mangers;
	m_eventManager = _mangers->Event();
}

/// <summary>
/// 현재 scene 지정
/// </summary>
/// <param name="_name">업데이트 될 Scene 지정</param>
void Truth::SceneManager::SetCurrnetScene(std::shared_ptr<Scene> _scene)
{
	m_eventManager.lock()->RemoveAllEvents();
	if (m_currentScene != nullptr)
	{
		m_currentScene->Exit();
		m_currentScene.reset();
	}
	m_currentScene = _scene;
	m_currentScene->Enter();
}

/// <summary>
/// 명시적 소멸자
/// </summary>
void Truth::SceneManager::Finalize()
{
	m_currentScene->Exit();
}

/// <summary>
/// 업데이트
/// </summary>
void Truth::SceneManager::Update()
{
	if (m_sceneChangeFlag)
	{
		m_sceneChangeFlag = false;
		ChangeScene();
	}

	m_currentScene->Update();
#ifdef EDITOR_MODE
	m_currentScene->EditorUpdate();
#endif // EDITOR_MODE
}

void Truth::SceneManager::FixedUpdate() const
{
	m_currentScene->FixedUpdate();
}

void Truth::SceneManager::LateUpdate() const
{
	m_currentScene->LateUpdate();
}

/// <summary>
/// Transform 일시 적용
/// </summary>
void Truth::SceneManager::ApplyTransform() const
{
	m_currentScene->ApplyTransform();
}

/// <summary>
/// Scene 시작
/// </summary>
void Truth::SceneManager::StartGameScene() const
{
	m_currentScene->Enter();
}

/// <summary>
/// Scene 변경
/// </summary>
/// <param name="_p">변경할 Scene</param>
void Truth::SceneManager::ChangeScene(const std::string& _name)
{
	m_sceneChangeFlag = true;
	m_nextSceneName = _name;
}

void Truth::SceneManager::ResetScene()
{
	ChangeScene(m_currentScene->m_name);
}

void Truth::SceneManager::ChangeScene()
{
	m_eventManager.lock()->RemoveAllEvents();
	
	m_mangers.lock()->Particle()->StopAllParticle();

	m_currentScene->Exit();
	m_currentScene.reset();

	std::ifstream inputstream(m_savedFilePath + m_nextSceneName + ".scene");
	boost::archive::text_iarchive inputArchive(inputstream);
	std::shared_ptr<Truth::Scene> s;
	inputArchive >> s;

	m_currentScene = s;
	m_currentScene->Initalize(m_mangers);
	m_currentScene->Enter();

	m_mangers.lock()->Time()->RestartTime();
	m_mangers.lock()->Input()->m_fpsMode = s->m_useNavMesh;
}

/// <summary>
/// Scene 리셋
/// </summary>
/// <param name="_p"></param>
void Truth::SceneManager::ResetScene() const
{
	m_currentScene->Exit();
	m_currentScene->Enter();
}

/// <summary>
/// 현재 Scene 저장
/// </summary>
void Truth::SceneManager::SaveCurrentScene() const
{
	std::ofstream outputstream(m_savedFilePath + m_currentScene->m_name + ".scene");
	boost::archive::text_oarchive outputArchive(outputstream);
	outputArchive << m_currentScene;
}

/// <summary>
/// Scene 저장
/// </summary>
void Truth::SceneManager::SaveScene(std::shared_ptr<Scene> _scene) const
{
	std::ofstream outputstream(m_savedFilePath + _scene->m_name + ".scene");
	boost::archive::text_oarchive outputArchive(outputstream);
	outputArchive << _scene;
}

void Truth::SceneManager::SaveAsScene(std::wstring& _path) const
{
	std::ofstream outputstream(_path);
	boost::archive::text_oarchive outputArchive(outputstream);
	outputArchive << m_currentScene;
}

/// <summary>
/// Scene 로드
/// 현재 Scene으로 지정 하지는 않는다.
/// </summary>
/// <param name="_path">Scene 파일 경로</param>
void Truth::SceneManager::LoadSceneData(std::wstring _path)
{
	std::ifstream inputstream(_path);
	boost::archive::text_iarchive inputArchive(inputstream);
	std::shared_ptr<Truth::Scene> s;
	inputArchive >> s;

	if (m_currentScene != nullptr)
	{
		m_currentScene->Exit();
		m_currentScene.reset();
	}

	m_currentScene = s;
 	m_currentScene->Initalize(m_mangers);
 	m_currentScene->Enter();
}

void Truth::SceneManager::ReloadSceneData()
{
	std::string sceneName = m_currentScene->m_name;
	std::ifstream inputstream(m_savedFilePath + sceneName + ".scene");
	boost::archive::text_iarchive inputArchive(inputstream);
	std::shared_ptr<Truth::Scene> s;
	inputArchive >> s;

	if (m_currentScene != nullptr)
	{
		m_currentScene->Exit();
		m_currentScene.reset();
	}

	m_currentScene = s;
 	m_currentScene->Initalize(m_mangers);
 	m_currentScene->Enter();
}
