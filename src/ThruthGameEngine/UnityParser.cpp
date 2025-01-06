#include "UnityParser.h"
#ifdef EDITOR_MODE

#include <fstream>
#include <iostream>
#include "TFileUtils.h"
#include "GraphicsManager.h"

#include "Entity.h"
#include "Component.h"
#include "BoxCollider.h"

/// <summary>
/// 파서 생성자
/// </summary>
/// <param name="_gp">그래픽 엔진 (컨버팅 용)</param>
Truth::UnityParser::UnityParser(GraphicsManager* _gp)
	:m_gp(_gp)
{
	m_ignore.insert(".vs");
	m_ignore.insert("Library");
	m_ignore.insert("Logs");
	m_ignore.insert("obj");
	m_ignore.insert("Packages");
	m_ignore.insert("ProjectSettings");
	m_ignore.insert("UserSettings");
}

Truth::UnityParser::~UnityParser()
{
	Reset();
}

/// <summary>
/// unity 파일을 파싱해서 필요한 정보만 가져오기
/// </summary>
/// <param name="_path">유니티 프로젝트의 루트 디렉토리</param>
void Truth::UnityParser::SetRootDir(const std::string& _path)
{
	m_rootPath = _path;

	if (!fs::exists(m_rootPath))
	{
		assert(false && "Wrong File or Directory Path");
		return;
	}
	if (fs::is_directory(m_rootPath))
	{
		ParseDir(m_rootPath);
	}
	else if (fs::is_regular_file(m_rootPath))
	{
		ParseFile(m_rootPath);
	}
}

/// <summary>
/// meta 파일만 따로 파싱하는 함수
/// </summary>
/// <param name="_path">파일 경로</param>
void Truth::UnityParser::ParseFile(fs::path& _path)
{
	// 파일이 아닌 경우 리턴
	if (!fs::is_regular_file(_path))
	{
		assert(false && "Not File Path");
		return;
	}

	// 확장자가 meta가 아닌 경우 리턴
	std::string extnName = _path.extension().generic_string();
	if (extnName != ".meta")
	{
		return;
	}

	// 파일을 열고 실패시 리턴
	std::ifstream fin(_path);
	if (!fin.is_open())
	{
		assert(false && "Cannot Open Meta File");
	}

	// 해당 파일에서 guid만 뽑아서 저장
	// 다른 정보는 아직 필요가 없다
	// 버전 관리까지 하지 않기 떄문
	std::string line;
	while (fin)
	{
		getline(fin, line);

		StringConverter::DeleteAlpha(line, ' ');
		std::vector<std::string> sLine = StringConverter::split(line, ':');
		if (!sLine.empty() && sLine[0] == "guid")
		{
			UnityFileFormat* uf = new UnityFileFormat{
				sLine[1],
				_path,
				_path.replace_extension()
			};


			m_guidMap[uf->m_guid] = uf;
			m_pathMap[uf->m_filePath] = uf;

			break;
		}
	}


}

/// <summary>
/// 디렉토리 파싱
/// </summary>
/// <param name="_path">경로</param>
void Truth::UnityParser::ParseDir(fs::path& _path)
{
	// 디렉토리가 아닌 경우 리턴 
	if (!fs::is_directory(_path))
	{
		assert(false && "Not Directory Path");
		return;
	}

	// 무시하는 디렉토리인 경우 리턴
	// unity에 있는 설정 정보는 필요가 없다.
	if (m_ignore.find(_path.filename().generic_string()) != m_ignore.end())
	{
		return;
	}

	// 해당 디렉토리에 있는 폴더와 파일
	std::queue<fs::path> dirQueue;
	std::queue<fs::path> fileQueue;

	// 해당 디렉토리에 있는 모든 파일 정보를 가져온다.
	fs::directory_iterator itr(_path);
	while (itr != fs::end(itr))
	{
		const fs::directory_entry& entry = *itr;

		fs::path childPath = entry.path();

		if (fs::is_directory(childPath))
		{
			dirQueue.push(childPath);
		}
		else if (fs::is_regular_file(childPath))
		{
			fileQueue.push(childPath);
		}
		itr++;
	}

	// 파일과 디렉토리를 각자 파싱한다.
	while (!fileQueue.empty())
	{
		ParseFile(fileQueue.front());
		fileQueue.pop();
	}

	while (!dirQueue.empty())
	{
		ParseDir(dirQueue.front());
		dirQueue.pop();
	}
}

