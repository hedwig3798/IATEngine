#pragma once
#include "EditorContext.h"

namespace Truth
{
	class Entity;
}

class LinkBonePopup :
    public EditorContext
{
private:
	std::weak_ptr<Truth::Entity> m_selectedEntity;

public:
	LinkBonePopup(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor, std::weak_ptr<Truth::Entity> _selectedEntity);
	virtual ~LinkBonePopup();

	void ShowContext(bool* p_open) override;
};

