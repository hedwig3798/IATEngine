#pragma once
#include "Headers.h"
#include "EventHandler.h"
#include "Entity.h"

namespace Ideal
{
	class IMeshObject;
}

class EditorUI;

/// <summary>
/// 게임이 실제로 돌아가는 씬
/// </summary>
namespace Truth
{
	class Managers;
	class NavMeshGenerater;

	class Scene final
		: public EventHandler
	{
		GENERATE_CLASS_TYPE_INFO(Scene);

		friend class LoadMapDataPopup;

	public:
		PROPERTY(name);
		std::string m_name;

		std::wstring m_mapPath;

		std::shared_ptr<NavMeshGenerater> m_navMesh;
		std::vector<std::shared_ptr<Entity>> m_mapEntity;

		bool m_useNavMesh = true;

		bool m_isFpsScene = false;

		float m_brightness = 0.4f;

		fs::path m_skyBox;

		bool m_started = false;

	public:
		typedef std::vector<std::shared_ptr<Entity>> EntityVector;
		PROPERTY(entities);
		EntityVector m_entities;
		EntityVector m_rootEntities;

		std::weak_ptr<Managers> m_managers;  

	private:
		std::queue<std::shared_ptr<Entity>> m_beginDestroy;
		std::queue<std::shared_ptr<Entity>> m_finishDestroy;

		std::queue<std::shared_ptr<Entity>> m_awakedEntity;
		std::queue<std::shared_ptr<Entity>> m_startedEntity;

	public:
		Scene(std::shared_ptr<Managers> _managers);
		Scene() = default;
		virtual ~Scene();

		// 후에 씬에 오브젝트가 추가되어 다음 프레임부터 Update 시작
		void AddEntity(std::shared_ptr<Entity> _p);
		void CreateEntity(std::shared_ptr<Entity> _p);

		void DeleteEntity(uint32 _index);
		void DeleteEntity(std::shared_ptr<Entity> _p);

		void Initalize(std::weak_ptr<Managers> _manager);

		void LoadEntity(std::shared_ptr<Entity> _entity);

		Vector3 FindPath(Vector3 _start, Vector3 _end, Vector3 _size) const;

		std::weak_ptr<Entity> FindEntity(std::string _name);
		std::weak_ptr<Entity> FindEntity(uint32 _index);


#ifdef EDITOR_MODE
		void EditorUpdate();
#endif // EDITOR_MODE

		void Update();
		void FixedUpdate();
		void LateUpdate();

		void ApplyTransform();

		void ResizeWindow();

		void Start();

		void Enter();
		void Exit();

		void ClearEntity();

		void SetBrightness();

	private:
		void LoadUnityData(const std::wstring& _path);
		void ResetMapData();

	};
}