/// <summary>
/// Yaml로 이루어진 unity 파일 파싱
/// </summary>
/// <param name="_node">Yaml Node</param>
void Truth::UnityParser::ParseYAMLFile(YAML::Node& _node, const std::string& _guid)
{
	// Tag에 있는 정보를 가져와서 파싱한다.
	std::string data = _node.Tag();

	// Tag중 앞부분은 필요가 없다
	auto sData = StringConverter::split(data, ':');
	// 숫자 정보만 가져와서 각각의 ID를 가져와서 저장한다.
	auto ssData = StringConverter::split(sData.back(), '&');
	UnityNodeFormat* unf = new UnityNodeFormat{ ssData[1], ssData[0], _node };
	m_classMap[_guid][ssData[0]].push_back(unf);
	m_nodeMap[_guid][ssData[1]] = unf;

}


/// <summary>
/// Yaml unity 파일 정리
/// Yaml-cpp 라이브러리를 사용해서 읽을 수 없는 정보를 정제하고
/// anchor에 접근이 불가능하므로 이를 보완한다.
/// 이때 원본 파일을 훼손하지 않고 정제된 버전의 파일을 복사해서 가져온다.
/// </summary>
/// <param name="_path">경로</param>
/// <returns>정제된 복사본 파일 경로</returns>
fs::path Truth::UnityParser::OrganizeUnityFile(fs::path& _path)
{
	// unity 와 prefab 파일만 읽는다 일단은
	if (_path.extension() != ".unity" && _path.extension() != ".prefab")
	{
		assert(false && "Not Unity Scene");
		return "";
	}

	// 별도로 저장할 파일 위치
	fs::path cscene = "../UnityData/" + _path.filename().generic_string();
	fs::create_directories(cscene.parent_path());

	// 파일을 열고 성공 여부를 체크
	std::ifstream fin(_path);
	std::ofstream fout(cscene);
	if (!fin.is_open())
	{
		assert(false && "Cannot Open Scene File");
	}
	if (!fout.is_open())
	{
		assert(false && "Cannot Open Duplicate Scene File");
	}

	// 파일 정제
	std::string line;
	bool isStripped = false;
	while (fin)
	{
		getline(fin, line);
		if (isStripped)
		{
			line = "S" + line;
			isStripped = false;
		}

		// stripped 단어의 경우 읽을 수 없으므로 제거한다.
		auto sLine = StringConverter::split(line, ' ');
		if (!sLine.empty() && sLine.back() == "stripped")
		{
			sLine.pop_back();
			line = "";
			for (auto& s : sLine)
			{
				line += s + " ";
			}
			line.pop_back();
			isStripped = true;
		}
		// fileID를 읽어 올 수 없어서 정제한다. 
		if (!sLine.empty() && sLine[0] == "---")
		{
			line = sLine[0] + " " + sLine[1] + sLine[2];
		}

		fout << line;
		fout << "\n";
	}

	fin.close();
	fout.close();

	return cscene;
}

