#pragma once
#include <memory>
#include <string>
#include "TypeInfo.h"
#include <windows.h>
#include <commdlg.h>

namespace Truth
{
	struct Texture;
	class GraphicsManager;
}
namespace Ideal
{
	class ISprite;
}

namespace Truth
{
	struct UISpriteSet
	{
		OPENFILENAME m_openFileName;
		TCHAR m_filePathBuffer[256];
		TCHAR m_fileBuffer[256];

		std::string m_name;

		std::shared_ptr<Ideal::ISprite> m_idealSprite;
		std::shared_ptr<Ideal::ISprite> m_overSprite;
		std::shared_ptr<Ideal::ISprite> m_clickSprite;

		GraphicsManager* m_gp;

		HWND m_hwnd;

		std::string m_path;

		std::shared_ptr<Texture> m_idealTexture;
		std::shared_ptr<Texture> m_overTexture;
		std::shared_ptr<Texture> m_clickTexture;

		void SetTexture();
		void ChangeTexture(std::wstring _path, int _type);
		void ChangeTexture(int _type);

		std::shared_ptr<Ideal::ISprite>& operator[](int _index)
		{
			switch (_index)
			{
			case 0:
				return m_idealSprite;
			case 1:
				return m_overSprite;
			case 2:
				return m_clickSprite;
			}
			return m_idealSprite;
		}

		std::shared_ptr<Texture>& GetTex(int _index)
		{
			switch (_index)
			{
			case 0:
				return m_idealTexture;
			case 1:
				return m_overTexture;
			case 2:
				return m_clickTexture;
			}
			return m_idealTexture;
		}
	};
}


