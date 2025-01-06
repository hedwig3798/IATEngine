#include "Processor.h"
#include "Managers.h"
#include "SceneManager.h"
#include "ISkinnedMeshObject.h"
#include "IRenderScene.h"
#include "IAnimation.h"
#include "GraphicsManager.h"
#include "InputManager.h"

#ifdef EDITOR_MODE
#include "imgui.h"
#include "EditorUI.h"
#include "UnityParser.h"
#endif // EDITOR_MODE

#include "IdealRenderer.h"
#include "ThreadPool.h"

#include <time.h>

Ideal::IdealRenderer* Processor::g_Renderer = nullptr;
Truth::InputManager* Processor::g_inputmanager = nullptr;
Truth::SceneManager* Processor::g_sceneManager = nullptr;

Processor::Processor()
	: m_hwnd(nullptr)
	, m_msg()
	, m_manager(nullptr)
	, m_wight(0)
	, m_height(0)
#ifdef EDITOR_MODE
	, m_editor(nullptr)
#endif // EDITOR_MODE
	, m_hinstance()
{
	DEBUG_PRINT("start process\n");
}

Processor::~Processor()
{
	Finalize();
	DEBUG_PRINT("end process\n");
}

/// <summary>
/// 프로세서 초기화
/// </summary>
/// <param name="_hInstance"></param>
void Processor::Initialize(HINSTANCE _hInstance, LPCWSTR _icon)
{
	m_icon = _icon;
	m_hinstance = _hInstance;
	CreateMainWindow(_hInstance, 1920, 1080, L"Chronos");
	InitializeManager();
	g_inputmanager = m_manager->Input().get();
	g_sceneManager = m_manager->Scene().get();
#ifdef CONVERT_DATA
	// ConvertData();
	// ConvertSkelFbxData(L"AsciiAniTest/idelTest.fbx");
	// ConvertSkelFbxData(L"AsciiAniTest/idelTest.fbx");
	// ConvertSkelFbxData(L"AsciiAniTest/FieldMob.fbx");
	// ConvertSkelFbxData(L"AsciiAniTest/Enemy_B_Idle.fbx");
	// ConvertAniFbxData(L"AsciiAniTest/idelTest.fbx");
	
// 	ConvertAniFbxData(L"PlayerAnimations/Skill/SlashSkill.fbx");
// 	ConvertAniFbxData(L"PlayerAnimations/Skill/TimeStop.fbx");
// 	ConvertAniFbxData(L"PlayerAnimations/ChargedAttack/ChargedAbility.fbx");
// 	ConvertAniFbxData(L"PlayerAnimations/Guard/GuardHit.fbx");
// 	ConvertAniFbxData(L"PlayerAnimations/Guard/GuardLoop.fbx");
// 	ConvertAniFbxData(L"PlayerAnimations/NormalAttack/NormalVariation1.fbx");
// 	ConvertAniFbxData(L"PlayerAnimations/NormalAttack/NormalVariation2.fbx");
// 	ConvertAniFbxData(L"PlayerAnimations/ComboAttackReady/ComboAttackReady.fbx");
//	ConvertAniFbxData(L"PlayerAnimations/ChargedAttack/ChargedAbility.fbx");
//	ConvertAniFbxData(L"PlayerAnimations/NormalAttack/RushAttack.fbx");
//	ConvertAniFbxData(L"PlayerAnimations/NormalAttack/NormalAbility.fbx");


// 	ConvertStaticFbxData(L"DebugObject/debugCube.fbx");
// 	ConvertStaticFbxData(L"DebugObject/debugSphere.fbx");
// 	ConvertAniFbxData(L"BossAnimations/Attacks/AttackShockWave.fbx");
// 	ConvertAniFbxData(L"BossAnimations/Attacks/AttackSpin.fbx");
// 	ConvertAniFbxData(L"BossAnimations/Attacks/JumpAttack.fbx");
// 	ConvertAniFbxData(L"BossAnimations/Attacks/BlackHoleSummon.fbx");
// 	ConvertAniFbxData(L"BossAnimations/Attacks/Combo3-1.fbx");
// 	ConvertAniFbxData(L"BossAnimations/Attacks/Combo2-1.fbx");
// 	ConvertAniFbxData(L"BossAnimations/Attacks/Combo1-2.fbx");
// 	ConvertAniFbxData(L"BossAnimations/Attacks/Combo1-1.fbx");
// 	ConvertAniFbxData(L"BossAnimations/Strafe/BossWalk.fbx");
// 	ConvertAniFbxData(L"BossAnimations/Idle/BattleEntrance.fbx");
// 	ConvertAniFbxData(L"BossAnimations/Idle/BossEntranceRoot.fbx");
// 	ConvertAniFbxData(L"BossAnimations/Attacks/BossSwordShoot.fbx");


// 	ConvertAniFbxData(L"EnemyAnimations/MeleeEnemy/Down/Down.fbx");
// 	ConvertAniFbxData(L"EnemyAnimations/MeleeEnemy/Down/Fall.fbx");
// 	ConvertAniFbxData(L"EnemyAnimations/MeleeEnemy/Down/FallAttack.fbx");
// 	ConvertAniFbxData(L"EnemyAnimations/MeleeEnemy/Down/GetUp.fbx");
// 	ConvertAniFbxData(L"EnemyAnimations/MeleeEnemy/Down/KnockBack.fbx");

// 	ConvertStaticFbxData(L"EnemyAnimations/RangeEnemy/Weapon/SM_mon_bow.fbx");
// 	ConvertStaticFbxData(L"EnemyAnimations/MeleeEnemy/Weapon/SM_mon_shield.fbx");
// 	ConvertStaticFbxData(L"EnemyAnimations/MeleeEnemy/Weapon/SM_mon_sword.fbx");
// 	ConvertStaticFbxData(L"BossAnimations/Spear/SM_niddle_sub.fbx");

 	int a = 0;
	// ConvertDataUseTrhead();
#endif // CONVERT_DATA

#ifdef EDITOR_MODE

	Truth::UnityParser up(m_manager->Graphics().get());
	// up.SetRootDir("E:\\Projects\\ChronosUnity\\Kronos_IAT_Unity\\Cronos_URP");
	// up.ParseUnityFile("E:\\Projects\\ChronosUnity\\Kronos_IAT_Unity\\Cronos_URP\\Assets\\Scenes\\ArtRoom\\FinalScene\\0_TitleTest.unity");
	// up.ParseUnityFile("E:\\Projects\\ChronosUnity\\Kronos_IAT_Unity\\Cronos_URP\\Assets\\Scenes\\ArtRoom\\FinalScene\\1_HN_Scene2.unity");
	// up.ParseUnityFile("E:\\Projects\\ChronosUnity\\Kronos_IAT_Unity\\Cronos_URP\\Assets\\Scenes\\ArtRoom\\FinalScene\\2_JS_Scene2.unity");
	// up.ParseUnityFile("E:\\Projects\\ChronosUnity\\Kronos_IAT_Unity\\Cronos_URP\\Assets\\Scenes\\ArtRoom\\FinalScene\\3_EJ_Scene2.unity");
	// up.ParseUnityFile("E:\\Projects\\ChronosUnity\\Kronos_IAT_Unity\\Cronos_URP\\Assets\\Scenes\\ArtRoom\\BOSSROOM\\Hanna_BOSSROOM.unity");
	// up.ParseMatarialData();
	m_editor = std::make_unique<EditorUI>(m_manager, m_hwnd);
#endif // EDITOR_MODE
}