/// <summary>
/// Transformd은 모든 GameObject에 있으므로 해당 노드를 기점으로 GameObject를 파악한다.
/// 부모에서 자식으로 이어지는 트리 구조를 재귀적으로 파악한다.
/// </summary>
/// <param name="_node">현 노드</param>
/// <param name="_guid">guid</param>
/// <param name="_parent">부모 (root의 경우 null)</param>
/// <returns>현 노드에 해당하는 game Object 구조체</returns>
void Truth::UnityParser::ParseTranfomrNode(const YAML::Node& _node, const std::string& _guid, uint32 _parent)
{
	GameObject* GO = new GameObject;
	GO->m_mine = static_cast<uint32>(m_gameObjects.size());
	GO->m_parent = _parent;
	m_gameObjects.push_back(GO);
	GO->m_isCollider = false;
	GO->m_guid = _guid;


	// get transform data
	GO->m_position = {
		_node["m_LocalPosition"]["x"].as<float>(),
		_node["m_LocalPosition"]["y"].as<float>(),
		_node["m_LocalPosition"]["z"].as<float>()
	};

	GO->m_rotation = {
		_node["m_LocalRotation"]["x"].as<float>(),
		_node["m_LocalRotation"]["y"].as<float>(),
		_node["m_LocalRotation"]["z"].as<float>(),
		_node["m_LocalRotation"]["w"].as<float>(),
	};

	GO->m_scale = {
		_node["m_LocalScale"]["x"].as<float>(),
		_node["m_LocalScale"]["y"].as<float>(),
		_node["m_LocalScale"]["z"].as<float>()
	};

	for (uint32 i = 0; i < 3; i++)
	{
		GO->m_localPosChange[i] = true;
		GO->m_localScaleChange[i] = true;
	}

	GO->m_localRotationChange = true;

	// get game object
	std::string GOfid = _node["m_GameObject"]["fileID"].as<std::string>();
	GO->m_fileID = GOfid;

	const YAML::Node& GONode = m_nodeMap[_guid][GOfid]->m_node;
	if (GONode["GameObject"].IsDefined())
	{
		ParseGameObject(_guid, GONode["GameObject"], GO);
	}

	// make child
	YAML::Node children = _node["m_Children"];
	for (YAML::iterator it = children.begin(); it != children.end(); ++it)
	{
		std::string childFid = (*it)["fileID"].as<std::string>();
		if (m_nodeMap[_guid][childFid]->m_node["Transform"].IsDefined())
		{
			ParseTranfomrNode(m_nodeMap[_guid][childFid]->m_node["Transform"], _guid, GO->m_mine);
		}
		else if (m_nodeMap[_guid][childFid]->m_node["STransform"].IsDefined())
		{
			std::string prefabFID = m_nodeMap[_guid][childFid]->m_node["STransform"]["m_PrefabInstance"]["fileID"].as<std::string>();
			ParsePrefabNode(m_nodeMap[_guid][prefabFID]->m_node["PrefabInstance"], _guid, GO->m_mine);
		}

	}
	return;
}

void Truth::UnityParser::ParsePrefabNode(const YAML::Node& _node, const std::string& _guid, uint32 _parent)
{
	GameObject* GO = new GameObject;
	GO->m_parent = _parent;
	GO->m_mine = static_cast<uint32>(m_gameObjects.size());
	m_gameObjects.push_back(GO);
	const std::string& prefabGuid = _node["m_SourcePrefab"]["guid"].as<std::string>();
	GO->m_guid = prefabGuid;

	// get transform data
	GetPrefabMatrix(_node["m_Modification"]["m_Modifications"], GO);
	GetPrefabMatarial(GO, _node["m_Modification"]["m_Modifications"]);

	if (m_guidMap.find(prefabGuid) != m_guidMap.end())
	{
		const fs::path& prefabPath = m_guidMap[prefabGuid]->m_filePath;

		if (prefabPath.extension() == ".prefab")
		{
			ReadPrefabFile(prefabPath, GO);
		}
		else if (prefabPath.extension() == ".fbx" || prefabPath.extension() == ".FBX")
		{
			ParseFbxMetaFile(GO, prefabPath);
		}
	}

	const YAML::Node& components = _node["m_Modification"]["m_AddedComponents"];
	if (components.IsDefined() && components.IsSequence())
	{
		for (YAML::const_iterator it = components.begin(); it != components.end(); ++it)
		{
			const YAML::Node& component = *it;
			std::string cFid = component["addedObject"]["fileID"].as<std::string>();

			const YAML::Node& boxc = m_nodeMap[_guid][cFid]->m_node["BoxCollider"];
			const YAML::Node& meshc = m_nodeMap[_guid][cFid]->m_node["MeshCollider"];
			/// find box collider
			if (boxc.IsDefined())
			{
				ParseBoxCollider(boxc, GO);
			}
			else if (meshc.IsDefined())
			{
				ParseMeshCollider(meshc, GO);
			}
		}
	}
}

