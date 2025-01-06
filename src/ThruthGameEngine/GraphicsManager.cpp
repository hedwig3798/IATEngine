#include "GraphicsManager.h"
#include "IRenderScene.h"
#include "ISkinnedMeshObject.h"
#include "imgui.h"
#include "Camera.h"
#include "Texture.h"
#include "Material.h"
#include <filesystem>
#include "UISpriteSet.h"
#ifdef EDITOR_MODE
#include "EditorCamera.h"
#endif // EDITOR_MODE
#include "TFileUtils.h"
#include <yaml-cpp/yaml.h>


/// <summary>
/// 그래픽 엔진과 소통하는 매니저
/// </summary>
Truth::GraphicsManager::GraphicsManager()
	: m_renderer(nullptr)
	, m_aspect(1.0f)
	, m_mainCamera(nullptr)
	, m_resolution(1920, 1080)
	, m_hwnd()
{

}

/// <summary>
/// 소멸자
/// </summary>
Truth::GraphicsManager::~GraphicsManager()
{
	DEBUG_PRINT("Finalize GraphicsManager\n");
}

/// <summary>
/// 초기화
/// </summary>
/// <param name="_hwnd">윈도우 핸들러</param>
/// <param name="_wight">스크린 넓이</param>
/// <param name="_height">스크린 높이</param>
void Truth::GraphicsManager::Initalize(HWND _hwnd, uint32 _wight, uint32 _height)
{
	m_hwnd = _hwnd;
	// Editor mode & Release mode
#ifdef EDITOR_MODE
	m_renderer = CreateRenderer(
		EGraphicsInterfaceType::D3D12_RAYTRACING_EDITOR,
		//EGraphicsInterfaceType::D3D12_EDITOR,
		&_hwnd,
		_wight,
		_height,
		m_assetPath[0],
		m_assetPath[1],
		m_assetPath[2]
	);

#else
	m_renderer = CreateRenderer(
		EGraphicsInterfaceType::D3D12_RAYTRACING,
		&_hwnd,
		_wight,
		_height,
		m_assetPath[0],
		m_assetPath[1],
		m_assetPath[2]
	);
#endif // EDITOR_MODE
	m_renderer->Init();

	// 추후에 카메라에 넘겨 줄 시야각
	m_aspect = static_cast<float>(_wight) / static_cast<float>(_height);

	m_renderer->SetDisplayResolutionOption(Ideal::Resolution::EDisplayResolutionOption::R_1920_1080);
}

void Truth::GraphicsManager::Finalize()
{
	for (auto& mat : m_matarialMap)
	{
		m_renderer->DeleteMaterial(mat.second->m_material);
		// mat.second->m_material.reset();
	}
	m_matarialMap.clear();
	for (auto& tex : m_textureMap)
	{
		m_renderer->DeleteTexture(tex.second->m_texture);
		// tex.second->m_texture.reset();
	}
	m_textureMap.clear();

	m_renderer.reset();
}

/// <summary>
/// 렌더
/// </summary>
void Truth::GraphicsManager::Render()
{
#ifdef EDITOR_MODE
	if (m_mainCamera)
	{
		m_mainCamera->CompleteCamera();
	}
	m_renderer->Render();
#else
	CompleteCamera();
	m_renderer->Render();
#endif // EDITOR_MODE
}

/// <summary>
/// FBX 에셋을 엔진 포멧에 맞도록 컨버팅
/// </summary>
/// <param name="_path">경로</param>
/// <param name="_isSkind">skinned 여부</param>
/// <param name="_isData">위치 데이터</param>
/// <param name="_isCenter">피벗 보간</param>
void Truth::GraphicsManager::ConvertAsset(std::wstring _path, bool _isSkind /*= false*/, bool _isData /*= false*/, bool _isCenter)
{
	m_renderer->ConvertAssetToMyFormat(_path, _isSkind, _isData, _isCenter);
}

/// <summary>
/// Bone이 있는 스키닝 매쉬
/// </summary>
/// <param name="_path">파일 경로</param>
/// <returns>매쉬 오브젝트</returns>
std::shared_ptr<Ideal::ISkinnedMeshObject> Truth::GraphicsManager::CreateSkinnedMesh(std::wstring _path)
{
	return m_renderer->CreateSkinnedMeshObject(_path);
}

/// <summary>
/// 정적 오브젝트
/// </summary>
/// <param name="_path">파일 경로</param>
/// <returns>매쉬 오브젝트 </returns>
std::shared_ptr<Ideal::IMeshObject> Truth::GraphicsManager::CreateMesh(std::wstring _path)
{
	return m_renderer->CreateStaticMeshObject(_path);
}