void Processor::Finalize()
{
	m_manager->Finalize();
}

void Processor::Process()
{
	clock_t start, finish;

	start = clock();
	Update();
	LateUpdate();
	finish = clock();

	std::string temp = std::to_string(finish - start);
	temp = std::string("update : ") + temp;
	temp += " / ";
	// DEBUG_PRINT(temp.c_str());

	start = clock();

	Render();
	finish = clock();

	temp = std::to_string(finish - start);
	temp = std::string("render : ") + temp;
	temp += " \n ";

	// DEBUG_PRINT(temp.c_str());
}

void Processor::Loop()
{
	m_manager->Scene()->StartGameScene();
	while (true)
	{
		if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_msg.message == WM_QUIT) break;

			DispatchMessage(&m_msg);
			TranslateMessage(&m_msg);
		}
		else
		{
			// 무언가 작동하는 부분
			Process();
		}
	}
}

void Processor::LoadScene(std::wstring _path)
{
	m_manager->Scene()->LoadSceneData(_path);
}

LRESULT CALLBACK Processor::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;
	if (g_Renderer)
	{
		g_Renderer->SetImGuiWin32WndProcHandler(hWnd, message, wParam, lParam);
	}
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_SIZE:
	{
		if (g_Renderer)
		{
			RECT rect;
			GetClientRect(hWnd, &rect);
			//AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
			DWORD width = rect.right - rect.left;
			DWORD height = rect.bottom - rect.top;
			g_Renderer->Resize(width, height);
			g_Renderer->SetDisplayResolutionOption(Ideal::Resolution::EDisplayResolutionOption::R_1920_1080);
		}

		if (g_sceneManager)
			g_sceneManager->m_currentScene->ResizeWindow();

		break;
	}
	case WM_SYSCOMMAND:
	{
		if (SC_KEYMENU == (wParam & 0xFFF0) && (lParam == 13))
		{
			g_Renderer->ToggleFullScreenWindow();
			g_Renderer->SetDisplayResolutionOption(Ideal::Resolution::EDisplayResolutionOption::R_1920_1080);
			if (g_sceneManager)
				g_sceneManager->m_currentScene->ResizeWindow();
		}

		[[fallthrough]];
	}
	default:
		if (g_inputmanager)
			g_inputmanager->m_deltaWheel = 0;

		return DefWindowProc(hWnd, message, wParam, lParam);

	}
	return 0;
}

void Processor::SaveCurrentScene()
{
	m_manager->Scene()->SaveCurrentScene();
}