void Truth::UnityParser::ParseGameObject(const std::string& _guid, const YAML::Node& _node, GameObject* _owner)
{
	// get component list
	const YAML::Node& comList = _node["m_Component"];
	_owner->m_name = _node["m_Name"].as<std::string>();
	for (YAML::const_iterator it = comList.begin(); it != comList.end(); ++it)
	{
		// get component list
		const YAML::Node& comp = *it;
		std::string compFid = comp["component"]["fileID"].as<std::string>();


		/// find box collider
		const YAML::Node& collider = m_nodeMap[_guid][compFid]->m_node["BoxCollider"];
		if (collider.IsDefined())
		{
			ParseBoxCollider(collider, _owner);
		}

		/// find Light
		const YAML::Node& light = m_nodeMap[_guid][compFid]->m_node["Light"];
		if (light.IsDefined())
		{
			ParseLight(light, _owner);
		}
	}
}

void Truth::UnityParser::ParseBoxCollider(const YAML::Node& _node, GameObject* _owner)
{
	_owner->m_isCollider = true;
	_owner->m_shape = 1;
	_owner->m_size.push_back({
		_node["m_Size"]["x"].as<float>(),
		_node["m_Size"]["y"].as<float>(),
		_node["m_Size"]["z"].as<float>(),
		});

	_owner->m_center.push_back({
		_node["m_Center"]["x"].as<float>(),
		_node["m_Center"]["y"].as<float>(),
		_node["m_Center"]["z"].as<float>(),
		});
}

void Truth::UnityParser::ParseMeshCollider(const YAML::Node& _node, GameObject* _owner)
{
	_owner->m_isCollider = true;
	_owner->m_shape = 4;
	_owner->m_isConvex = static_cast<bool>(_node["m_Convex"].as<int>());
}

void Truth::UnityParser::ParseMeshFilter(const YAML::Node& _node, GameObject* _owner)
{
}

void Truth::UnityParser::ParseLight(const YAML::Node& _node, GameObject* _owner)
{
	_owner->m_isLight = true;
	m_lightCount++;
	_owner->m_intensity = _node["m_Intensity"].as<float>();
	_owner->m_type = _node["m_Type"].as<uint32>();
	_owner->m_color.x = _node["m_Color"]["r"].as<float>();
	_owner->m_color.y = _node["m_Color"]["g"].as<float>();
	_owner->m_color.z = _node["m_Color"]["b"].as<float>();
	_owner->m_color.w = _node["m_Color"]["a"].as<float>();
	_owner->m_range = _node["m_Range"].as<float>();
	_owner->m_angle = _node["m_SpotAngle"].as<float>();
}

void Truth::UnityParser::GetPrefabMatrix(const YAML::Node& _node, GameObject* _owner)
{
	Vector3 scale = { 1.0f ,1.0f, 1.0f };
	Vector3 pos = { 0.0f, 0.0f, 0.0f };
	Quaternion rot = { 0.0f, 0.0f, 0.0f, 1.0f };

	for (auto itr = _node.begin(); itr != _node.end(); itr++)
	{
		if ((*itr)["propertyPath"].as<std::string>() == "m_LocalScale.x")
		{
			_owner->m_localScaleChange[0] = true;
			scale.x = (*itr)["value"].as<float>();
		}
		else if ((*itr)["propertyPath"].as<std::string>() == "m_LocalScale.y")
		{
			_owner->m_localScaleChange[1] = true;
			scale.y = (*itr)["value"].as<float>();
		}
		else if ((*itr)["propertyPath"].as<std::string>() == "m_LocalScale.z")
		{
			_owner->m_localScaleChange[2] = true;
			scale.z = (*itr)["value"].as<float>();
		}

		else if ((*itr)["propertyPath"].as<std::string>() == "m_LocalPosition.x")
		{
			_owner->m_localPosChange[0] = true;
			pos.x = (*itr)["value"].as<float>();
		}
		else if ((*itr)["propertyPath"].as<std::string>() == "m_LocalPosition.y")
		{
			_owner->m_localPosChange[1] = true;
			pos.y = (*itr)["value"].as<float>();
		}
		else if ((*itr)["propertyPath"].as<std::string>() == "m_LocalPosition.z")
		{
			_owner->m_localPosChange[2] = true;
			pos.z = (*itr)["value"].as<float>();
		}

		else if ((*itr)["propertyPath"].as<std::string>() == "m_LocalRotation.w")
		{
			_owner->m_localRotationChange = true;
			rot.w = (*itr)["value"].as<float>();
		}
		else if ((*itr)["propertyPath"].as<std::string>() == "m_LocalRotation.x")
		{
			rot.x = (*itr)["value"].as<float>();
		}
		else if ((*itr)["propertyPath"].as<std::string>() == "m_LocalRotation.y")
		{
			rot.y = (*itr)["value"].as<float>();
		}
		else if ((*itr)["propertyPath"].as<std::string>() == "m_LocalRotation.z")
		{
			rot.z = (*itr)["value"].as<float>();
		}
	}

	_owner->m_scale = scale;
	_owner->m_rotation = rot;
	_owner->m_position = pos;
}

