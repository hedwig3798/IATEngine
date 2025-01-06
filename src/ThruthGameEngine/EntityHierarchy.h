#pragma once
#include "EditorDefine.h"

#ifdef EDITOR_MODE


#include "EditorContext.h"

namespace Truth
{
	class Entity;
}

class EntityHierarchy :
    public EditorContext
{
private:
	std::queue<std::weak_ptr<Truth::Entity>> m_createdEntity;
	bool m_showMapLight = false;

public:
	std::weak_ptr<Truth::Entity> m_selectedEntity;

public:
	EntityHierarchy(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor);
	virtual ~EntityHierarchy();

	void ShowContext(bool* p_open) override;

private:
	void DisplayEntity(std::weak_ptr<Truth::Entity> _entity);
};

#endif // EDITOR_MODE