/// <summary>
/// 디버깅 매쉬 생성
/// </summary>
/// <param name="_path">경로</param>
/// <returns>디버깅 매쉬</returns>
std::shared_ptr<Ideal::IMeshObject> Truth::GraphicsManager::CreateDebugMeshObject(std::wstring _path)
{
	return m_renderer->CreateDebugMeshObject(_path);
}

/// <summary>
/// 애니메이션 생성
/// </summary>
/// <param name="_path">경로</param>
/// <param name="_offset">애니메이션 오프셋 매트릭스</param>
/// <returns>애니메이션</returns>
std::shared_ptr<Ideal::IAnimation> Truth::GraphicsManager::CreateAnimation(std::wstring _path, const Matrix& _offset /*= Matrix::Identity*/)
{
	return m_renderer->CreateAnimation(_path, _offset);
}

/// <summary>
/// 직사광성 생성
/// </summary>
/// <returns>직사광선</returns>
std::shared_ptr<Ideal::IDirectionalLight> Truth::GraphicsManager::CreateDirectionalLight()
{
	return m_renderer->CreateDirectionalLight();
}

/// <summary>
/// 직사광선 삭제
/// </summary>
/// <param name="_dLight">직사광선</param>
void Truth::GraphicsManager::DeleteDirectionalLight(std::shared_ptr<Ideal::IDirectionalLight> _dLight)
{
	m_renderer->DeleteLight(_dLight);
}

std::shared_ptr<Ideal::ISpotLight> Truth::GraphicsManager::CreateSpotLight()
{
	return m_renderer->CreateSpotLight();
}

void Truth::GraphicsManager::DeleteSpotLight(std::shared_ptr<Ideal::ISpotLight> _sLight)
{
	m_renderer->DeleteLight(_sLight);
}

std::shared_ptr<Ideal::IPointLight> Truth::GraphicsManager::CreatePointLight()
{
	return m_renderer->CreatePointLight();
}

void Truth::GraphicsManager::DeletePointLight(std::shared_ptr<Ideal::IPointLight> _pLight)
{
	m_renderer->DeleteLight(_pLight);
}

std::shared_ptr<Ideal::ISprite> Truth::GraphicsManager::CreateSprite()
{
	return m_renderer->CreateSprite();
}

void Truth::GraphicsManager::DeleteSprite(std::shared_ptr<Ideal::ISprite> _sprite)
{
	m_renderer->DeleteSprite(_sprite);
}

/// <summary>
/// 카메라 생성
/// </summary>
/// <returns>카메라 오브젝트</returns>
std::shared_ptr<Ideal::ICamera> Truth::GraphicsManager::CreateCamera()
{
	return m_renderer->CreateCamera();
}

/// <summary>
/// 등록된 Mesh
/// </summary>
/// <param name="_meshObject"></param>
void Truth::GraphicsManager::DeleteMeshObject(std::shared_ptr<Ideal::IMeshObject> _meshObject)
{
	m_renderer->DeleteMeshObject(_meshObject);
}

void Truth::GraphicsManager::DeleteDebugMeshObject(std::shared_ptr<Ideal::IMeshObject> _meshObject)
{
	m_renderer->DeleteDebugMeshObject(_meshObject);
}

/// <summary>
/// 메인 카메라 지정
/// </summary>
/// <param name="_camera">카메라 오브젝트</param>
void Truth::GraphicsManager::SetMainCamera(Camera* _camera)
{
	m_renderer->SetMainCamera(_camera->m_camera);
	m_mainCamera = _camera;
}

std::shared_ptr<Truth::Texture> Truth::GraphicsManager::CreateTexture(const std::wstring& _path, bool _createMips, bool _isNormalMap, bool _ignoreSRGB)
{
	std::filesystem::path p(_path);
	if (p.is_absolute())
	{
		::SetCurrentDirectory(Managers::GetRootPath().c_str());
		p = fs::relative(_path);
	}
	if (m_textureMap.find(p) == m_textureMap.end())
	{
		std::shared_ptr<Texture> tex = std::make_shared<Texture>();
		if (_path.empty())
		{
			return nullptr;
		}
		tex->m_texture = m_renderer->CreateTexture(p, _createMips, _isNormalMap, true);
		tex->m_useCount = 1;
		tex->m_path = p;

		tex->w = tex->m_texture->GetWidth();
		tex->h = tex->m_texture->GetHeight();
		return m_textureMap[p] = tex;
	}
	return m_textureMap[p];
}

void Truth::GraphicsManager::DeleteTexture(std::shared_ptr<Texture> _texture)
{
	m_renderer->DeleteTexture(_texture->m_texture);
}