void Truth::UnityParser::GetPrefabMatarial(GameObject* _GO, const YAML::Node& _node)
{
	for (auto itr = _node.begin(); itr != _node.end(); itr++)
	{
		std::string propertyPath = (*itr)["propertyPath"].as<std::string>();
		if (propertyPath.find("m_Materials.Array.data") != std::string::npos)
		{
			if (!(*itr)["objectReference"].IsDefined())
			{
				return;
			}
			if (!(*itr)["objectReference"]["guid"].IsDefined())
			{
				return;
			}
			std::string matGuid = (*itr)["objectReference"]["guid"].as<std::string>();
			if (m_matarialMap.find(matGuid) != m_matarialMap.end())
			{
				_GO->m_matarialsName.push_back(matGuid);
				continue;
			}
			if (matGuid == "31321ba15b8f8eb4c954353edc038b1d")
			{
				continue;
			}
			ParseMatarialFile(_GO, matGuid);
		}

		else if (propertyPath == "m_Name")
		{
			_GO->m_name = (*itr)["value"].as<std::string>();
		}
	}

	return;
}

void Truth::UnityParser::ParseFbxMetaFile(GameObject* _GO, const fs::path& _fbxPath)
{
	_GO->m_isMesh = true;
	_GO->m_meshPath = _fbxPath.generic_string();
	m_meshFilterCount++;

	// get matarial

	fs::path metaPath = _fbxPath;

	m_unLoadMesh.insert(_fbxPath);

	metaPath += ".meta";

	fs::path uscene(metaPath);
	std::ifstream dataFile(metaPath);

	auto doc = YAML::LoadAll(dataFile);
	for (size_t i = 0; i < doc.size(); i++)
	{
		const YAML::Node& externalObjects = doc[i]["ModelImporter"]["externalObjects"];

		if (externalObjects.IsDefined())
		{
			for (auto& node : externalObjects)
			{
				if (node["first"]["type"].as<std::string>() == "UnityEngine:Material")
				{
					std::string matGuid = node["second"]["guid"].as<std::string>();
					if (matGuid == "31321ba15b8f8eb4c954353edc038b1d")
					{
						_GO->m_isMesh = false;
						m_meshFilterCount--;
						continue; 
					}
					if (m_matarialMap.find(matGuid) != m_matarialMap.end())
					{
						_GO->m_matarialsName.push_back(m_matarialMap[matGuid].m_name);
						continue;
					}
					ParseMatarialFile(_GO, matGuid);
				}
			}
		}
	}
}

