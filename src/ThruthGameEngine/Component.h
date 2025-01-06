#pragma once
#include "Headers.h"
#include "EventHandler.h"
#include "Managers.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "Scene.h"

/// <summary>
/// 모든 컴포넌트의 부모 클래스
/// 일단은 이렇게 구현해보자...
/// </summary>

enum class KEY;

namespace Truth
{
	class Entity;
	class Transform;
	class Managers;
	class Collider;
}



namespace Truth
{
	class Component abstract
		: public EventHandler, public std::enable_shared_from_this<Component>
	{
		GENERATE_CLASS_TYPE_INFO(Component);

	public:
		std::string m_name;
		int32 m_index;
		const uint32 m_ID;

	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& _ar, const unsigned int _file_version) const;
		template<class Archive>
		void load(Archive& _ar, const unsigned int _file_version);

		static uint32 m_IDGenerater;

	protected:
		bool m_canMultiple;

		std::weak_ptr<Entity> m_owner;
		std::weak_ptr<Transform> m_transform;
		std::weak_ptr<Managers> m_managers;

	public:
		Component();
		virtual ~Component();

		bool CanMultiple() const { return m_canMultiple; }

		virtual void Update() {};
		virtual void Awake() {};
		virtual void Start() {};
		virtual void Destroy() {};
		virtual void LateUpdate() {};
		virtual void FixedUpdate() {};
		virtual void OnCollisionEnter(Truth::Collider* _other) {};
		virtual void OnCollisionExit(Truth::Collider* _other) {};
		virtual void OnCollisionStay(Truth::Collider* _other) {};
		virtual void OnTriggerEnter(Truth::Collider* _other) {};
		virtual void OnTriggerExit(Truth::Collider* _other) {};
		virtual void OnTriggerStay(Truth::Collider* _other) {};
		virtual void Initialize() {};
		virtual void ApplyTransform() {};
		virtual void ResizeWindow() {};
		virtual void SetActive() {};
#ifdef EDITOR_MODE
		virtual void EditorSetValue() {};
#endif // EDITOR_MODE


		void SetOwner(std::weak_ptr<Entity> _val) { m_owner = _val; }
		std::weak_ptr<Entity> GetOwner() const { return m_owner; }
		void SetManager(std::weak_ptr<Managers> _val) { m_managers = _val; }

#pragma region inline
	protected:
		// 자주 사용될 기능을 미리 묶어 놓는다.
		// Component 에서 자주 사용될 함수 목록
		// 이벤트 관련 함수
		template <typename T>
		inline void EventBind(std::string _name, void (T::* func)(const void*))
		{
			m_managers.lock()->Event()->Subscribe(_name, MakeListenerInfo(func));
		}
		inline void EventPublish(std::string _name, const void* _param, float4 _delay = 0.0f)
		{
			m_managers.lock()->Event()->PublishEvent(_name, _param, _delay);
		}

		// 입력 관련 함수
		inline bool GetKeyDown(KEY _key) const
		{
			return (m_managers.lock()->Input()->GetKeyState(_key) == KEY_STATE::DOWN);
		}
		inline bool GetKeyUp(KEY _key) const
		{
			return (m_managers.lock()->Input()->GetKeyState(_key) == KEY_STATE::UP);
		}
		inline bool GetKey(KEY _key) const
		{
			return (m_managers.lock()->Input()->GetKeyState(_key) == KEY_STATE::HOLD);
		}

		// 입력 관련 함수
		inline bool GetKeyDown(MOUSE _key) const
		{
			return (m_managers.lock()->Input()->GetKeyState(_key) == KEY_STATE::DOWN);
		}
		inline bool GetKeyUp(MOUSE _key) const
		{
			return (m_managers.lock()->Input()->GetKeyState(_key) == KEY_STATE::UP);
		}
		inline bool GetKey(MOUSE _key) const
		{
			return (m_managers.lock()->Input()->GetKeyState(_key) == KEY_STATE::HOLD);
		}

		inline int16 MouseDx()
		{
			return m_managers.lock()->Input()->GetMouseMoveX();
		}
		inline int16 MouseDy()
		{
			return m_managers.lock()->Input()->GetMouseMoveY();
		}
		// 시간 관련 함수들
		inline float GetDeltaTime()
		{
			return m_managers.lock()->Time()->GetDT();
		}

		inline float GetFixedDeltaTime()
		{
			return m_managers.lock()->Time()->GetFDT();
		}

		inline void WaitForSecondsRealtime(float time)
		{
			m_managers.lock()->Time()->WaitForSecondsRealtime(time);
		}

		inline void SetTimeScaleForSeconds(float scale, float time)
		{
			m_managers.lock()->Time()->SetTimeScaleForSeconds(scale, time);
		}
	public:
		void Translate(Vector3& _val);

		void SetRotation(const Quaternion& _val);
		void SetWorldRotation(const Quaternion& _val);
		void SetPosition(const Vector3& _pos) const;
		void SetScale(const Vector3& _scale) const;

		const Vector3& GetPosition() const;
		const Quaternion& GetRotation() const;
		const Vector3& GetScale() const;

		const Matrix& GetWorldTM() const;
		const Matrix& GetParentWorldTM() const;
		const Matrix& GetLocalTM() const;
		void SetLocalTM(const Matrix& _tm);

		void AddEmptyEntity();
		void AddEntity(std::shared_ptr<Entity> _entity);

		bool HasParent();
		bool IsLinked();
		const Matrix& GetParentMatrix();
		const Matrix& GetParentLinkedMatrix();

#pragma endregion inline
	};
}
template<class Archive>
void Truth::Component::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& m_name;
	_ar& m_canMultiple;
}

template<class Archive>
void Truth::Component::load(Archive& _ar, const unsigned int file_version)
{
	_ar& m_name;
	_ar& m_canMultiple;
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Truth::Component)
BOOST_CLASS_EXPORT_KEY(Truth::Component)
BOOST_CLASS_VERSION(Truth::Component, 0)