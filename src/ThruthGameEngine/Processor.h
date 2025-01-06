#pragma once
#include "Headers.h"
#include "Managers.h"
#include "SceneManager.h"
#include <thread>
#include <mutex>

#define CONVERT_DATA
#define MAX_THREAD_COUNT 16
namespace Truth
{
	class Managers;
	class Scene;
}
	class EditorUI;

namespace Ideal
{
	class IdealRenderer;
	class IRenderScene;
	class ISkinnedMeshObject;
}

/// <summary>
/// 엔진의 프로세서
/// 엔진 전체의 진행을 담당한다.
/// </summary>
class Processor
{
private:

#ifdef CONVERT_DATA
	const std::vector<std::wstring> m_aniData =
	{
		/// Player animation List
		L"PlayerAnimations/ChargedAttack/M_Big_Sword@Attack_3Combo_1.FBX",
		L"PlayerAnimations/ChargedAttack/M_Big_Sword@Attack_3Combo_2.FBX",
		L"PlayerAnimations/ChargedAttack/M_Big_Sword@Attack_3Combo_3.FBX",
		L"PlayerAnimations/ChargedAttack/M_Big_Sword@Attack_7Combo_7.FBX",
		L"PlayerAnimations/ChargedAttack/M_katana_Blade@Skill_H.FBX",
		L"PlayerAnimations/ComboAttackReady/Buff_01.fbx",
		L"PlayerAnimations/Dodge/roll_front.fbx",
		L"PlayerAnimations/Fall/unitychan_Fall.fbx",
		L"PlayerAnimations/Guard/Revenge_Guard_Loop.fbx",
		L"PlayerAnimations/Hit/hit01.fbx",
		L"PlayerAnimations/Idle/idle.fbx",
		L"PlayerAnimations/Move/BackStep/M_katana_Blade@Walk_ver_A_Back.fbx",
		L"PlayerAnimations/Move/BackStep/M_katana_Blade@Walk_ver_A_Back_L45.fbx",
		L"PlayerAnimations/Move/BackStep/M_katana_Blade@Walk_ver_A_Back_R45.fbx",
		L"PlayerAnimations/Move/Front/M_katana_Blade@Walk_ver_A_Front_L90.fbx",
		L"PlayerAnimations/Move/Front/M_katana_Blade@Walk_ver_A_Front_R90.fbx",
		L"PlayerAnimations/Move/FrontWalk/M_katana_Blade@Walk_ver_A_Front.fbx",
		L"PlayerAnimations/Move/FrontWalk/M_katana_Blade@Walk_ver_A_Front_L45.fbx",
		L"PlayerAnimations/Move/FrontWalk/M_katana_Blade@Walk_ver_A_Front_R45.fbx",
		L"PlayerAnimations/Move/Run/run_strafe_front.fbx",
		L"PlayerAnimations/Move/Run/run_strafe_frontleft.fbx",
		L"PlayerAnimations/Move/Run/run_strafe_frontright.fbx",
		L"PlayerAnimations/NormalAttack/Combo_01_1.fbx",
		L"PlayerAnimations/NormalAttack/Combo_01_2.fbx",
		L"PlayerAnimations/NormalAttack/Combo_01_3.fbx",
		L"PlayerAnimations/NormalAttack/Combo_02_3.fbx",
		L"PlayerAnimations/NormalAttack/Combo_03_3.fbx",
		L"PlayerAnimations/NormalAttack/Combo_03_4.fbx"
	};

	const std::vector<std::wstring> m_skelMeshdata =
	{
		L"PlayerAnimations/ChargedAttack/M_Big_Sword@Attack_3Combo_1.FBX"
	};

	const std::vector<std::wstring> m_staticMeshData =
	{
	};

#endif // CONVERT_DATA

#ifdef EDITOR_MODE
	std::unique_ptr<EditorUI> m_editor;
#endif // EDITOR_MODE

	std::shared_ptr<Truth::Managers> m_manager;

	std::shared_ptr<Ideal::IMeshObject> mesh;
	std::shared_ptr<Ideal::ISkinnedMeshObject> m_cat;
	std::shared_ptr<Ideal::IAnimation> m_walkAnim;
	// 윈도우 생성을 위한 변수
	HWND m_hwnd;
	HINSTANCE m_hinstance;
	MSG m_msg;

	static Ideal::IdealRenderer* g_Renderer;
	static Truth::InputManager* g_inputmanager;
	static Truth::SceneManager* g_sceneManager;

	uint32 m_wight;
	uint32 m_height;

	bool show_demo_window = true;

	const std::string m_savedFilePath = "../Scene/";

	LPCWSTR m_icon;

public:
	// 특수 멤버함수
	Processor();
	~Processor();

	// 초기화
	void Initialize(HINSTANCE _hInstance, LPCWSTR _icon = IDI_APPLICATION);
	void Finalize();
	void Process();
	void Loop();

	void SaveCurrentScene();
	void SaveScene(std::shared_ptr<Truth::Scene> _scene);
	void LoadScene(std::wstring _path);

	// 윈도우 함수
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	// LRESULT CALLBACK WndProcInClass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	std::shared_ptr<Truth::Managers> GetManagers() const { return m_manager; };

private:
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();

	void CreateMainWindow(HINSTANCE _hInstance, uint32 _width = 1920, uint32 _height = 1080, const wchar_t szAppName[] = L"Truth Engine");
	void InitializeManager();

#ifdef CONVERT_DATA
	void ConvertDataUseTrhead();
	void ConvertData();

	static void ConvertAniFbxData(const std::wstring& _path);
	static void ConvertSkelFbxData(const std::wstring& _path);
	static void ConvertStaticFbxData(const std::wstring& _path);
#endif // CONVERT_DATA

};

