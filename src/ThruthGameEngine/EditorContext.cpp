#include "EditorContext.h"

EditorContext::EditorContext(const std::string& _name, std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor)
	: m_name(_name)
	, m_manager(_manager)
	, m_hwnd(_hwnd)
	, m_editor(_editor)
{
	memset(&m_openFileName, 0, sizeof(OPENFILENAME));
	m_openFileName.lStructSize = sizeof(OPENFILENAME);
	m_openFileName.hwndOwner = m_hwnd;
	m_openFileName.lpstrFile = m_fileBuffer;
	m_openFileName.nMaxFile = 256;
	m_openFileName.lpstrInitialDir = L".";
}

EditorContext::~EditorContext()
{
}
