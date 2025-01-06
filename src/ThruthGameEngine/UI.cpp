#include "UI.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Texture.h"
#include "ISprite.h"
#include "InputManager.h"
#include "ButtonBehavior.h"
#include "..\EngineDemo\TitleUI.h"
#include "..\EngineDemo\ClearUI.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::UI)

Truth::UI::UI()
	: m_sprite()
	, m_texturePath()
	, m_size{ 100.0f, 100.0f }
	, m_alpha(1.0f)
	, m_zDepth(0.0f)
	, m_position(100.0f, 100.0f)
	, m_prevState(BUTTON_STATE::IDEL)
	, m_state(BUTTON_STATE::IDEL)
	, m_rect{}
	, m_behavior(nullptr)
	, m_scale(1.0f, 1.0f)
	, m_minSampling(0.f, 0.f)
	, m_maxSampling(1.0f, 1.0f)
	, m_isButton(false)
{
	m_texturePath[0] = "../Resources/Textures/UI/title_button_basic.png";
	m_texturePath[1] = "../Resources/Textures/UI/title_button_blue.png";
	m_texturePath[2] = "../Resources/Textures/UI/title_button_click.png";
}

Truth::UI::~UI()
{
	if (m_sprite)
		m_managers.lock()->Graphics()->DeleteUISpriteSet(m_sprite);
}

void Truth::UI::SetScale(const Vector2& _scale, bool _centerPos)
{
	m_scale = _scale;
	for (uint32 i = 0; i < 3; i++)
	{
		auto tex = m_sprite->GetTex(i);
		float w = static_cast<float>(tex->w);
		float h = static_cast<float>(tex->h);

		Vector2 gpScale = { (m_size.x / w) * m_scale.x, (m_size.y / h) * m_scale.y };
		Vector2 gpPosition = { (m_size.x / w) * m_scale.x, (m_size.y / h) * m_scale.y };
		(*m_sprite)[i]->SetScale({ (m_size.x / w) * m_scale.x, (m_size.y / h) * m_scale.y });
		if (_centerPos)
		{
			(*m_sprite)[i]->SetPosition({ m_position.x - (m_size.x * m_scale.x * 0.5f), m_position.y - (m_size.y * m_scale.y * 0.5f) });
		}
	}
}

void Truth::UI::SetSampling(const Vector2& _min, const Vector2& _max)
{
	m_minSampling = _min;
	m_maxSampling = _max;

	for (uint32 i = 0; i < 3; i++)
	{
		auto tex = m_sprite->GetTex(i);
		float w = static_cast<float>(tex->w);
		float h = static_cast<float>(tex->h);

		(*m_sprite)[i]->SetSampleRect(
			{
				static_cast<uint32>(w * m_minSampling.x),
				static_cast<uint32>(h * m_minSampling.y),
				static_cast<uint32>(w * m_maxSampling.x),
				static_cast<uint32>(h * m_maxSampling.y)
			});
	}
}

void Truth::UI::SetOnlyUI()
{
	m_isButton = false;
}

void Truth::UI::SetButton()
{
	m_isButton = true;
}

void Truth::UI::SetAlpha(float _alpha)
{
	m_alpha = _alpha;
	for (uint32 i = 0; i < 3; i++)
	{
		(*m_sprite)[i]->SetAlpha(_alpha);
	}
}

