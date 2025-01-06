#pragma once
#include "Headers.h"
#include "EventHandler.h"
#include "Scene.h"
namespace Truth
{
	class Scene;
	class EventManager;
	class Managers;
}

namespace Truth
{
	class SceneManager
		: public EventHandler
	{
	private:
		std::weak_ptr<EventManager> m_eventManager;
		std::weak_ptr<Managers> m_mangers;
		const std::string m_savedFilePath = "../Scene/";

		bool m_sceneChangeFlag = false;
		std::string m_nextSceneName;
	public:
		std::shared_ptr<Scene> m_currentScene;

		SceneManager();
		~SceneManager();

		void Initalize(std::shared_ptr<Managers> _mangers);

		void Update();
		void FixedUpdate() const;
		void LateUpdate() const;

		void ApplyTransform() const;

		void StartGameScene() const;

		void ChangeScene(const std::string& _name);
		void ResetScene();
		void ChangeScene();

		void ResetScene() const;
		void SetCurrnetScene(std::shared_ptr<Scene> _scene);

		void Finalize();

		void SaveCurrentScene() const;
		void SaveScene(std::shared_ptr<Scene> _scene) const;
		void SaveAsScene(std::wstring& _path) const;
		void LoadSceneData(std::wstring _path);
		void ReloadSceneData();
	};

}

