#pragma once
#include "EditorDefine.h"

#ifdef EDITOR_MODE

#include "EditorContext.h"
namespace Truth
{
	class Entity;
	class Component;
}

class Inspector :
    public EditorContext
{
private:
	std::vector<const char*>& m_componentList;
	std::weak_ptr<Truth::Entity>& m_selectedEntity;
	std::queue<std::pair<std::weak_ptr<Truth::Entity>, int>> m_deletedComponent;

public:
	Inspector(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor, std::weak_ptr<Truth::Entity>& _selectedEntity);
	virtual ~Inspector();

public:
	void ShowContext(bool* p_open) override;

private:
	void TranslateComponent(std::shared_ptr<Truth::Component> EntityComponent);
	void AddComponentList(std::shared_ptr<Truth::Entity> SelectedEntity);
};
#endif // EDITOR_MODE

