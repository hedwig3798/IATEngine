#include "InputManager.h"
#include "Managers.h"

Truth::InputManager::InputManager()
	: m_keyInfomation()
	, m_hwnd(nullptr)
	, m_currentMousePos{}
	, m_prevMousePos{}
	, m_oldMousePosX(0)
	, m_oldMousePosY(0)
	, m_nowMousePosX(0)
	, m_nowMousePosY(0)
	, m_mouseDx(0)
	, m_mouseDy(0)
	, m_deltaWheel(0)
	, m_keyboard(nullptr)
	, m_mouse(nullptr)
	, m_input(nullptr)
	, m_mouseInfomation()
	, m_keyState()
	, m_mouseState()
	, m_showCursor(true)
	, m_fpsMode(false)
{
	DEBUG_PRINT("Create InputManager\n");
	m_mousePoint = new POINT;
}

Truth::InputManager::~InputManager()
{
	m_keyboard->Unacquire();
	m_mouse->Unacquire();
	DEBUG_PRINT("Finalize InputManager\n");
}

/// <summary>
/// 초기화
/// </summary>
/// <param name="_hwnd">윈도우 포커스 확인을 위한 핸들러</param>
void Truth::InputManager::Initalize(HINSTANCE _hinstance, HWND _hwnd, std::shared_ptr<EventManager> _eventManager)
{
	HRESULT hr;
	hr = DirectInput8Create(
		_hinstance,
		DIRECTINPUT_HEADER_VERSION,
		IID_IDirectInput8,
		(void**)&m_input,
		nullptr
	);
	assert(SUCCEEDED(hr) && "\ndirect input initalize fail.\nCannot Create dinput");


	m_input->CreateDevice(GUID_SysKeyboard, &m_keyboard, nullptr);
	assert(SUCCEEDED(hr) && "\ndirect input initalize fail\nCannot Create dKeyboard");

	m_input->CreateDevice(GUID_SysMouse, &m_mouse, nullptr);
	assert(SUCCEEDED(hr) && "\ndirect input initalize fail\nCannot Create dMouse");

	m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr) && "\ndirect input initalize fail\nCannot Set Keyboard Data");
	m_keyboard->SetCooperativeLevel(_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	m_mouse->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(hr) && "\ndirect input initalize fail\nCannot Set Keyboard Data");
	m_mouse->SetCooperativeLevel(_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	m_keyboard->Acquire();
	m_mouse->Acquire();

	m_hwnd = _hwnd;

	for (UINT i = 0; i < static_cast<UINT>(KEY::END); i++)
	{
		m_keyInfomation[i] = tKeyInfo{ KEY_STATE::NONE, false };
	}

	m_eventManager = _eventManager;
}

