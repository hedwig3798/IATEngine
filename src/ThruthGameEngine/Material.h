#pragma once
#include <memory>
#include <string>
#include "TypeInfo.h"
#include <windows.h>
#include <commdlg.h>
#include "Types.h"

namespace Truth
{
	struct Texture;
	class GraphicsManager;
}
namespace Ideal
{
	class IMaterial;
}

namespace Truth
{
	struct Material
	{
		OPENFILENAME m_openFileName;
		TCHAR m_filePathBuffer[256] = {};
		TCHAR m_fileBuffer[256] = {};

		std::string m_name; 

		std::shared_ptr<Ideal::IMaterial> m_material;

		GraphicsManager* m_gp;

		HWND m_hwnd;

		std::string m_path;

		std::shared_ptr<Texture> m_baseMap;
		std::shared_ptr<Texture> m_normalMap;
		std::shared_ptr<Texture> m_maskMap;

		uint32 m_layer = 1;

		float m_tileX = 1.0f;
		float m_tileY = 1.0f;

		bool m_alphaCulling = false;
		bool m_transparent = false;

		void SetTexture();
		void ChangeTexture(std::wstring _path, int _type);
		void ChangeMaterial();
		void ChangeTexture(int _type);
		void SaveMaterial();
	};
}


