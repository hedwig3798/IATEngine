#include "LoadMapDataPopup.h"
#include "imgui.h"
#include "Managers.h"
#include "SceneManager.h"
#include "Scene.h"

LoadMapDataPopup::LoadMapDataPopup(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor)
	: EditorContext("Load Map Data", _manager, _hwnd, _editor)
	, m_inputTextBuffer{}
{
}

LoadMapDataPopup::~LoadMapDataPopup()
{

}

void LoadMapDataPopup::ShowContext(bool* p_open)
{
	ImGui::OpenPopup(m_name.c_str());
	if (ImGui::BeginPopupModal(m_name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Enter your text below:");
		ImGui::InputText("##input", m_inputTextBuffer, IM_ARRAYSIZE(m_inputTextBuffer));

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			USES_CONVERSION;
			auto& s = m_manager.lock()->Scene()->m_currentScene;
			s->ResetMapData();
			s->m_mapPath = A2W(m_inputTextBuffer);
			s->LoadUnityData(s->m_mapPath);
			ImGui::CloseCurrentPopup();
			m_isActive = false;
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			m_isActive = false;
		}
		ImGui::EndPopup();
	}
}
