#pragma once
#include "EditorDefine.h"

#ifdef EDITOR_MODE


#include "Headers.h"
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

class TFileUtils;

namespace Truth
{
	class GraphicsManager;
}

namespace Truth
{
	/// <summary>
	/// 이 클래스는 Debug 모드에서만 작동하도록 만들 예정
	/// 오직 에디터에서만 작동하는 클래스
	/// 해당 클래스로 얻은 정보는 에디터상에서 편집 불가능
	/// </summary>
	class UnityParser
	{
	private:
		/// <summary>
		/// Node 정보를 저장하는 구조체
		/// </summary>
		struct UnityNodeFormat
		{
			std::string m_fileID;
			std::string m_classID;
			YAML::Node m_node;
		};

		/// <summary>
		/// guid와 파일 정보를 저장하는 구조체
		/// </summary>
		struct UnityFileFormat
		{
			std::string m_guid;
			fs::path m_metaPath;
			fs::path m_filePath;
		};

		struct MatarialData
		{
			std::string m_name;
			fs::path m_albedo;
			fs::path m_normal;
			fs::path m_metalicRoughness;
			
			float m_tileX = 1.0f;
			float m_tileY = 1.0f;

			bool m_alphaCulling = false;
			bool m_transparent = false;
		};

		/// <summary>
		/// unity game object tree
		/// 이를 기반으로 Entity Tree를 재구성
		/// </summary>
		struct GameObject
		{
			// Collider Info
			bool m_isCollider = false;
			int32 m_shape = 0;
			std::vector<Vector3> m_size;
			std::vector<Vector3> m_center;
			bool m_isConvex = false;
			
			// Mesh Filter Info
			bool m_isMesh = false;
			std::string m_meshPath = "";
			std::vector<std::string> m_matarialsName;

			bool m_isLight = false;
			uint32 m_type;
			// same thigins
			float m_intensity;
			Color m_color;

			float m_range;
			float m_angle;


			std::string m_guid = "";
			std::string m_fileID = "";

			int32 m_parent = -1;
			int32 m_mine = -1;
			std::string m_name;

			// Transform Info
			Vector3 m_position;
			Vector3 m_scale;
			Quaternion m_rotation;

			bool m_localPosChange[3] = {0, };
			bool m_localScaleChange[3] = {0, };
			bool m_localRotationChange = false;
		};

		uint32 m_meshFilterCount = 0;
		uint32 m_lightCount = 0;

		std::vector<GameObject*> m_gameObjects;

		std::set <fs::path> m_unLoadMesh;

		std::vector<Vector3> m_boxVertex;
		std::vector<uint32> m_boxindx;

		std::set<std::string> m_ignore;

		// guid, data
		std::map<std::string, UnityFileFormat*> m_guidMap;
		// path, data
		std::map<fs::path, UnityFileFormat*> m_pathMap;

		// guid, fileID, node
		std::map<std::string, std::map<std::string, UnityNodeFormat*>> m_nodeMap;
		// guid, classID, vector<node>
		std::map<std::string, std::map<std::string, std::vector<UnityNodeFormat*>>> m_classMap;
		// guid, matarialdata
		std::map<std::string, MatarialData> m_matarialMap;
		std::map<std::string, MatarialData> m_copiedMatarialMap;

		const fs::path m_defaultPath = "../Resources/MapData/";
		const fs::path m_assetPath = "../Resources/Assets/MapData/";
		const fs::path m_modelPath = "../Resources/Models/MapData/";
		const fs::path m_texturePath = "../Resources/Textures/MapData/";
		const fs::path m_matSavePath = "../Resources/Matarial/";
		const fs::path m_debugCubePath = "DebugObject/debugCube";
		fs::path m_sceneName;
		
		const std::wstring m_convertPath = L"MapData/";
		const std::string m_sconvertPath = "MapData/";
		

		GraphicsManager* m_gp;

		fs::path m_rootPath;

	public:
		UnityParser(GraphicsManager* _gp);
		~UnityParser();

		void SetRootDir(const std::string& _path);
		void ParseUnityFile(const std::string& _path);

		void Reset();

	private:
		void ParseFile(fs::path& _path);
		void ParseDir(fs::path& _path);

		void ParseYAMLFile(YAML::Node& _node, const std::string& _guid);

		fs::path OrganizeUnityFile(fs::path& _path);

		void ParseTranfomrNode(const YAML::Node& _node, const std::string& _guid, uint32 _parent);
		void ParsePrefabNode(const YAML::Node& _node, const std::string& _guid, uint32 _parent);
		
		void ReadPrefabFile(const fs::path& _path, GameObject* _parent);

		void ParseGameObject(const std::string& _guid, const YAML::Node& _node, GameObject* _owner);
		void ParseBoxCollider(const YAML::Node& _node, GameObject* _owner);
		void ParseMeshCollider(const YAML::Node& _node, GameObject* _owner);
		void ParseMeshFilter(const YAML::Node& _node, GameObject* _owner);
		void ParseLight(const YAML::Node& _node, GameObject* _owner);

		void GetPrefabMatrix(const YAML::Node& _node, GameObject* _owner);
		void GetPrefabMatarial(GameObject* _GO, const YAML::Node& _node);

		void ParseFbxMetaFile(GameObject* _GO, const fs::path& _fbxPath);

		void ParseMatarialFile(GameObject* _GO, const std::string& _matGuid);
		void CopyTexture(const YAML::Node& _node, fs::path& _output);

		void WriteMaterialData();

		void ConvertUnloadedMesh();

		void WriteData();
		void WriteColliderData(std::shared_ptr<TFileUtils> _file, GameObject* _GO);
		void WriteMeshData(std::shared_ptr<TFileUtils> _file, GameObject* _GO);
		void WriteLightData(std::shared_ptr<TFileUtils> _file, GameObject* _GO);
		void WriteLocalTMData(std::shared_ptr<TFileUtils> _file, GameObject* _GO);
	};
}

#endif // EDITOR_MODE
