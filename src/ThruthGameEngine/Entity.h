#pragma once
#include "Headers.h"
#include "Component.h"

/// <summary>
/// 모든 엔티티의 부모 클래스
/// 일단은 이렇게 구현해보자...
/// </summary>
namespace Truth
{
	class Component;
	class Transform;
	class Managers;
	class Collider;
}

namespace Ideal
{
	class IBone;
}

namespace Truth
{
	class Entity final
		: public std::enable_shared_from_this<Entity>
	{
		GENERATE_CLASS_TYPE_INFO(Entity);

	private:
		friend class boost::serialization::access;

		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

		typedef std::vector<std::pair<std::weak_ptr<Component>, const Method*>> ComponentMethod;

	protected:
		static uint32 m_entityCount;

		std::weak_ptr<Managers> m_manager;

	public:
		int32 m_index;

		PROPERTY(tag);
		std::string m_tag;

		PROPERTY(ID);
		uint32 m_ID;

		PROPERTY(layer);
		uint8 m_layer;

		PROPERTY(name);
		std::string m_name;

		PROPERTY(components);
		std::vector<std::shared_ptr<Component>> m_components;

		std::vector<std::shared_ptr<Entity>> m_children;

		std::weak_ptr<Entity> m_parent;
		std::weak_ptr<Ideal::IBone> m_linkedBone;

		std::string m_linkBoneName;

		bool m_isDead = false;

		PROPERTY(isActive);
		bool m_isActive = true;
		
		// bool m_parnetAcrive = true;

		bool m_isStatic = false;

		bool m_isAdded = true;

		ComponentMethod m_onCollisionEnter;
		ComponentMethod m_onCollisionStay;
		ComponentMethod m_onCollisionExit;

		ComponentMethod m_onTriggerEnter;
		ComponentMethod m_onTriggerStay;
		ComponentMethod m_onTriggerExit;

		ComponentMethod m_update;
		ComponentMethod m_fixedUpdate;
		ComponentMethod m_latedUpdate;

		ComponentMethod m_onBecomeVisible;
		ComponentMethod m_onBecomeInvisible;

		ComponentMethod m_destroy;
		ComponentMethod m_applyTransform;

		ComponentMethod m_resizeWindow;
		ComponentMethod m_setActive;

		std::shared_ptr<Transform> m_transform;

	public:
		Entity(std::shared_ptr<Managers> _mangers);
		Entity();
		~Entity();

		void Initialize();

		void SetPosition(const Vector3& _pos) const;
		void SetScale(const Vector3& _scale) const;
		void SetWorldRotation(const Quaternion& _rotation) const;

		const Vector3& GetLocalPosition() const;
		const Quaternion& GetLocalRotation() const;
		const Vector3& GetLocalScale() const;

		const Vector3& GetWorldPosition() const;
		const Quaternion& GetWorldRotation() const;
		const Vector3& GetWorldScale() const;

		const Matrix& GetWorldTM() const;

		void ApplyTransform();

		void SetWorldTM(const Matrix& _tm) const;
		void SetLocalTM(const Matrix& _tm) const;

		bool HasParent() const { return !m_parent.expired(); };
		bool HasChildren() const { return !m_children.empty(); }
		bool IsLinked() const { return !m_linkedBone.expired(); };

		const Matrix& GetParentMatrix() const
		{
			if (!m_parent.expired())
			{
				return m_parent.lock()->GetWorldTM();
			}
			else
			{
				return Matrix::Identity;
			}
		}

		void Awake();
		void Destroy();
		void Start();

		void Update();
		void FixedUpdate();
		void LateUpdate();

		void ResizeWindow();

		void SetActive(bool _active);

		void OnCollisionEnter(Collider* _other);
		void OnCollisionStay(Collider* _other);
		void OnCollisionExit(Collider* _other);

		void OnTriggerEnter(Collider* _other);
		void OnTriggerStay(Collider* _other);
		void OnTriggerExit(Collider* _other);

		void DeleteComponent(int32 _index);

		template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
		std::shared_ptr<C> AddComponent();
		template<typename C, typename... Args, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
		std::shared_ptr<C> AddComponent(Args... _args);

		void AddComponent(std::shared_ptr<Component> _component);

		template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
		std::weak_ptr<C> GetComponent();

		template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
		std::vector<std::weak_ptr<C>> GetComponents();

		void SetManager(std::weak_ptr<Managers> _val) { m_manager = _val; };

		std::string& GetName() { return m_name; };

		void AddChild(std::shared_ptr<Entity> _entity);
		void DeleteChild(std::shared_ptr<Entity> _entity);

		void LinkBone(const std::string& _boneName);

	private:
		void ApplyComponent(std::shared_ptr<Component> _c);

		void IterateComponentMethod(ComponentMethod& _cm);
		void IterateComponentMethod(ComponentMethod& _cm, Collider* _param);
	};

