#pragma once
#include "EditorContext.h"
class NewScenePopup :
    public EditorContext
{
private:
	char m_inputTextBuffer[256];

public:
	NewScenePopup(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor);
	virtual ~NewScenePopup();

	void ShowContext(bool* p_open) override;
};