void Truth::UI::Initialize()
{
	auto gp = m_managers.lock()->Graphics();
	m_sprite = gp->CreateUISpriteSet();

	if (m_behavior)
		m_behavior->Initialize(m_managers, ::Cast<UI, Component>(shared_from_this()), m_owner);

	for (uint32 i = 0; i < 3; i++)
	{
		if (!m_texturePath[i].empty())
		{
			(*m_sprite)[i] = gp->CreateSprite();
			auto tex = gp->CreateTexture(fs::path(m_texturePath[i]), false, false, true);
			m_sprite->GetTex(i) = tex;

			float w = static_cast<float>(tex->w);
			float h = static_cast<float>(tex->h);

			Vector2 gpScale = { (m_size.x / w) * m_scale.x, (m_size.y / h) * m_scale.y };
			Vector2 gpPosition = { (m_size.x / w) * m_scale.x, (m_size.y / h) * m_scale.y };

			(*m_sprite)[i]->SetTexture(tex->m_texture);
			(*m_sprite)[i]->SetScale(gpScale);
			(*m_sprite)[i]->SetPosition(gpPosition);
			(*m_sprite)[i]->SetActive(IsActive());
			(*m_sprite)[i]->SetAlpha(m_alpha);
			(*m_sprite)[i]->SetZ(m_zDepth);
			(*m_sprite)[i]->SetSampleRect(
				{
					static_cast<uint32>(w * m_minSampling.x),
					static_cast<uint32>(h * m_minSampling.y),
					static_cast<uint32>(w * m_maxSampling.x),
					static_cast<uint32>(h * m_maxSampling.y)
				});
		}
	}
	for (uint32 i = 0; i < 3; i++)
	{
		auto tex = m_sprite->GetTex(i);
		float w = static_cast<float>(tex->w);
		float h = static_cast<float>(tex->h);
		(*m_sprite)[i]->SetScale({ (m_size.x / w) * m_scale.x, (m_size.y / h) * m_scale.y });
		(*m_sprite)[i]->SetPosition({ m_position.x - (m_size.x * m_scale.x * 0.5f), m_position.y - (m_size.y * m_scale.y * 0.5f) });
		(*m_sprite)[i]->SetActive(IsActive());
		(*m_sprite)[i]->SetAlpha(m_alpha);
		(*m_sprite)[i]->SetZ(m_zDepth);
		(*m_sprite)[i]->SetSampleRect(
			{
				static_cast<uint32>(w * m_minSampling.x),
				static_cast<uint32>(h * m_minSampling.y),
				static_cast<uint32>(w * m_maxSampling.x),
				static_cast<uint32>(h * m_maxSampling.y)
			});

		m_texturePath[i] = tex->m_path.generic_string();
	}
	m_rect.left = static_cast<LONG>(m_position.x - (m_size.x * 0.5f) * m_scale.x);
	m_rect.top = static_cast<LONG>(m_position.y - (m_size.y * 0.5f) * m_scale.y);
	m_rect.right = static_cast<LONG>(m_position.x + (m_size.x * 0.5f) * m_scale.x);
	m_rect.bottom = static_cast<LONG>(m_position.y + (m_size.y * 0.5f) * m_scale.y);

	SetSpriteActive(BUTTON_STATE::IDEL);


}

void Truth::UI::Start()
{
	ResizeWindow();
	if (m_behavior)
		m_behavior->Start();
	for (uint32 i = 0; i < 3; i++)
	{
		auto tex = m_sprite->GetTex(i);
		float w = static_cast<float>(tex->w);
		float h = static_cast<float>(tex->h);
		(*m_sprite)[i]->SetScale({ (m_size.x / w) * m_scale.x, (m_size.y / h) * m_scale.y });
		(*m_sprite)[i]->SetPosition({ m_position.x - (m_size.x * m_scale.x * 0.5f), m_position.y - (m_size.y * m_scale.y * 0.5f) });
		(*m_sprite)[i]->SetActive(IsActive());
		(*m_sprite)[i]->SetAlpha(m_alpha);
		(*m_sprite)[i]->SetZ(m_zDepth);
		(*m_sprite)[i]->SetSampleRect(
			{
				static_cast<uint32>(w * m_minSampling.x),
				static_cast<uint32>(h * m_minSampling.y),
				static_cast<uint32>(w * m_maxSampling.x),
				static_cast<uint32>(h * m_maxSampling.y)
			});
		m_texturePath[i] = tex->m_path.generic_string();
	}
	SetSpriteActive(BUTTON_STATE::IDEL);

}

void Truth::UI::Update()
{
#ifdef EDITOR_MODE
	ResizeWindow();
#endif
	if (m_behavior)
		m_behavior->Update();

	if (m_noneUpdate)
		return;

	if (!m_isButton)
	{
		SetSpriteActive(BUTTON_STATE::IDEL);
		return;
	}

	CheckState();

	switch (m_state)
	{
	case BUTTON_STATE::IDEL:
	{
		SetSpriteActive(BUTTON_STATE::IDEL);
		break;
	}
	case BUTTON_STATE::OVER:
	{
		SetSpriteActive(BUTTON_STATE::OVER);
		if (m_behavior != nullptr)
			m_behavior->OnMouseOver();
		break;
	}
	case BUTTON_STATE::DOWN:
	{
		SetSpriteActive(BUTTON_STATE::DOWN);
		if (m_behavior != nullptr)
			m_behavior->OnMouseClick();
		break;
	}
	case BUTTON_STATE::UP:
	{
		SetSpriteActive(BUTTON_STATE::IDEL);
		if (m_behavior != nullptr)
			m_behavior->OnMouseUp();
		break;
	}
	case BUTTON_STATE::HOLD:
	{
		SetSpriteActive(BUTTON_STATE::DOWN);
		break;
	}
	default:
		break;
	}
}

