#pragma once
#include "Component.h"
#include "UISpriteSet.h"

namespace Ideal
{
	class IText;
}

namespace Truth
{
	class ButtonBehavior;
}

/// <summary>
/// 텍스트 렌더를 위한 UI 컴포넌트
/// </summary>
namespace Truth
{
	class TextUI :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(TextUI);

	private:
		std::weak_ptr<Ideal::IText> m_textSprite;

		PROPERTY(spriteSize);
		Vector2 m_spriteSize;

		PROPERTY(textSize);
		Vector2 m_textSize;

		Vector2 m_finalSize;

		PROPERTY(fontSize);
		float m_fontSize;

		PROPERTY(position);
		Vector2 m_position;

		PROPERTYM(alpha, 0.0f, 1.0f);
		float m_alpha;

		PROPERTYM(zDepth, 0.0f, 0.9f);
		float m_zDepth;

		PROPERTY(behavior);
		std::shared_ptr<ButtonBehavior> m_behavior;

		PROPERTY(text);
		std::wstring m_text;

	public:
		TextUI();
		virtual ~TextUI();

		void ChangeText(const std::wstring& _text);
		void SetAlpha(float _alpha);

	private:
		METHOD(Initialize);
		virtual void Initialize() override;

		METHOD(Start);
		virtual void Start() override;

		METHOD(Awake);
		virtual void Awake() override;

		METHOD(Update);
		virtual void Update() override;

		bool IsActive();

		void SetSpriteActive(bool _active);

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE
	};
}

