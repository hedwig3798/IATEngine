#pragma once
#include "EditorDefine.h"
#ifdef EDITOR_MODE
#include <memory>
#include <queue>
#include <windows.h>

class EditorContext;

namespace Truth
{
	class Entity;
	class Transform;
	class Managers;
	class Component;
}

class EditorUI
{
private:
	std::queue<std::shared_ptr<EditorContext>> m_contextQueue;
	
public:
	EditorUI(std::shared_ptr<Truth::Managers> Manager, HWND _hwnd);
	~EditorUI() {}
	void RenderUI(bool* p_open);

	void AddContext(std::shared_ptr<EditorContext> _context);

private:
	// void ShowAnimator(bool* p_open);
};
#endif // EDITOR_MODE
