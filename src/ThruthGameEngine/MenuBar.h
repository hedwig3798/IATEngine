#pragma once
#include "EditorDefine.h"

#ifdef EDITOR_MODE
#include "EditorContext.h"
class MenuBar :
    public EditorContext
{
public:
    MenuBar(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor);
    virtual ~MenuBar();

    void ShowContext(bool* p_open) override;

};
#endif // EDITOR_MODE

