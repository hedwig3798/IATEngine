#include "LinkBonePopup.h"
#include "imgui.h"
#include "Entity.h"
#include "SkinnedMesh.h"

LinkBonePopup::LinkBonePopup(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor, std::weak_ptr<Truth::Entity> _selectedEntity)
	: EditorContext("Link Bone", _manager, _hwnd, _editor)
	, m_selectedEntity(_selectedEntity)
{

}

LinkBonePopup::~LinkBonePopup()
{

}

void LinkBonePopup::ShowContext(bool* p_open)
{
	if (!m_selectedEntity.lock()->m_parent.expired())
	{
		ImGui::OpenPopup("Input String");
		if (ImGui::BeginPopupModal("Input String", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static std::string input;

			ImGui::Text("Choose Bone");
			auto& boneMap = m_selectedEntity.lock()->m_parent.lock()->GetComponent<Truth::SkinnedMesh>().lock()->GetBoneMap();

			std::vector<const char*> boneNames;

			for (auto& b : boneMap)
			{
				boneNames.push_back(b.first.c_str());
			}

			int selected = -1;
			if (ImGui::ListBox("bone", &selected, boneNames.data(), static_cast<int>(boneNames.size()), 10))
			{
				input = boneNames[selected];
			}

			ImGui::Text(input.c_str());

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				m_selectedEntity.lock()->LinkBone(input);
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
}
