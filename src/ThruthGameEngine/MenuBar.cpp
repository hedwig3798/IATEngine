#include "MenuBar.h"
#ifdef EDITOR_MODE
#include "imgui.h"
#include "Scene.h"
#include "Managers.h"
#include "NewScenePopup.h"
#include "EditorUI.h"
#include "LoadMapDataPopup.h"
#include "ParticleManager.h"
#include "GraphicsManager.h"
#pragma region test Scene
#include "Entity.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "Mesh.h"
#include "Camera.h"
#include "FreeCamera.h"

#include "Transform.h"
#include "TestComponent.h"

#include "SkinnedMesh.h"
#include "DirectionLight.h"
#include "Animator.h"

#include "Controller.h"

#include "NavMeshGenerater.h"

#include "DirectionLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "UI.h"
#pragma endregion test Scene
MenuBar::MenuBar(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor)
	: EditorContext("Menu Bar", _manager, _hwnd, _editor)
{
	m_isActive = true;
}

MenuBar::~MenuBar()
{

}

void MenuBar::ShowContext(bool* p_open)
{
	auto sceneManager = m_manager.lock()->Scene();
	auto currentScene = sceneManager->m_currentScene;

	// Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
	// Most functions would normally just assert/crash if the context is missing.
	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");

	// Verify ABI compatibility between caller code and compiled version of Dear ImGui. This helps detects some build issues.
	IMGUI_CHECKVERSION();


	ImGui::BeginMainMenuBar();
	ImGui::Text("\t\t\t\t\t\t\t");

	ImGui::Button("Menu");

	if (ImGui::BeginPopupContextItem("menu", 0))
	{
		if (ImGui::Selectable("New Scene"))
		{
			m_editor->AddContext(
				std::make_shared<NewScenePopup>(m_manager, m_hwnd, m_editor)
			);
		}
		if (ImGui::Selectable("Save Scene"))
		{
			sceneManager->SaveCurrentScene();

		}
		if (ImGui::Selectable("Save as Scene"))
		{
			m_openFileName.lpstrDefExt = L"scene";
			if (GetSaveFileName(&m_openFileName) != 0)
			{
				std::wstring filepath = m_openFileName.lpstrFile;
				std::vector<std::wstring> f = StringConverter::split(filepath, L'\\');

				f.back().pop_back();
				f.back().pop_back();
				f.back().pop_back();
				f.back().pop_back();
				f.back().pop_back();
				f.back().pop_back();

				sceneManager->SaveAsScene(filepath);
				USES_CONVERSION;
				currentScene->m_name = W2A(f.back().c_str());
			}
			::SetCurrentDirectory(Truth::Managers::GetRootPath().c_str());
		}
		if (ImGui::Selectable("Load Scene"))
		{
			m_openFileName.lpstrFilter = m_sceneFileFilter;
			if (GetOpenFileName(&m_openFileName) != 0)
			{
				std::wstring filepath = m_openFileName.lpstrFile;
				m_manager.lock()->Scene()->LoadSceneData(filepath);
			}
			::SetCurrentDirectory(Truth::Managers::GetRootPath().c_str());
		}
		if (ImGui::Selectable("Load Map Data"))
		{
			m_editor->AddContext(
				std::make_shared<LoadMapDataPopup>(m_manager, m_hwnd, m_editor)
			);
		}
		if (ImGui::Selectable("Create Empty"))
		{
			currentScene->AddEntity(std::make_shared<Truth::Entity>(m_manager.lock()));
		}
		if (ImGui::Selectable("Load Entity"))
		{
			m_openFileName.lpstrDefExt = L"entity";
			m_openFileName.lpstrFilter = m_entityFileFilter;
			if (GetOpenFileName(&m_openFileName) != 0)
			{
				std::wstring filepath = m_openFileName.lpstrFile;
				std::vector<std::wstring> f = StringConverter::split(filepath, L'\\');

				std::shared_ptr<Truth::Entity> e;

				std::ifstream inputstream(filepath);
				boost::archive::text_iarchive inputArchive(inputstream);
				inputArchive >> e;
				e->SetManager(m_manager);

				currentScene->AddEntity(e);
			}
			::SetCurrentDirectory(Truth::Managers::GetRootPath().c_str());
		}

		if (ImGui::Selectable("Create Particle"))
		{
			m_manager.lock()->Particle()->CreateEmptyParticle();
		}
		if (ImGui::Selectable("Set Skybox"))
		{
			if (GetOpenFileName(&m_openFileName) != 0)
			{
				::SetCurrentDirectory(Truth::Managers::GetRootPath().c_str());
				fs::path filepath = m_openFileName.lpstrFile;
				std::vector<std::wstring> f = StringConverter::split(filepath, L'\\');
				filepath = fs::relative(filepath);

				m_manager.lock()->Graphics()->ChangeSkyBox(filepath);
				currentScene->m_skyBox = filepath;
			}
			::SetCurrentDirectory(Truth::Managers::GetRootPath().c_str());
		}

		if (currentScene->m_useNavMesh)
		{
			if (ImGui::Selectable("No Nav Mesh"))
				m_manager.lock()->Scene()->m_currentScene->m_useNavMesh = false;
		}
		else
		{
			if (ImGui::Selectable("Use Nav Mesh"))
				m_manager.lock()->Scene()->m_currentScene->m_useNavMesh = true;
		}

		ImGui::EndPopup();
	}

	if (ImGui::Button("Play"))
	{
		m_manager.lock()->EditToGame();
	}
	if (ImGui::Button("Stop"))
	{
		m_manager.lock()->GameToEdit();
	}

	if (ImGui::DragFloat("brightness", &currentScene->m_brightness), 0.001f, 0.0f, 1.0f)
	{
		currentScene->SetBrightness();
	}

	static float step = 1.0f;
	static float dtdis = 1.0f;
	ImGui::Text("frame : %.2f\t", 1 / dtdis);
	if (step < 0.0f)
	{
		dtdis = m_manager.lock()->Time()->GetADT();
		step += 1.0f;
	}
	step -= dtdis;

	ImGui::EndMainMenuBar();
}
#endif // EDITOR_MODE
