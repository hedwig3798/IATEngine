#pragma once
#include "EditorContext.h"
class LoadMapDataPopup :
    public EditorContext
{
private:
	char m_inputTextBuffer[256];

public:
	LoadMapDataPopup(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor);
	virtual ~LoadMapDataPopup();

	void ShowContext(bool* p_open) override;
};

