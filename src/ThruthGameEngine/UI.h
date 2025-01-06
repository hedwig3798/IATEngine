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
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();

		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

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


	template<class Archive>
	void Truth::UI::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		for (int i = 0; i < 3; i++)
			_ar& m_texturePath[i];

		_ar& m_size;
		_ar& m_position;
		_ar& m_alpha;
		_ar& m_zDepth;
		_ar& m_behavior;
		_ar& m_scale;
		_ar& m_minSampling;
		_ar& m_maxSampling;
		_ar& m_isButton;
	}

	template<class Archive>
	void Truth::UI::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		for (int i = 0; i < 3; i++)
			_ar& m_texturePath[i];

		_ar& m_size;
		_ar& m_position;
		_ar& m_alpha;
		_ar& m_zDepth;

		if (file_version >= 1)
			_ar& m_behavior;

		if (file_version >= 2)
			_ar& m_scale;

		if (file_version >= 3)
		{
			_ar& m_minSampling;
			_ar& m_maxSampling;
		}
		if (file_version >= 4)
			_ar& m_isButton;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::UI)
BOOST_CLASS_VERSION(Truth::UI, 4)

