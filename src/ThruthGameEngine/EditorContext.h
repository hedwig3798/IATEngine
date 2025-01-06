#pragma once
#include "EditorDefine.h"

#include <memory>
#include <vector>
#include <string>
#include <windows.h>
#include "Types.h"
#include <queue>

namespace Truth
{
	class Managers;
}

class EditorUI;

/// <summary>
/// Editor �󿡼� ���� UI ������Ʈ
/// �� Context�� �� Window�� ǥ���Ѵ�.
/// </summary>
class EditorContext
{
protected:
	std::string m_name;
	std::weak_ptr<Truth::Managers> m_manager;

	OPENFILENAME m_openFileName;
	TCHAR m_filePathBuffer[256] = L"";
	TCHAR m_fileBuffer[256] = L"";
	const TCHAR* m_sceneFileFilter = L"Scene ����\0*.scene";
	const TCHAR* m_entityFileFilter = L"Entity ����\0*.entity";

	std::shared_ptr<EditorContext> m_subContext;

	HWND m_hwnd;

	bool m_isActive;

	EditorUI* m_editor;

public:
	EditorContext(const std::string& _name, std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor);
	virtual ~EditorContext();

	virtual void ShowContext(bool* p_open) abstract;
	bool IsActive() const { return m_isActive; }
};