std::shared_ptr<Truth::Material> Truth::GraphicsManager::CreateMaterial(const std::string& _name, bool _useDefalutPath)
{
	std::filesystem::path matp;
	if (_useDefalutPath)
	{
		matp = m_matSavePath + _name + ".matData";
	}
	else
	{
		matp = _name;
		if (matp.is_absolute())
		{
			matp = fs::relative(matp);
		}
	}

	fs::create_directories(matp.parent_path());

	std::shared_ptr<Material> mat = std::make_shared<Material>();
	if (m_matarialMap.find(_name) == m_matarialMap.end())
	{
		mat->m_material = m_renderer->CreateMaterial();
		mat->m_gp = this;
		mat->m_hwnd = m_hwnd;
		mat->m_name = _name;
		mat->m_baseMap = nullptr;
		mat->m_normalMap = nullptr;
		mat->m_maskMap = nullptr;
		mat->m_path = matp.generic_string();

		if (std::filesystem::exists(matp))
		{
			std::ifstream fin(matp);

			YAML::Node node = YAML::Load(fin);

			std::filesystem::path albedo;
			std::filesystem::path normal;
			std::filesystem::path metalicRoughness;

			if (node["baseMap"].IsDefined())
				albedo = node["baseMap"].as<std::string>();
			if (node["normalMap"].IsDefined())
				normal = node["normalMap"].as<std::string>();
			if (node["maskMap"].IsDefined())
				metalicRoughness = node["maskMap"].as<std::string>();

			if (node["tileX"].IsDefined())
				mat->m_tileX = node["tileX"].as<float>();
			if (node["tileY"].IsDefined())
				mat->m_tileY = node["tileY"].as<float>();

			if (node["alphaCulling"].IsDefined())
				mat->m_alphaCulling = node["alphaCulling"].as<bool>();
			if (node["alphaCulling"].as<bool>())
			{
				int a = 1;
			}

			if (node["transparent"].IsDefined())
				mat->m_transparent = node["transparent"].as<bool>();
			if (node["layer"].IsDefined())
				mat->m_layer = node["layer"].as<uint32>();


			mat->m_baseMap = CreateTexture(albedo, true, false);
			mat->m_normalMap = CreateTexture(normal, true, true);
			mat->m_maskMap = CreateTexture(metalicRoughness, true, true);

			mat->SetTexture();

			fin.close();
		}
		else
		{
			YAML::Node node;
			YAML::Emitter emitter;
			emitter << YAML::BeginDoc;
			emitter << YAML::BeginMap;

			fs::path al = "../Resources/DefaultData/DefaultAlbedo.png";
			fs::path no = "../Resources/DefaultData/DefaultNormalMap.png";
			fs::path ma = "../Resources/DefaultData/DefaultBlack.png";

			emitter << YAML::Key << "baseMap" << YAML::Value << al.generic_string();
			emitter << YAML::Key << "normalMap" << YAML::Value << no.generic_string();
			emitter << YAML::Key << "maskMap" << YAML::Value << ma.generic_string();

			emitter << YAML::Key << "tileX" << YAML::Value << (1.0f);
			emitter << YAML::Key << "tileY" << YAML::Value << (1.0f);

			emitter << YAML::Key << "alphaCulling" << YAML::Value << false;
			emitter << YAML::Key << "transparent" << YAML::Value << false;

			mat->m_baseMap = CreateTexture(al.generic_wstring(), true, false);
			mat->m_normalMap = CreateTexture(no.generic_wstring(), true, true);
			mat->m_maskMap = CreateTexture(ma.generic_wstring(), true, true);

			mat->SetTexture();

			emitter << YAML::EndMap;
			emitter << YAML::EndDoc;

			std::ofstream fout(matp);
			fout << emitter.c_str();

			fout.close();
		}

		return m_matarialMap[_name] = mat;
	}
	return m_matarialMap[_name];
}

void Truth::GraphicsManager::DeleteMaterial(std::shared_ptr<Material> _material)
{
	m_renderer->DeleteMaterial(_material->m_material);
}

std::shared_ptr<Ideal::IParticleMaterial> Truth::GraphicsManager::CreateParticleMaterial()
{
	return m_renderer->CreateParticleMaterial();
}

void Truth::GraphicsManager::DeleteParticleMaterial(std::shared_ptr<Ideal::IParticleMaterial> _material)
{
	m_renderer->DeleteParticleMaterial(_material);
}

std::shared_ptr<Ideal::IMesh> Truth::GraphicsManager::CreateParticleMesh(const std::wstring& _name)
{
	if (m_particleMeshMap.find(_name) != m_particleMeshMap.end())
	{
		return m_particleMeshMap[_name];
	}
	m_particleMeshMap[_name] = m_renderer->CreateParticleMesh(_name);
	return m_particleMeshMap[_name];
}

void Truth::GraphicsManager::DeleteParticleMesh(std::shared_ptr<Ideal::IMesh> _mesh)
{
	return;
}

std::shared_ptr<Ideal::IParticleSystem> Truth::GraphicsManager::CreateParticle(std::shared_ptr<Ideal::IParticleMaterial> _mat)
{
	return m_renderer->CreateParticleSystem(_mat);
}