void Truth::UnityParser::ParseMatarialFile(GameObject* _GO, const std::string& _matGuid)
{
	fs::create_directories(m_texturePath);
	
	MatarialData matdata = MatarialData();

	fs::path matfile = m_guidMap[_matGuid]->m_filePath;

	if (matfile.extension() != ".mat")
	{
		return;
	}

	std::ifstream matDataFile(matfile);

	matdata.m_name = _matGuid;
	_GO->m_matarialsName.push_back(_matGuid);

	auto matDoc = YAML::LoadAll(matDataFile);
	for (auto& matNode : matDoc)
	{
		const YAML::Node& keyWordNode = matNode["Material"]["m_ValidKeywords"];
		if (keyWordNode.IsDefined() && keyWordNode.IsSequence())
		{
			for (auto& keyWord : keyWordNode)
			{
				if (keyWord.as<std::string>() == "_ALPHATEST_ON")
					matdata.m_alphaCulling = true;
				if (keyWord.as<std::string>() == "_SURFACE_TYPE_TRANSPARENT")
					matdata.m_transparent = true;
			}
		}

		const YAML::Node& keywordNode = matNode["Material"]["m_ValidKeywords"];
		bool normalMap = false, maskMap = false, metalicMap = false;
		if (keywordNode.IsDefined() && keywordNode.IsSequence())
		{
			for (auto& keyword : keywordNode)
			{
				if (keyword.as<std::string>() == "_MASKMAP")
					maskMap = true;
				if (keyword.as<std::string>() == "_NORMALMAP")
					normalMap = true;
				if (keyword.as<std::string>() == "_METALLICSPECGLOSSMAP")
					metalicMap = true;
			}
		}

		const YAML::Node& texNode = matNode["Material"]["m_SavedProperties"]["m_TexEnvs"];
		if (texNode.IsDefined() && texNode.IsSequence())
		{
			for (auto& texmap : texNode)
			{
				if (texmap["_NormalMap"].IsDefined() && normalMap)
					CopyTexture(texmap["_NormalMap"], matdata.m_normal);

				else if (texmap["_BumpMap"].IsDefined())
					CopyTexture(texmap["_BumpMap"], matdata.m_normal);

				else if (texmap["_MainTex"].IsDefined())
				{
					CopyTexture(texmap["_MainTex"], matdata.m_albedo);

					matdata.m_tileX = texmap["_MainTex"]["m_Scale"]["x"].as<float>();
					matdata.m_tileY = texmap["_MainTex"]["m_Scale"]["y"].as<float>();
				}
				else if (texmap["_MaskMap"].IsDefined())
					CopyTexture(texmap["_MaskMap"], matdata.m_metalicRoughness);

				else if (texmap["_MetallicGlossMap"].IsDefined() && metalicMap)
					CopyTexture(texmap["_MetallicGlossMap"], matdata.m_metalicRoughness);

				else if (texmap["_BarkBaseColorMap"].IsDefined())
					CopyTexture(texmap["_BarkBaseColorMap"], matdata.m_albedo);

				else if (texmap["_BarkMaskMap"].IsDefined())
					CopyTexture(texmap["_BarkMaskMap"], matdata.m_metalicRoughness);

				else if (texmap["_BarkNormalMap"].IsDefined())
					CopyTexture(texmap["_BarkNormalMap"], matdata.m_normal);
			}
		}
	}
	m_matarialMap[_matGuid] = matdata;
}

void Truth::UnityParser::CopyTexture(const YAML::Node& _node, fs::path& _output)
{
	if (!_node["m_Texture"]["guid"].IsDefined())
		return;
	std::string texGuid = _node["m_Texture"]["guid"].as<std::string>();
	if (m_guidMap.find(texGuid) == m_guidMap.end())
		return;

	const fs::path originPath = m_guidMap[texGuid]->m_filePath;
	fs::path parent = fs::relative(originPath, m_rootPath);
	fs::path copyPath = m_texturePath / parent;

	if (!fs::exists(copyPath.parent_path()))
		fs::create_directories(copyPath.parent_path());

	if (!fs::exists(copyPath))
		fs::copy(originPath, copyPath, fs::copy_options::skip_existing);

	_output = copyPath;
}

