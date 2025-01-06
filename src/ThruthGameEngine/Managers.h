#pragma once
#include "Headers.h"

namespace Truth
{
	class TimeManager;
	class InputManager;
	class EventManager;
	class SceneManager;
	class PhysicsManager;
	class GraphicsManager;
	class EditorCamera;
	class ParticleManager;
	class SoundManager;
	class Mesh;
	class ComponentFactory;
}

namespace Truth
{
	class Managers
		: public std::enable_shared_from_this<Managers>
	{
	private:
		static fs::path ROOT_PATH;
		HWND m_hwnd;
	public:

		std::shared_ptr<TimeManager> m_timeManager;
		std::shared_ptr<InputManager> m_inputManager;
		std::shared_ptr<EventManager> m_eventManager;
		std::shared_ptr<SceneManager> m_sceneManager;
		std::shared_ptr<PhysicsManager> m_physXManager;
		std::shared_ptr<GraphicsManager> m_graphicsManager;
		std::shared_ptr<ParticleManager> m_particleManager;
		std::shared_ptr<SoundManager> m_soundManager;

		std::unique_ptr<ComponentFactory> m_componentFactory;

#ifdef EDITOR_MODE
		bool m_isEdit = true;
	private:
		std::shared_ptr<EditorCamera> m_editorCamera;
#endif // EDITOR_MODE

	public:
		Managers();
		~Managers();

	public:
		void Initialize(HINSTANCE _hinstance, HWND _hwnd, uint32 _width, uint32 _height);
		void Update();
		void LateUpdate() const;
		void FixedUpdate() const;
		void Render() const;

		void Finalize();

		void EndGame();

#ifdef EDITOR_MODE
		void EditToGame();
		void GameToEdit();
#endif // EDITOR_MODE
		inline std::shared_ptr<Truth::TimeManager> Time() const { return m_timeManager; };
		inline std::shared_ptr<Truth::InputManager> Input() const { return m_inputManager; };
		inline std::shared_ptr<Truth::EventManager> Event() const { return m_eventManager; };
		inline std::shared_ptr<Truth::SceneManager> Scene() const { return m_sceneManager; };
		inline std::shared_ptr<Truth::PhysicsManager> Physics() const { return m_physXManager; };
		inline std::shared_ptr<Truth::GraphicsManager> Graphics() const { return m_graphicsManager; };
		inline std::shared_ptr<Truth::ParticleManager> Particle() const { return m_particleManager; };
		inline std::shared_ptr<Truth::SoundManager> Sound() const { return m_soundManager; };

		inline static const fs::path& GetRootPath() { return ROOT_PATH; };

	private:
		void CreateManagers();
		void InitlizeManagers(HINSTANCE _hinstance, HWND _hwnd, uint32 _width, uint32 _height);
	};
}

