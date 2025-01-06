#pragma once
#define DIRECTINPUT_VERSION 0x0800
#define KEYBORD_SIZE 256
#include "Headers.h"
#include "EventHandler.h"
#include <dinput.h>

#pragma comment(lib, "dinput8")

/// <summary>
/// 키 상태 
/// </summary>
enum class KEY_STATE
{
	NONE, // 키가 눌리지 않은 상태
	DOWN, // 키가 눌려진 상태 
	HOLD, // 키가 눌린 상태
	UP,	// 키가 올라온 상태
	END,
};

/// <summary>
/// 입력을 지원하는 키 
/// </summary>
enum class KEY
{
	ESC = 0x01,

	_1, _2, _3, _4, _5, _6, _7, _8, _9, _0,

	MINUS,
	EQUALS,
	BACK,
	TAB,

	Q, W, E, R, T, Y, U, I, O, P,
	LBRACKET, RBRACKET, ENTER, LCTRL,
	A, S, D, F, G, H, J, K, L, 
	SEMICOLON, APOSTROPHE, GRAVE, LSHIFT, BACKSLASH,
	Z, X, C, V, B, N, M,
	COMMA, DOT, SLASH, RSHIFT, MULTIPLY, LALT, SPACE, CAPITAL,

	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, 

	NUMLOCK, SCROLLLOCK, 

	N7, N8, N9, NMINUS,
	N4, N5, N6, NADD,
	N1, N2, N3, N0,
	NDOT,

	NENTER = 0x9c,
	RCTRL,

	DIVIDE = 0xb5,
	RALT = 0xb8,

	PAUSE,
	HOME, UP, PUP, LEFT, RIGHT, END, DOWN, PDOWN, INSERT, DELETE__,

	LWIN, RWIN,


	LAST,
};
enum class MOUSE
{
	LMOUSE, 
	RMOUSE, 
	WHEEL,
	LAST,
};

/// <summary>
/// 키 정보
/// </summary>
struct tKeyInfo
{
	KEY_STATE state;
	bool prevPush;
};

/// <summary>
/// input mamaging class
/// </summary>
namespace Truth
{
	class InputManager
	{
	private:

		IDirectInput8* m_input;
		IDirectInputDevice8* m_keyboard;
		IDirectInputDevice8* m_mouse;

		BYTE m_keyState[KEYBORD_SIZE];
		DIMOUSESTATE2 m_mouseState;
		
		// 윈도우 포커스 여부를 확인 할 윈도우 핸들러
		HWND m_hwnd;

		tKeyInfo m_keyInfomation[KEYBORD_SIZE];
		tKeyInfo m_mouseInfomation[3];

	private:
		// 현재 프레임 마우스 위치
		POINT m_currentMousePos;
		// 과거 프레임 마우스 위치
		POINT m_prevMousePos;

		int m_oldMousePosX;
		int m_oldMousePosY;

	public:
		int m_nowMousePosX;
		int m_nowMousePosY;

	private:
		int m_mouseDx;
		int m_mouseDy;

		// 이벤트 발행을 위한 이벤트 매니저
		// 그때 그때 생성하는거 보다 미리 받아놓는다.
		std::weak_ptr<EventManager> m_eventManager;
		LPPOINT m_mousePoint;

		bool m_showCursor;

	public:
		int m_deltaWheel;
		bool m_fpsMode = false;

		int GetDeltaWheel() const { return m_deltaWheel; }

	public:
		InputManager();
		~InputManager();

	public:
		// 초기화
		void Initalize(HINSTANCE _hinstance, HWND _hwnd, std::shared_ptr<EventManager> _eventManager);

		// 업데이트
		void Update();
		// 리셋
		void Reset();

		void Finalize();

		void OnMouseMove(int _btnState, int _x, int _y);
		void ResetMouseMovement(int _x = 0, int _y = 0);

		KEY_STATE GetKeyState(KEY _eKey) const;
		KEY_STATE GetKeyState(MOUSE _eKey) const;

		int16 GetMouseMoveX() const;
		int16 GetMouseMoveY() const;

		int32 GetMousePosX();
		int32 GetMousePosY();

		void SetFPSMode(bool _isFPS);

	private:
		void SetShowCursor(bool _isShow);
	};
}