void Truth::UnityParser::WriteMaterialData()
{
	fs::create_directories(m_matSavePath);

	for (auto& mat : m_matarialMap)
	{
		fs::path p = m_matSavePath / (mat.second.m_name + ".matData");
		const auto& matData = mat.second;

		if (matData.m_transparent)
		{
			int a = 1;
		}

		YAML::Node node;
		YAML::Emitter emitter;
		emitter << YAML::BeginDoc;
		emitter << YAML::BeginMap;

		emitter << YAML::Key << "baseMap" << YAML::Value << matData.m_albedo.generic_string();
		emitter << YAML::Key << "normalMap" << YAML::Value << matData.m_normal.generic_string();
		emitter << YAML::Key << "maskMap" << YAML::Value << matData.m_metalicRoughness.generic_string();

		emitter << YAML::Key << "tileX" << YAML::Value << matData.m_tileX;
		emitter << YAML::Key << "tileY" << YAML::Value << matData.m_tileY;
		emitter << YAML::Key << "alphaCulling" << YAML::Value << matData.m_alphaCulling;
		emitter << YAML::Key << "transparent" << YAML::Value << matData.m_transparent;

		emitter << YAML::EndMap;
		emitter << YAML::EndDoc;

		std::ofstream fout(p);
		fout << emitter.c_str();

		fout.close();
	}
}

void Truth::UnityParser::ConvertUnloadedMesh()
{
	fs::path assetPath = m_assetPath;
	fs::path dataPath = "MapData";

	fs::path modelPath = m_modelPath;
	fs::create_directories(assetPath);
	fs::create_directories(modelPath);

	for (auto& path : m_unLoadMesh)
	{
		fs::copy(path, assetPath, fs::copy_options::update_existing);
		m_gp->ConvertAsset(dataPath / path.filename(), false, true);
	}
}


void Truth::UnityParser::WriteData()
{
	ConvertUnloadedMesh();
	WriteMaterialData();

	std::shared_ptr<TFileUtils> file = std::make_shared<TFileUtils>();
	std::wstring path = m_defaultPath;
	path += m_sceneName.generic_wstring() + L"/";
	path += L"Data.map";

	fs::path tempPath = path;

	fs::create_directories(tempPath.parent_path());

	file->Open(path, FileMode::Write);
	file->Write<size_t>(m_gameObjects.size());
	for (auto g : m_gameObjects)
	{
		file->Write<int32>(g->m_parent);
		file->Write<std::string>(g->m_name);
		WriteLocalTMData(file, g);
		WriteColliderData(file, g);
		WriteMeshData(file, g);
		WriteLightData(file, g);
	}
}

void Truth::UnityParser::WriteColliderData(std::shared_ptr<TFileUtils> _file, GameObject* _GO)
{
	_file->Write<bool>(_GO->m_isCollider);
	if (_GO->m_isCollider)
	{
		_file->Write<int32>(_GO->m_shape);

		if (_GO->m_shape == 1)
		{
			_file->Write<Vector3>(_GO->m_size[0]);
			_file->Write<Vector3>(_GO->m_center[0]);
		}
		else if (_GO->m_shape == 4)
			_file->Write<bool>(_GO->m_isConvex);
	}
}

void Truth::UnityParser::WriteMeshData(std::shared_ptr<TFileUtils> _file, GameObject* _GO)
{
	_file->Write<bool>(_GO->m_isMesh);
	if (_GO->m_isMesh)
	{
		_file->Write<std::string>(_GO->m_meshPath);

		if (_GO->m_meshPath.empty())
		{
			int a = 1;
		}

		_file->Write<size_t>(_GO->m_matarialsName.size());
		for (size_t i = 0; i < _GO->m_matarialsName.size(); ++i)
		{
			_file->Write<std::string>(_GO->m_matarialsName[i]);
		}
	}
}

void Truth::UnityParser::WriteLightData(std::shared_ptr<TFileUtils> _file, GameObject* _GO)
{
	_file->Write<bool>(_GO->m_isLight);
	if (_GO->m_isLight)
	{
		_file->Write<uint32>(_GO->m_type);
		_file->Write<float>(_GO->m_intensity);
		_file->Write<float>(_GO->m_color.x);
		_file->Write<float>(_GO->m_color.y);
		_file->Write<float>(_GO->m_color.z);
		_file->Write<float>(_GO->m_color.w);
		_file->Write<float>(_GO->m_range);
		_file->Write<float>(_GO->m_angle);
	}
}

