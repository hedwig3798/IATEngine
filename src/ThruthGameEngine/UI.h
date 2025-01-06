#pragma once
#include "Component.h"
#include "UISpriteSet.h"

namespace Ideal
{
	class ISprite;
	class IText;
}

namespace Truth
{
	class ButtonBehavior;
	struct UISpriteSet;
}

namespace Truth
{
	class UI :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(UI);

	private:

		enum class BUTTON_STATE
		{
			IDEL,
			OVER,
			DOWN,
			HOLD,
			UP,
			END,
		};

	public:
		PROPERTY(sprite);
		std::shared_ptr<UISpriteSet> m_sprite;

		std::string m_texturePath[3];

		BUTTON_STATE m_prevState;
		BUTTON_STATE m_state;

		RECT m_rect;

		PROPERTY(isButton);
		bool m_isButton;

		PROPERTY(size);
		Vector2 m_size;

		PROPERTY(minSampling);
		Vector2 m_minSampling;

		PROPERTY(maxSampling);
		Vector2 m_maxSampling;

		PROPERTY(scale);
		Vector2 m_scale;

		PROPERTY(position);
		Vector2 m_position;

		PROPERTYM(alpha, 0.0f, 1.f);
		float m_alpha;

		PROPERTYM(zDepth, 0.0f, 1.0f);
		float m_zDepth;

		PROPERTY(behavior);
		std::shared_ptr<ButtonBehavior> m_behavior;


		bool m_noneUpdate = false;

	public:
		UI();
		virtual ~UI();

		void SetScale(const Vector2& _scale, bool _centerPos = false);
		void SetSampling(const Vector2& _min, const Vector2& _max);
		void SetOnlyUI();
		void SetButton();
		void SetAlpha(float _alpha);
		const std::string& GetName() { return m_owner.lock()->m_name; };
	private:
		METHOD(Initialize);
		virtual void Initialize() override;

		METHOD(Start);
		virtual void Start() override;

		METHOD(Update);
		virtual void Update() override;

		void CheckState();

		bool IsActive();

		void SetSpriteActive(BUTTON_STATE _state);

		METHOD(ResizeWindow);
		void ResizeWindow() override;

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE
	};
}

