#include "EntityHierarchy.h"
#ifdef EDITOR_MODE

#include "imgui.h"
#include "Scene.h"
#include "Managers.h"
#include "SceneManager.h"
#include "SkinnedMesh.h"
#include "Entity.h"
#include "EditorContext.h"
#include "LinkBonePopup.h"
#include "EditorUI.h"
#include "DirectionLight.h"
#include "SpotLight.h"
#include "PointLight.h"

EntityHierarchy::EntityHierarchy(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor)
	: EditorContext("Entity Hierarchy", _manager, _hwnd, _editor)
{
	m_isActive = true;
}

EntityHierarchy::~EntityHierarchy()
{

}

void EntityHierarchy::ShowContext(bool* p_open)
{
	// Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
// Most functions would normally just assert/crash if the context is missing.
	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");

	// Verify ABI compatibility between caller code and compiled version of Dear ImGui. This helps detects some build issues.
	IMGUI_CHECKVERSION();

	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = true;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_close = false;
	static bool no_nav = false;
	static bool no_background = false;
	static bool no_bring_to_front = false;
	static bool no_docking = false;
	static bool unsaved_document = false;

	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (no_menu)            window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (no_docking)         window_flags |= ImGuiWindowFlags_NoDocking;
	if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
	if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

	// We specify a default position/size in case there's no data in the .ini file.
	// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	std::shared_ptr<Truth::Scene> currentScene = m_manager.lock()->Scene()->m_currentScene;
	const auto& currentSceneName = currentScene->m_name;
	const auto& currentSceneEntities = currentScene->m_entities;
	const auto& currentSceneRootEntities = currentScene->m_rootEntities;
	const auto& currentSceneMapEntities = currentScene->m_mapEntity;

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Hierarchy", p_open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.


		ImGui::End();
		return;
	}

	// Most "big" widgets share a common width settings by default. See 'Demo->Layout->Widgets Width' for details.
	// e.g. Use 2/3 of the space for widgets and 1/3 for labels (right align)
	//ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.35f);
	// e.g. Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
	ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

	ImGui::Checkbox("ShowLight", &m_showMapLight);
	
	

	/// 여기부터 UI 만들기

	/// Hierarchy UI
	{
		uint32 selectCount = 0;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::Selectable("Create Empty"))
			{
				currentScene->AddEntity(std::make_shared<Truth::Entity>(m_manager.lock()));
			}
			ImGui::EndPopup();
		}

		// Current Scene Name
		if (ImGui::CollapsingHeader(currentSceneName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto& e : currentSceneRootEntities)
			{
				DisplayEntity(e);
			}
			while (!m_createdEntity.empty())
			{
				currentScene->AddEntity(m_createdEntity.front().lock());
				m_createdEntity.pop();
			}

			if (m_showMapLight)
			{
				for (auto& e : currentSceneMapEntities)
				{
					if (!e->GetComponent<Truth::DirectionLight>().expired() || 
						!e->GetComponent<Truth::SpotLight>().expired() ||
						!e->GetComponent<Truth::PointLight>().expired())
					{
						DisplayEntity(e);
					}
				}
			}
		}
	}

	/// End of ShowDemoWindow()
	ImGui::PopItemWidth();
	ImGui::End();
}


void EntityHierarchy::DisplayEntity(std::weak_ptr<Truth::Entity> _entity)
{
	if (_entity.expired())
		return;

	const std::string entityName = _entity.lock()->m_name + "##" + std::to_string(_entity.lock()->m_ID);

	std::shared_ptr<Truth::Scene> currentScene = m_manager.lock()->Scene()->m_currentScene;

	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_AllowItemOverlap;

	if (!_entity.lock()->HasChildren())
	{
		flag |= ImGuiTreeNodeFlags_Leaf;
	}

	bool isOpen = ImGui::TreeNodeEx(("##" + entityName).c_str(), flag);
	ImGui::SameLine();

	if (ImGui::Selectable(entityName.c_str()))
	{
		m_selectedEntity = _entity;
	}

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("Entity", &_entity, sizeof(_entity));
		ImGui::Text("%s", entityName.c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
		{
			IM_ASSERT(payload->DataSize == sizeof(_entity));
			std::weak_ptr<Truth::Entity> payload_n = *(const std::weak_ptr<Truth::Entity>*)payload->Data;

			if (!payload_n.lock()->m_parent.expired())
				payload_n.lock()->m_parent.lock()->DeleteChild(payload_n.lock());
			_entity.lock()->AddChild(payload_n.lock());

			m_manager.lock()->Scene()->m_currentScene->EditorUpdate();
		}
		ImGui::EndDragDropTarget();
	}

	// Entity's Popup Menu
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Selectable("Delete"))
		{
			m_manager.lock()->Scene()->m_currentScene->DeleteEntity(_entity.lock());
			m_manager.lock()->Scene()->m_currentScene->EditorUpdate();
		}
		if (ImGui::Selectable("SaveEntity"))
		{
			m_openFileName.lpstrDefExt = L"entity";
			m_openFileName.lpstrFilter = m_entityFileFilter;
			if (GetSaveFileName(&m_openFileName) != 0)
			{
				std::wstring filepath = m_openFileName.lpstrFile;
				std::vector<std::wstring> f = StringConverter::split(filepath, L'\\');

				std::ofstream outputstream(filepath);
				boost::archive::text_oarchive outputArchive(outputstream);
				outputArchive << _entity.lock();
			}
			::SetCurrentDirectory(Truth::Managers::GetRootPath().c_str());
		}
		if (ImGui::Selectable("Create Child"))
		{
			auto child = std::make_shared<Truth::Entity>(m_manager.lock());
			_entity.lock()->AddChild(child);
			m_createdEntity.push(child);
		}
		if (ImGui::Selectable("Link Bone"))
		{
			m_editor->AddContext(
				std::make_shared<LinkBonePopup>(m_manager, m_hwnd, m_editor, _entity)
			);
		}

		if (ImGui::Selectable("Set Root"))
		{
			_entity.lock()->m_parent.lock()->DeleteChild(_entity.lock());
			_entity.lock()->m_parent.reset();
		}


		ImGui::EndPopup();
	}

	if (isOpen)
	{
		for (auto& child : _entity.lock()->m_children)
		{
			DisplayEntity(child);
		}
		ImGui::TreePop();
	}
}
#endif // EDITOR_MODE
