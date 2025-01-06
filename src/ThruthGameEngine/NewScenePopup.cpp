#include "NewScenePopup.h"
#include "imgui.h"
#include "Scene.h"
#include "Managers.h"

NewScenePopup::NewScenePopup(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor)
	: EditorContext("New Scene Popup", _manager, _hwnd, _editor)
	, m_inputTextBuffer{}
{
}

NewScenePopup::~NewScenePopup()
{
}

void NewScenePopup::ShowContext(bool* p_open)
{
	ImGui::OpenPopup(m_name.c_str());
	if (ImGui::BeginPopupModal(m_name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Enter your text below:");
		ImGui::InputText("##input", m_inputTextBuffer, IM_ARRAYSIZE(m_inputTextBuffer));

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			USES_CONVERSION;
			std::shared_ptr<Truth::Scene> ns = std::make_shared<Truth::Scene>(m_manager.lock());
			ns->m_name = m_inputTextBuffer;
			m_manager.lock()->Scene()->SaveScene(ns);
			std::string path = "../Scene/" + ns->m_name + ".scene";
			m_manager.lock()->Scene()->LoadSceneData(A2W(path.c_str()));
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
