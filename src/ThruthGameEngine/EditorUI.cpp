#include "EditorUI.h"
#ifdef EDITOR_MODE

#include "EntityHierarchy.h"
#include "Inspector.h"
#include "MenuBar.h"

EditorUI::EditorUI(std::shared_ptr<Truth::Managers> _manager, HWND _hwnd)
{
	std::shared_ptr<EntityHierarchy> eh = std::make_shared<EntityHierarchy>(_manager, _hwnd, this);
	std::shared_ptr<Inspector> ins = std::make_shared<Inspector>(_manager, _hwnd, this, eh->m_selectedEntity);
	std::shared_ptr<MenuBar> menu = std::make_shared<MenuBar>(_manager, _hwnd, this);

	m_contextQueue.push(eh);
	m_contextQueue.push(ins);
	m_contextQueue.push(menu);
}

void EditorUI::RenderUI(bool* p_open)
{
	size_t contextCount = m_contextQueue.size();
	bool isOpen = true;
	for (size_t i = 0; i < contextCount; i++)
	{
		auto& context = m_contextQueue.front();

		if (context->IsActive())
		{
			context->ShowContext(&isOpen);
			m_contextQueue.push(context);
		}
		m_contextQueue.pop();
	}
}

void EditorUI::AddContext(std::shared_ptr<EditorContext> _context)
{
	m_contextQueue.push(_context);
}

// void EditorUI::ShowAnimator(bool* p_open)
// {
// 	// Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
// 	// Most functions would normally just assert/crash if the context is missing.
// 	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");
// 
// 	// Verify ABI compatibility between caller code and compiled version of Dear ImGui. This helps detects some build issues.
// 	IMGUI_CHECKVERSION();
// 
// 	static bool no_titlebar = false;
// 	static bool no_scrollbar = false;
// 	static bool no_menu = true;
// 	static bool no_move = false;
// 	static bool no_resize = false;
// 	static bool no_collapse = false;
// 	static bool no_close = false;
// 	static bool no_nav = false;
// 	static bool no_background = false;
// 	static bool no_bring_to_front = false;
// 	static bool no_docking = false;
// 	static bool unsaved_document = false;
// 
// 	ImGuiWindowFlags window_flags = 0;
// 	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
// 	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
// 	if (no_menu)            window_flags |= ImGuiWindowFlags_MenuBar;
// 	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
// 	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
// 	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
// 	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
// 	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
// 	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
// 	if (no_docking)         window_flags |= ImGuiWindowFlags_NoDocking;
// 	if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
// 	if (no_close)           p_open = NULL; // Don't pass our bool* to Begin
// 
// 	// We specify a default position/size in case there's no data in the .ini file.
// 	// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
// 	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
// 	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
// 	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
// 
// 	// Main body of the Demo window starts here.
// 	if (!ImGui::Begin("Animator", p_open, window_flags))
// 	{
// 		// Early out if the window is collapsed, as an optimization.
// 		ImGui::End();
// 		return;
// 	}
// 
// 	// Most "big" widgets share a common width settings by default. See 'Demo->Layout->Widgets Width' for details.
// 	// e.g. Use 2/3 of the space for widgets and 1/3 for labels (right align)
// 	//ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.35f);
// 	// e.g. Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
// 	ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
// 
// 	/// Animator UI
// 	if (m_selectedEntity.lock().get())
// 	{
// 		if (m_selectedEntity.lock().get()->GetComponent<Truth::Animator>().lock())
// 		{
// 			m_selectedAnimator = m_selectedEntity.lock().get()->GetComponent<Truth::Animator>();
// 
// 			uint32 selectCount = 0;
// 
// 
// 			// Graph Editor
// 
// 
// 			if (ImGui::BeginPopupContextItem())
// 			{
// 				if (ImGui::Selectable("Create State"))
// 				{
// 					Truth::Animator::AnimatorNode node;
// 					m_selectedAnimator.lock()->delegate.m_animationNodes.push_back(node);
// 				}
// 				ImGui::EndPopup();
// 			}
// 
// 			if (ImGui::Button("Fit all nodes"))
// 			{
// 				m_selectedAnimator.lock()->fit = GraphEditor::Fit_AllNodes;
// 			}
// 			ImGui::SameLine();
// 			if (ImGui::Button("Fit selected nodes"))
// 			{
// 				m_selectedAnimator.lock()->fit = GraphEditor::Fit_SelectedNodes;
// 			}
// 			GraphEditor::Show(m_selectedAnimator.lock()->delegate
// 				, m_selectedAnimator.lock()->options
// 				, m_selectedAnimator.lock()->viewState
// 				, true
// 				, &m_selectedAnimator.lock()->fit);
// 
// 
// 
// 		}
// 	}
// 
// 
// 
// 	/// End of ShowDemoWindow()
// 	ImGui::PopItemWidth();
// 	ImGui::End();
// }
#endif // EDITOR_MODE