void Processor::SaveScene(std::shared_ptr<Truth::Scene> _scene)
{
	m_manager->Scene()->SaveScene(_scene);
}

void Processor::Update()
{
	m_manager->Update();
}

void Processor::LateUpdate()
{
	m_manager->LateUpdate();
}

void Processor::FixedUpdate()
{
	m_manager->FixedUpdate();
}

void Processor::Render()
{
#ifdef EDITOR_MODE
	if (g_Renderer)
	{
		g_Renderer->ClearImGui();
		//ImGui::ShowDemoWindow(&show_demo_window);
		m_editor->RenderUI(&show_demo_window);
	}
#endif // EDITOR_MODE

	// 	if (m_manager->Input()->GetKeyState(KEY::P) == KEY_STATE::DOWN)
	// 	{
	// 		m_manager->EditToGame();
	// 	}
	// 
	// 	if (m_manager->Input()->GetKeyState(KEY::O) == KEY_STATE::DOWN)
	// 	{
	// 		m_manager->GameToEdit();
	// 	}

	m_manager->Render();
}

void Processor::CreateMainWindow(HINSTANCE _hInstance, uint32 _width, uint32 _height, const wchar_t szAppName[])
{
	m_wight = _width;
	m_height = _height;

	// 창 지정
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Processor::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _hInstance;
	wcex.hIcon = LoadIcon(_hInstance, m_icon);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szAppName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, m_icon);

	RegisterClassExW(&wcex);

	RECT windowRect = { 0,0, 1920, 1080 };
	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

	m_hwnd = CreateWindowW(szAppName, szAppName, WS_OVERLAPPEDWINDOW,
		100, 100, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, _hInstance, NULL);

	if (!m_hwnd)
	{
		return;
	}

	ShowWindow(m_hwnd, SW_SHOWNORMAL);
	UpdateWindow(m_hwnd);

	RECT nowRect;
	DWORD _style = (DWORD)GetWindowLong(m_hwnd, GWL_STYLE);
	DWORD _exstyle = (DWORD)GetWindowLong(m_hwnd, GWL_EXSTYLE);

	GetWindowRect(m_hwnd, &nowRect);

	// 	RECT newRect = {};
	// 	newRect.left = 0;
	// 	newRect.top = 0;
	// 	newRect.right = _width;
	// 	newRect.bottom = _height;
	// 
	// 	//AdjustWindowRectEx(&newRect, _style, NULL, _exstyle);
	// 	//AdjustWindowRectEx(&newRect, _style, NULL, _exstyle);
	// 
	// 	// 클라이언트 영역보다 윈도 크기는 더 커야 한다. (외곽선, 타이틀 등)
	// 	int _newWidth = (newRect.right - newRect.left);
	// 	int _newHeight = (newRect.bottom - newRect.top);
	// 
	// 	SetWindowPos(m_hwnd, HWND_NOTOPMOST, nowRect.left, nowRect.top,
	// 		_newWidth, _newHeight, SWP_SHOWWINDOW);
}

void Processor::InitializeManager()
{
	m_manager = std::make_shared<Truth::Managers>();
	m_manager->Initialize(m_hinstance, m_hwnd, m_wight, m_height);
	g_Renderer = m_manager->Graphics()->GetRenderer().get();
	// g_Renderer->ConvertAssetToMyFormat(L"debugCube/debugCube.fbx");
}

#ifdef CONVERT_DATA
void Processor::ConvertDataUseTrhead()
{
	ThreadPool th(MAX_THREAD_COUNT);

	std::function<void(const std::wstring&)> fAni = Processor::ConvertAniFbxData;
	std::function<void(const std::wstring&)> fSkel = Processor::ConvertSkelFbxData;
	std::function<void(const std::wstring&)> fStatic = Processor::ConvertStaticFbxData;
	std::vector<std::future<void>> futures;

	for (auto& path : m_aniData)
	{
		futures.emplace_back(th.EnqueueJob(fAni, path));
	}
	for (auto& path : m_skelMeshdata)
	{
		futures.emplace_back(th.EnqueueJob(fSkel, path));
	}
	for (auto& path : m_staticMeshData)
	{
		futures.emplace_back(th.EnqueueJob(fStatic, path));
	}
}

void Processor::ConvertData()
{
	for (auto& path : m_aniData)
	{
		ConvertAniFbxData(path);
	}
	for (auto& path : m_skelMeshdata)
	{
		ConvertSkelFbxData(path);
	}
	for (auto& path : m_staticMeshData)
	{
		ConvertStaticFbxData(path);
	}
}

void Processor::ConvertAniFbxData(const std::wstring& _path)
{
	g_Renderer->ConvertAnimationAssetToMyFormat(_path);
}

void Processor::ConvertSkelFbxData(const std::wstring& _path)
{
	g_Renderer->ConvertAssetToMyFormat(_path, true);
}

void Processor::ConvertStaticFbxData(const std::wstring& _path)
{
	g_Renderer->ConvertAssetToMyFormat(_path, false, true);
}
#endif // CONVERT_DATA
