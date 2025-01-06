#pragma once
#include "Headers.h"

namespace Truth
{
	class Mesh;
	class Camera;
	struct Material;
	struct Texture;
#ifdef EDITOR_MODE
	class EditorCamera;
#endif // EDITOR_MODE
	struct UISpriteSet;
	class CineCamera;
}

namespace Ideal
{
	class IMaterial;
	class ITexture;
	class ISprite;
	class IdealRenderer;
}

namespace Truth
{
	class GraphicsManager
	{
	private:
		std::shared_ptr<Ideal::IdealRenderer> m_renderer;
		Camera* m_mainCamera;
		float m_aspect;

		Vector2 m_resolution;

		std::unordered_map<std::wstring, std::shared_ptr<Ideal::IMesh>> m_particleMeshMap;

		const std::string m_matSavePath = "../Resources/Matarial/";

		const wchar_t* m_assetPath[3] =
		{
			L"../Resources/Assets/",
			L"../Resources/Models/",
			L"../Resources/Textures/"
		};
		HWND m_hwnd;

		std::map<std::string, std::weak_ptr<CineCamera>> m_cineCameraMap;

	public:
		// path / texture
		std::unordered_map<fs::path, std::shared_ptr<Texture>> m_textureMap;

		// name / matarial
		std::unordered_map<fs::path, std::shared_ptr<Material>> m_matarialMap;

	public:
		GraphicsManager();
		~GraphicsManager();

		void Initalize(HWND _hwnd, uint32 _wight, uint32 _height);
		void Finalize();
		void Render();

		void ConvertAsset(std::wstring _path, bool _isSkind = false, bool _isData = false, bool _isCenter = false);

		std::shared_ptr<Ideal::ISkinnedMeshObject> CreateSkinnedMesh(std::wstring _path);
		std::shared_ptr<Ideal::IMeshObject> CreateMesh(std::wstring _path);
		std::shared_ptr<Ideal::IMeshObject> CreateDebugMeshObject(std::wstring _path);
		std::shared_ptr<Ideal::IAnimation> CreateAnimation(std::wstring _path, const Matrix& _offset = Matrix::Identity);

		std::shared_ptr<Ideal::IDirectionalLight> CreateDirectionalLight();
		void DeleteDirectionalLight(std::shared_ptr<Ideal::IDirectionalLight> _dLight);

		std::shared_ptr<Ideal::ISpotLight> CreateSpotLight();
		void DeleteSpotLight(std::shared_ptr<Ideal::ISpotLight> _sLight);

		std::shared_ptr<Ideal::IPointLight> CreatePointLight();
		void DeletePointLight(std::shared_ptr<Ideal::IPointLight> _pLight);

		std::shared_ptr<Ideal::ISprite> CreateSprite();
		void DeleteSprite(std::shared_ptr<Ideal::ISprite> _sprite);

		std::shared_ptr<Ideal::ICamera> CreateCamera();

		void DeleteMeshObject(std::shared_ptr<Ideal::IMeshObject> _meshObject);
		void DeleteDebugMeshObject(std::shared_ptr<Ideal::IMeshObject> _meshObject);

		void SetMainCamera(Camera* _camera);

		std::shared_ptr<Texture> CreateTexture(const std::wstring& _path, bool _createMips = false, bool _isNormalMap = false, bool _ignoreSRGB = false);
		void DeleteTexture(std::shared_ptr<Texture> _texture);

		std::shared_ptr<Material> CreateMaterial(const std::string& _name, bool _useDefalutPath = true);
		void DeleteMaterial(std::shared_ptr<Material> _material);

		std::shared_ptr<Ideal::IParticleMaterial> CreateParticleMaterial();
		void DeleteParticleMaterial(std::shared_ptr<Ideal::IParticleMaterial> _material);
		std::shared_ptr<Ideal::IMesh> CreateParticleMesh(const std::wstring& _name);
		void DeleteParticleMesh(std::shared_ptr<Ideal::IMesh> _mesh);
		std::shared_ptr<Ideal::IParticleSystem> CreateParticle(std::shared_ptr<Ideal::IParticleMaterial> _mat);
		void DeleteParticle(std::shared_ptr<Ideal::IParticleSystem> _particle);

		std::shared_ptr<Ideal::IShader> CreateShader(const std::wstring& _name);

		std::shared_ptr<Material> GetMaterial(const std::string& _name);

		std::shared_ptr<UISpriteSet> CreateUISpriteSet();
		void DeleteUISpriteSet(std::shared_ptr<UISpriteSet> _UISpriteSet);

		std::shared_ptr<Ideal::IText> CreateTextSprite(uint32 _w, uint32 _h, float _size, std::wstring _font = L"Tahoma");
		void DeleteTextSprite(std::shared_ptr<Ideal::IText> _textSprite);
		void ToggleFullScreen();

		void ResizeWindow(uint32 _w, uint32 _h);

		Vector2 GetContentPosMin();
		Vector2 GetContentPosMax();

		// void SetDisplayResolution();
		Vector2 GetDisplayResolution();
		RECT GetWindowRect();
		[[maybe_unused]] void BakeStaticMesh();

		void AddCineCamera(const std::string& _name, std::shared_ptr<CineCamera> _cineCamera);
		std::shared_ptr<CineCamera> GetCineCamera(const std::string& _name);
		void DeleteCineCamera(const std::string& _name);

		void ChangeSkyBox(fs::path _path);

		void SetBrightness(float _brightness);

#ifdef EDITOR_MODE
		void SetMainCamera(EditorCamera* _camera);
#endif // EDITOR_MODE


		float GetAspect() const { return m_aspect; }

		std::shared_ptr<Ideal::IdealRenderer> GetRenderer() const {return m_renderer;}

		void CompleteCamera();

		void ResetMainCamera();
	};
}