/// <summary>
/// 업데이트 합수
/// 키의 상태에 따라 이벤트를 발행한다.
/// </summary>
void Truth::InputManager::Update()
{
	HRESULT hr = S_OK;

	m_keyboard->Acquire();
	m_mouse->Acquire();

	m_keyboard->GetDeviceState(KEYBORD_SIZE, m_keyState);
	hr = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&m_mouseState);

	for (int i = 0; i < KEYBORD_SIZE; i++)
	{
		if (m_keyState[i] & 0x80)
		{
			if (m_keyInfomation[i].prevPush)
			{
				m_keyInfomation[(int)i].state = KEY_STATE::HOLD;
			}
			else
			{
				m_keyInfomation[(int)i].state = KEY_STATE::DOWN;
			}

			m_keyInfomation[i].prevPush = true;
		}
		else
		{
			if (m_keyInfomation[i].prevPush)
			{
				m_keyInfomation[(int)i].state = KEY_STATE::UP;
			}
			else
			{
				m_keyInfomation[(int)i].state = KEY_STATE::NONE;
			}

			m_keyInfomation[i].prevPush = false;
		}
	}

	m_mousePoint->x = m_mouseState.lX;
	m_mousePoint->y = m_mouseState.lY;
	m_deltaWheel = m_mouseState.lZ;

	for (int i = 0; i < 3; i++)
	{
		if (m_mouseState.rgbButtons[i] & 0x80)
		{
			if (m_mouseInfomation[i].prevPush)
			{
				m_mouseInfomation[i].state = KEY_STATE::HOLD;
			}
			else
			{
				m_mouseInfomation[i].state = KEY_STATE::DOWN;
			}

			m_mouseInfomation[i].prevPush = true;
		}
		else
		{
			if (m_mouseInfomation[i].prevPush)
			{
				m_mouseInfomation[i].state = KEY_STATE::UP;
			}
			else
			{
				m_mouseInfomation[i].state = KEY_STATE::NONE;
			}

			m_mouseInfomation[i].prevPush = false;
		}
	}

	GetCursorPos(m_mousePoint);

	OnMouseMove(0, m_mousePoint->x, m_mousePoint->y);

	if (m_fpsMode)
	{
		RECT rect;
		::GetWindowRect(m_hwnd, &rect);
		SetCursorPos((rect.right - rect.left) * 0.5f + rect.left, (rect.bottom - rect.top) * 0.5f + rect.top);

		if (GetKeyState(KEY::M) == KEY_STATE::DOWN)
		{
			SetShowCursor(true);
			m_fpsMode = false;
		}
	}
	else
	{
		if (GetKeyState(KEY::N) == KEY_STATE::DOWN)
		{
			SetShowCursor(false);
			m_fpsMode = true;
		}
	}
}

/// <summary>
/// 리셋
/// 모든 키의 과거 상태를 눌리지 않는 상태로 바꾸낟.
/// </summary>
void Truth::InputManager::Reset()
{
	for (size_t i = 0; i < static_cast<size_t>(KEY::END); i++)
	{
		this->m_keyInfomation[i].prevPush = false;
	}
}

void Truth::InputManager::Finalize()
{
	Reset();
}



void Truth::InputManager::OnMouseMove(int _btnState, int _x, int _y)
{
	if (m_fpsMode)
	{
		RECT rect;
		::GetWindowRect(m_hwnd, &rect);
		m_oldMousePosX = (rect.right - rect.left) * 0.5f + rect.left;
		m_oldMousePosY = (rect.bottom - rect.top) * 0.5f + rect.top;
	}
	else
	{
		m_oldMousePosX = m_nowMousePosX;
		m_oldMousePosY = m_nowMousePosY;
	}

	m_nowMousePosX = _x;
	m_nowMousePosY = _y;

	m_mouseDx = m_nowMousePosX - m_oldMousePosX;
	m_mouseDy = m_nowMousePosY - m_oldMousePosY;
}

void Truth::InputManager::ResetMouseMovement(int _x /*= 0*/, int _y /*= 0*/)
{
	m_mouseDx = 0;
	m_mouseDy = 0;
}

KEY_STATE Truth::InputManager::GetKeyState(KEY _eKey) const
{
	return m_keyInfomation[(int)_eKey].state;
}

KEY_STATE Truth::InputManager::GetKeyState(MOUSE _eKey) const
{
	return m_mouseInfomation[(int)_eKey].state;
}

int16 Truth::InputManager::GetMouseMoveX() const
{
	return m_mouseDx;
}

int16 Truth::InputManager::GetMouseMoveY() const
{
	return m_mouseDy;
}

int32 Truth::InputManager::GetMousePosX()
{
	return m_nowMousePosX;

}

int32 Truth::InputManager::GetMousePosY()
{
	return m_nowMousePosY;
}

void Truth::InputManager::SetFPSMode(bool _isFPS)
{
	if (_isFPS != m_fpsMode)
	{
		m_fpsMode = _isFPS;
		SetShowCursor(!_isFPS);
	}
}

void Truth::InputManager::SetShowCursor(bool _isShow)
{
	if (_isShow != m_showCursor)
	{
		::ShowCursor(_isShow);
		m_showCursor = _isShow;
	}
}

