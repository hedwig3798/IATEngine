#include "UISpriteSet.h"
#include "ISprite.h"
#include "Texture.h"
#include "managers.h"
#include "GraphicsManager.h"

void Truth::UISpriteSet::SetTexture()
{
	if (m_idealSprite != nullptr)
		m_idealSprite->SetTexture(m_idealTexture->m_texture);
	if (m_overSprite != nullptr)
		m_overSprite->SetTexture(m_overTexture->m_texture);
	if (m_clickSprite != nullptr)
		m_clickSprite->SetTexture(m_clickTexture->m_texture);
}

void Truth::UISpriteSet::ChangeTexture(std::wstring _path, int _type)
{
	fs::path p = fs::path(_path).extension();
	if (!(
		p == ".png" ||
		p == ".PNG" ||
		p == ".tga" ||
		p == ".TGA")
		)
	{
		return;
	}
	auto m_tex = m_gp->CreateTexture(_path);
	switch (_type)
	{
	case 0:
		m_idealSprite->SetTexture(m_tex->m_texture);
		m_idealTexture = m_tex;
		break;
	case 1:
		m_overSprite->SetTexture(m_tex->m_texture);
		m_overTexture = m_tex;
		break;
	case 2:
		m_clickSprite->SetTexture(m_tex->m_texture);
		m_clickTexture = m_tex;
		break;
	default:
		break;
	}
}

void Truth::UISpriteSet::ChangeTexture(int _type)
{
	memset(&m_openFileName, 0, sizeof(OPENFILENAME));
	m_openFileName.lStructSize = sizeof(OPENFILENAME);
	m_openFileName.hwndOwner = m_hwnd;
	m_openFileName.lpstrFile = m_fileBuffer;
	m_openFileName.nMaxFile = 256;
	m_openFileName.lpstrInitialDir = L".";

	if (GetOpenFileName(&m_openFileName) != 0)
	{
		ChangeTexture(m_openFileName.lpstrFile, _type);
	}
	::SetCurrentDirectory(Managers::GetRootPath().c_str());
}