void Truth::UI::CheckState()
{
	int mouseX = m_managers.lock()->Input()->m_nowMousePosX;
	int mouseY = m_managers.lock()->Input()->m_nowMousePosY;

	if (IsActive())
	{
		if (((mouseX >= m_rect.left) && (mouseX <= m_rect.right)) &&
			((mouseY >= m_rect.top) && (mouseY <= m_rect.bottom)))
		{
			if (GetKey(MOUSE::LMOUSE))
			{
				if (m_prevState == BUTTON_STATE::DOWN || m_prevState == BUTTON_STATE::HOLD)
					m_state = BUTTON_STATE::HOLD;
				else
					m_state = BUTTON_STATE::DOWN;
			}
			else
			{
				if (m_prevState == BUTTON_STATE::IDEL || m_prevState == BUTTON_STATE::OVER)
					m_state = BUTTON_STATE::OVER;
				else if (m_prevState == BUTTON_STATE::UP)
					m_state = BUTTON_STATE::IDEL;
				else if (m_prevState == BUTTON_STATE::DOWN || m_prevState == BUTTON_STATE::HOLD)
					m_state = BUTTON_STATE::UP;
			}
		}
		else
			m_state = BUTTON_STATE::IDEL;

		m_prevState = m_state;
	}
}

bool Truth::UI::IsActive()
{
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

void Truth::UI::SetSpriteActive(BUTTON_STATE _state)
{
	uint32 state = static_cast<uint32>(_state);
	for (uint32 i = 0; i < 3; i++)
	{
		if (state == i)
			(*m_sprite)[i]->SetActive(true);
		else
			(*m_sprite)[i]->SetActive(false);
	}
}

void Truth::UI::ResizeWindow()
{
	auto gp = m_managers.lock()->Graphics();

	// RECT winRect = gp->GetWindowRect();
	Vector2 realLT = gp->GetContentPosMin();
	Vector2 realRB = gp->GetContentPosMax();
	Vector2 resolution = gp->GetDisplayResolution();

	float contentW = realRB.x - realLT.x;
	float contentH = realRB.y - realLT.y;

	float winW = resolution.x;
	float winH = resolution.y;

	float ratioW = contentW / winW;
	float ratioH = contentH / winH;

	Vector2 editorSize = {};
	editorSize.x = m_size.x * ratioW;
	editorSize.y = m_size.y * ratioH;

	Vector2 editorPos = {};
	editorPos.x = realLT.x + (m_position.x * ratioW);
	editorPos.y = realLT.y + (m_position.y * ratioH);
	// m_position = { m_position.x * ratioW, m_position.y * ratioH };

	m_rect.left = static_cast<LONG>(editorPos.x - (editorSize.x * 0.5f) * m_scale.x);
	m_rect.top = static_cast<LONG>(editorPos.y - (editorSize.y * 0.5f) * m_scale.y);
	m_rect.right = static_cast<LONG>(editorPos.x + (editorSize.x * 0.5f) * m_scale.x);
	m_rect.bottom = static_cast<LONG>(editorPos.y + (editorSize.y * 0.5f) * m_scale.y);
}

#ifdef EDITOR_MODE
void Truth::UI::EditorSetValue()
{
	ResizeWindow();

	for (uint32 i = 0; i < 3; i++)
	{
		auto tex = m_sprite->GetTex(i);
		float w = static_cast<float>(tex->w);
		float h = static_cast<float>(tex->h);
		(*m_sprite)[i]->SetScale({ (m_size.x / w) * m_scale.x, (m_size.y / h) * m_scale.y });
		(*m_sprite)[i]->SetPosition({ m_position.x - (m_size.x * m_scale.x * 0.5f), m_position.y - (m_size.y * m_scale.y * 0.5f) });
		(*m_sprite)[i]->SetActive(IsActive());
		(*m_sprite)[i]->SetAlpha(m_alpha);
		(*m_sprite)[i]->SetZ(m_zDepth);
		(*m_sprite)[i]->SetSampleRect(
			{
				static_cast<uint32>(w * m_minSampling.x),
				static_cast<uint32>(h * m_minSampling.y),
				static_cast<uint32>(w * m_maxSampling.x),
				static_cast<uint32>(h * m_maxSampling.y)
			});

		m_texturePath[i] = tex->m_path.generic_string();
	}
	SetSpriteActive(BUTTON_STATE::IDEL);

	if (m_behavior)
		m_behavior->Initialize(m_managers, ::Cast<UI, Component>(shared_from_this()), m_owner);
}
#endif // EDITOR_MODE
