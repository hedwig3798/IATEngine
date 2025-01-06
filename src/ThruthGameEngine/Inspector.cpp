#include "Inspector.h"
#ifdef EDITOR_MODE
#include "imgui.h"
#include "Entity.h"

Inspector::Inspector(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor, std::weak_ptr<Truth::Entity>& _selectedEntity)
	: EditorContext("Inspector", _manager, _hwnd, _editor)
	, m_componentList(TypeInfo::g_componentFactory->m_componentList)
	, m_selectedEntity(_selectedEntity)
{
	m_isActive = true;
}

Inspector::~Inspector()
{

}

void Inspector::ShowContext(bool* p_open)
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

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Inspector", p_open, window_flags))
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

	/// 여기부터 UI 만들기
	const auto& entities = m_manager.lock()->Scene()->m_currentScene->m_entities;

	if (!m_selectedEntity.expired())
	{
		// Set Entity Name

		auto entity = m_selectedEntity.lock();
		std::string sEntityName = entity->m_name;
		char* cEntityName = (char*)sEntityName.c_str();

		if (ImGui::Checkbox("##1", &entity->m_isActive))
		{
			entity->SetActive(entity->m_isActive);
		}
		ImGui::SameLine();
		ImGui::InputText("##2", cEntityName, 128);
		if (m_manager.lock()->Input()->GetKeyState(KEY::ENTER) == KEY_STATE::DOWN)
		{
			sEntityName = std::string(cEntityName, cEntityName + strlen(cEntityName));
			entity->m_name = sEntityName;
		}


		// Show Components
		for (auto& e : entity->m_components)
			TranslateComponent(e);

		while (!m_deletedComponent.empty())
		{
			auto& t = m_deletedComponent.front();
			t.first.lock()->DeleteComponent(t.second);
			m_deletedComponent.pop();
		}

		// Add Component
		AddComponentList(m_selectedEntity.lock());
	}

	/// End of ShowDemoWindow()
	ImGui::PopItemWidth();
	ImGui::End();
}

void Inspector::TranslateComponent(std::shared_ptr<Truth::Component> _component)
{
	if (_component == nullptr)
		return;

	const TypeInfo& typeinfo = _component->GetTypeInfo();

	// 컴포넌트 이름
	const char* componentName = typeinfo.GetName();



	auto& properties = typeinfo.GetProperties();
	bool isSelect = false;

	// const auto& entities = m_manager->Scene()->m_currentScene->m_entities;

	if (ImGui::CollapsingHeader(componentName, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::Selectable("Remove Component"))
				m_deletedComponent.push(std::make_pair(m_selectedEntity, _component->m_index));

			ImGui::EndPopup();
		}

		for (auto* p : properties)
			isSelect |= p->DisplayUI(_component.get(), "##" + std::to_string(_component->m_ID));
	}
	if (isSelect)
		_component->EditorSetValue();
}

void Inspector::AddComponentList(std::shared_ptr<Truth::Entity> SelectedEntity)
{
	if (ImGui::CollapsingHeader("Add Component"))
	{
		int selectedItem = -1;
		if (ImGui::ListBox("Component", &selectedItem, m_componentList.data(), static_cast<int32>(m_componentList.size()), 12))
			SelectedEntity->AddComponent(TypeInfo::g_componentFactory->Create(m_componentList[selectedItem]));
	}
}
#endif // EDITOR_MODE