void Truth::UnityParser::WriteLocalTMData(std::shared_ptr<TFileUtils> _file, GameObject* _GO)
{
	_file->Write<float>(_GO->m_position.x);
	_file->Write<float>(_GO->m_position.y);
	_file->Write<float>(_GO->m_position.z);

	_file->Write<float>(_GO->m_rotation.x);
	_file->Write<float>(_GO->m_rotation.y);
	_file->Write<float>(_GO->m_rotation.z);
	_file->Write<float>(_GO->m_rotation.w);

	_file->Write<float>(_GO->m_scale.x);
	_file->Write<float>(_GO->m_scale.y);
	_file->Write<float>(_GO->m_scale.z);

	_file->Write<bool>(_GO->m_localPosChange[0]);
	_file->Write<bool>(_GO->m_localPosChange[1]);
	_file->Write<bool>(_GO->m_localPosChange[2]);

	_file->Write<bool>(_GO->m_localScaleChange[0]);
	_file->Write<bool>(_GO->m_localScaleChange[1]);
	_file->Write<bool>(_GO->m_localScaleChange[2]);

	_file->Write<bool>(_GO->m_localRotationChange);
}

/// <summary>
/// Prefab 파일을 파싱한다.
/// </summary>
/// <param name="_path">경로</param>
/// <param name="_parent">부모 오브젝트</param>
void Truth::UnityParser::ReadPrefabFile(const fs::path& _path, GameObject* _parent)
{
	fs::path uscene(_path);
	fs::path cscene = OrganizeUnityFile(uscene);
	std::ifstream dataFile(cscene);

	std::string& guid = m_pathMap[uscene]->m_guid;

	auto doc = YAML::LoadAll(dataFile);
	for (size_t i = 0; i < doc.size(); i++)
	{
		ParseYAMLFile(doc[i], guid);
	}

	// get transform node ( 4 = transform class at unity scene file )
	for (auto& p : m_classMap[guid]["4"])
	{
		// paresing when root game object
		if (p->m_node["Transform"]["m_Father"]["fileID"].IsDefined())
		{
			std::string parentFid = p->m_node["Transform"]["m_Father"]["fileID"].as<std::string>();
			if (parentFid == "0")
			{
				ParseTranfomrNode(p->m_node["Transform"], guid, _parent->m_mine);
			}
		}
	}

	// get prefab node ( 1001 = prefab class at unity scene file )
	for (auto& p : m_classMap[guid]["1001"])
	{
		ParsePrefabNode(p->m_node["PrefabInstance"], guid, _parent->m_mine);
	}

}

void Truth::UnityParser::ParseUnityFile(const std::string& _path)
{
	// get scene path
	fs::path uscene(_path);
	fs::path cscene = OrganizeUnityFile(uscene);

	// copy scene data
	std::ifstream dataFile(cscene);
	std::string& guid = m_pathMap[uscene]->m_guid;

	// load yaml unity file
	auto doc = YAML::LoadAll(dataFile);
	for (size_t i = 0; i < doc.size(); i++)
		ParseYAMLFile(doc[i], guid);

	m_sceneName = uscene.filename().replace_extension("");

	// get root game object
	YAML::Node p = m_classMap[guid]["1660057539"][0]->m_node["SceneRoots"]["m_Roots"];

	for (auto itr = p.begin(); itr != p.end(); itr++)
	{
		const std::string& fid = (*itr)["fileID"].as<std::string>();
		YAML::Node& node = m_nodeMap[guid][fid]->m_node;

		if (node["Transform"].IsDefined())
			ParseTranfomrNode(node["Transform"], guid, -1);
		else if (node["PrefabInstance"].IsDefined())
			ParsePrefabNode(node["PrefabInstance"], guid, -1);
	}

	WriteData();
}

/// <summary>
/// 파싱한 데이터를 초기화한다
/// </summary>
void Truth::UnityParser::Reset()
{
	for (auto& fn : m_nodeMap)
	{
		for (auto& n : fn.second)
		{
			delete n.second;
		}
	}

	for (auto& fn : m_guidMap)
	{
		delete fn.second;
	}

	for (auto& r : m_gameObjects)
	{
		delete r;
	}

	m_guidMap.clear();
	m_pathMap.clear();
	m_nodeMap.clear();
	m_classMap.clear();
}


#endif // EDITOR_MODE