	/// template로 작성된 함수 목록
	template<class Archive>
	void Entity::save(Archive& _ar, const unsigned int _file_version) const
	{
		_ar& m_name;
		_ar& m_layer;
		_ar& m_components;
		_ar& m_children;
		_ar& m_linkBoneName;
		_ar& m_isActive;
	}

	template<class Archive>
	void Entity::load(Archive& _ar, const unsigned int _file_version)
	{
		_ar& m_name;
		if (_file_version == 0)
		{
			_ar& m_ID;
		}
		_ar& m_layer;
		_ar& m_components;
		if (_file_version >= 2)
			_ar& m_children;
		if (_file_version >= 3)
			_ar& m_linkBoneName;
		if (_file_version >= 4)
			_ar& m_isActive;

	}

	/// <summary>
	/// 엔티티에 컴포넌트 추가
	/// </summary>
	/// <typeparam name="C">컴포넌트 타입</typeparam>
	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
	std::shared_ptr<C> Entity::AddComponent()
	{
		if (GetComponent<C>().expired() || GetComponent<C>().lock()->CanMultiple())
		{
			std::shared_ptr<C> component = nullptr;
			component = std::make_shared<C>();
			component->SetManager(m_manager);
			component->SetOwner(shared_from_this());
			component->m_index = static_cast<int32>(m_components.size());
			m_components.push_back(component);
			// component->Initalize();
			ApplyComponent(component);

			return component;
		}
		else
		{
			return nullptr;
		}
	}

	template<typename C, typename... Args, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
	std::shared_ptr<C> Entity::AddComponent(Args... _args)
	{
		if (GetComponent<C>().expired() || GetComponent<C>().lock()->CanMultiple())
		{
			std::shared_ptr<C> component = nullptr;
			component = std::make_shared<C>(_args...);
			component->SetManager(m_manager);
			component->SetOwner(shared_from_this());
			component->m_index = static_cast<int32>(m_components.size());
			m_components.push_back(component);
			// component->Initalize();
			ApplyComponent(component);

			return component;
		}
		else
		{
			return nullptr;
		}
	}

	/// <summary>
	/// 컴포넌트 가져오기
	/// 여려개가 있는 컴포넌트의 경우 가장 바깥에 있는 하나만 반환 
	/// </summary>
	/// <typeparam name="C">컴포넌트 타입</typeparam>
	/// <returns>컴포넌트</returns>
	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
	std::weak_ptr<C> Entity::GetComponent()
	{
		for (auto& c : m_components)
		{
			auto component = ::Cast<C, Component>(c);
			if (component != nullptr)
			{
				return component;
			}
		}

		return std::weak_ptr<C>();
	}

	/// <summary>
	/// 컴포넌트들 가져오기
	/// 한개인 경우 크기가 하나인 리스트로 돌려준다.
	/// </summary>
	/// <typeparam name="C">컴포넌트 타입</typeparam>
	/// <returns>컴포넌트 리스트</returns>
	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
	std::vector<std::weak_ptr<C>> Entity::GetComponents()
	{
		std::vector<std::weak_ptr<C>> result;
		for (auto& c : m_components)
		{
			auto component = ::Cast<C, Component>(c);
			if (component != nullptr)
			{
				result.push_back(component);
			}
		}
		return result;
	}
}
BOOST_CLASS_VERSION(Truth::Entity, 4)