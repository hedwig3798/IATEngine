#include "TextUI.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Texture.h"
#include "IText.h"
#include "InputManager.h"
#include "ButtonBehavior.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::TextUI)

/// <summary>
/// 생성자
/// </summary>
Truth::TextUI::TextUI()
	: m_textSprite()
	, m_spriteSize{ 1, 1 }
	, m_textSize{ 100.f, 100.f }
	, m_alpha(1.0f)
	, m_zDepth(0.0f)
	, m_position(100.0f, 100.0f)
	, m_behavior(nullptr)
	, m_fontSize(20)
	, m_text(L"test")
{
	m_finalSize = { m_spriteSize.x * m_textSize.x, m_spriteSize.y * m_textSize.y };
}

/// <summary>
/// 소멸자
/// </summary>
Truth::TextUI::~TextUI()
{
	m_managers.lock()->Graphics()->DeleteTextSprite(m_textSprite.lock());
}

/// <summary>
/// 텍스트 변경
/// 기존 텍스트와 같으면 무시한다.
/// </summary>
/// <param name="_text">변경할 텍스트</param>
void Truth::TextUI::ChangeText(const std::wstring& _text)
{
	return;

	if (!m_text._Equal(_text))
	{
		m_textSprite.lock()->ChangeText(_text);
		m_text = _text;
	}
}

void Truth::TextUI::SetAlpha(float _alpha)
{
	return;

	m_alpha = _alpha;
	m_textSprite.lock()->SetAlpha(_alpha);
}

/// <summary>
/// 초기화
/// </summary>
void Truth::TextUI::Initialize()
{
	return;
	if (m_behavior)
		m_behavior->Initialize(m_managers, ::Cast<TextUI, Component>(shared_from_this()), m_owner);
	auto gp = m_managers.lock()->Graphics();
	m_textSprite = gp->CreateTextSprite(
		static_cast<uint32>(m_textSize.x), 
		static_cast<uint32>(m_textSize.y),
		m_fontSize);
	m_textSprite.lock()->SetScale({ m_spriteSize.x, m_spriteSize.y });
	m_finalSize = { m_spriteSize.x * m_textSize.x, m_spriteSize.y * m_textSize.y };
	m_textSprite.lock()->SetPosition({ m_position.x - (m_finalSize.x * 0.5f), m_position.y - (m_finalSize.y * 0.5f) });
	m_textSprite.lock()->SetActive(IsActive());
	m_textSprite.lock()->SetAlpha(m_alpha);
	m_textSprite.lock()->SetZ(m_zDepth);
	m_textSprite.lock()->ChangeText(m_text);
}

void Truth::TextUI::Start()
{
	return;

	if (m_behavior)
		m_behavior->Start();
	ResizeWindow();
}

void Truth::TextUI::Awake()
{
	return;

	if (m_behavior)
		m_behavior->Awake();
}

void Truth::TextUI::Update()
{
	return;

#ifdef EDITOR_MODE
	ResizeWindow();
#endif
	if (m_behavior)
		m_behavior->Update();
}

/// <summary>
/// 해당 엔티티가 현재 활성화인지
/// TODO: 엔티티 클래스로 기능 이전 예정
/// </summary>
/// <returns></returns>
bool Truth::TextUI::IsActive()
{
	return false;

	auto owner = m_owner.lock();

	bool isActive;
	bool isParentActive;
	isActive = owner->m_isActive;

	if (owner->HasParent())
		isParentActive = owner->m_parent.lock()->m_isActive;
	else
		isParentActive = true;

	return isActive && isParentActive;
}

/// <summary>
/// 스프라이트 렌터 여부
/// </summary>
/// <param name="_active">활성화 여부</param>
void Truth::TextUI::SetSpriteActive(bool _active)
{
	return;

	m_textSprite.lock()->SetActive(_active);
}

#ifdef EDITOR_MODE
void Truth::TextUI::EditorSetValue()
{
	return;
	auto gp = m_managers.lock()->Graphics();
	gp->DeleteTextSprite(m_textSprite.lock());
	m_textSprite = gp->CreateTextSprite(
		static_cast<uint32>(m_textSize.x), 
		static_cast<uint32>(m_textSize.y),
		m_fontSize);
	m_textSprite.lock()->SetScale({ m_spriteSize.x, m_spriteSize.y });
	m_finalSize = { m_spriteSize.x * m_textSize.x, m_spriteSize.y * m_textSize.y };
	m_textSprite.lock()->SetPosition({ m_position.x - (m_finalSize.x * 0.5f), m_position.y - (m_finalSize.y * 0.5f) });
	m_textSprite.lock()->SetActive(IsActive());
	m_textSprite.lock()->SetAlpha(m_alpha);
	m_textSprite.lock()->SetZ(m_zDepth);
	m_textSprite.lock()->ChangeText(m_text);
}
#endif // EDITOR_MODE