void Truth::GraphicsManager::DeleteParticle(std::shared_ptr<Ideal::IParticleSystem> _particle)
{
	m_renderer->DeleteParticleSystem(_particle);
}

std::shared_ptr<Ideal::IShader> Truth::GraphicsManager::CreateShader(const std::wstring& _name)
{
	return m_renderer->CreateAndLoadParticleShader(_name);
}

std::shared_ptr<Truth::Material> Truth::GraphicsManager::GetMaterial(const std::string& _name)
{
	return m_matarialMap[_name];
}

std::shared_ptr<Truth::UISpriteSet> Truth::GraphicsManager::CreateUISpriteSet()
{
	std::shared_ptr<Truth::UISpriteSet> result = std::make_shared<Truth::UISpriteSet>();
	result->m_gp = this;
	result->m_hwnd = m_hwnd;

	return result;
}

void Truth::GraphicsManager::DeleteUISpriteSet(std::shared_ptr<UISpriteSet> _UISpriteSet)
{
	m_renderer->DeleteSprite((*_UISpriteSet)[0]);
	m_renderer->DeleteSprite((*_UISpriteSet)[1]);
	m_renderer->DeleteSprite((*_UISpriteSet)[2]);
}

std::shared_ptr<Ideal::IText> Truth::GraphicsManager::CreateTextSprite(uint32 _w, uint32 _h, float _size, std::wstring _font)
{
	return nullptr;
	return m_renderer->CreateText(_w, _h, _size, _font);
}

void Truth::GraphicsManager::DeleteTextSprite(std::shared_ptr<Ideal::IText> _textSprite)
{
	m_renderer->DeleteText(_textSprite);
}

void Truth::GraphicsManager::ToggleFullScreen()
{
	m_renderer->ToggleFullScreenWindow();
}

void Truth::GraphicsManager::ResizeWindow(uint32 _w, uint32 _h)
{
	m_renderer->Resize(_w, _h);
}

DirectX::SimpleMath::Vector2 Truth::GraphicsManager::GetContentPosMin()
{
	return m_renderer->GetTopLeftEditorPos();
}

DirectX::SimpleMath::Vector2 Truth::GraphicsManager::GetContentPosMax()
{
	return m_renderer->GetRightBottomEditorPos();
}


DirectX::SimpleMath::Vector2 Truth::GraphicsManager::GetDisplayResolution()
{
	return m_resolution;
}

RECT Truth::GraphicsManager::GetWindowRect()
{
	RECT result;
	::GetClientRect(m_hwnd, &result);
	return result;
}

void Truth::GraphicsManager::BakeStaticMesh()
{
	// 	m_renderer->BakeOption(32, 10.f);
	// 	m_renderer->BakeStaticMeshObject();
	// 	m_renderer->ReBuildBLASFlagOn();
}

void Truth::GraphicsManager::AddCineCamera(const std::string& _name, std::shared_ptr<CineCamera> _cineCamera)
{
	auto itr = m_cineCameraMap.find(_name);
	if (itr == m_cineCameraMap.end())
		m_cineCameraMap[_name] = _cineCamera;
}

std::shared_ptr<Truth::CineCamera> Truth::GraphicsManager::GetCineCamera(const std::string& _name)
{
	auto itr = m_cineCameraMap.find(_name);
	if (itr == m_cineCameraMap.end())
		return nullptr;
	return (*itr).second.lock();
}

void Truth::GraphicsManager::DeleteCineCamera(const std::string& _name)
{
	auto itr = m_cineCameraMap.find(_name);
	if (itr == m_cineCameraMap.end())
		return;
	m_cineCameraMap.erase(itr);
}

void Truth::GraphicsManager::ChangeSkyBox(fs::path _path)
{
	if (!fs::exists(_path))
		return;

	auto it = m_textureMap.find(_path);
	if (it == m_textureMap.end())
	{
		std::shared_ptr<Texture> t = std::make_shared<Texture>();
		t->m_texture = m_renderer->CreateSkyBox(_path);
		m_textureMap[_path] = t;
	}
	m_renderer->SetSkyBox(m_textureMap[_path]->m_texture);
}

void Truth::GraphicsManager::SetBrightness(float _brightness)
{
	m_renderer->SetRendererAmbientIntensity(_brightness);
}

#ifdef EDITOR_MODE
void Truth::GraphicsManager::SetMainCamera(EditorCamera* _camera)
{
	m_renderer->SetMainCamera(_camera->m_camera);
}
#endif // EDITOR_MODE

void Truth::GraphicsManager::CompleteCamera()
{
	if (m_mainCamera)
	{
		m_mainCamera->CompleteCamera();
	}
}

void Truth::GraphicsManager::ResetMainCamera()
{
	m_mainCamera = nullptr;
}

