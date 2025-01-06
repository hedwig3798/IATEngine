#pragma once
#include <map>
#include <string>
#include <vector>
#include <typeinfo>
#include <memory>
#include "ComponentFactory.h"
#include "ButtonFactory.h"

class Method;
class Property;
class TypeInfo;

namespace Truth
{
	class Component;
}

template<typename T>
struct has_const_iterator
{
private:
	typedef char                      yes;
	typedef struct { char array[2]; } no;

	template<typename C> static yes test(typename C::const_iterator*);
	template<typename C> static no  test(...);
public:
	static const bool value = sizeof(test<T>(0)) == sizeof(yes);
	typedef T type;
};

template <typename T>
struct has_begin_end
{
	template<typename C> static char(&f(typename std::enable_if<
		std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::begin)),
		typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

	template<typename C> static char(&f(...))[2];

	template<typename C> static char(&g(typename std::enable_if<
		std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::end)),
		typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

	template<typename C> static char(&g(...))[2];

	static bool const beg_value = sizeof(f<T>(0)) == 1;
	static bool const end_value = sizeof(g<T>(0)) == 1;
};

template<typename T>
struct is_container 
	: std::integral_constant<
	bool,
	has_const_iterator<T>::value&& 
	has_begin_end<T>::beg_value&& 
	has_begin_end<T>::end_value>
{ 
};

/// <summary>
/// Super Ÿ���� �ִ���
/// </summary>
template<typename, typename = void>
struct HasSuperType
	: std::false_type
{
};
template<typename T>
struct HasSuperType<T, std::void_t<typename T::Super>>
	: std::true_type
{
};

/// <summary>
/// StaticTypeInfo �Լ��� �ִ���
/// </summary>
template<typename, typename = void>
struct HasStaticTypeInfoFunc
	: std::false_type
{
};
template<typename T>
struct HasStaticTypeInfoFunc<T, std::void_t<decltype(std::declval<T>().StaticTypeInfo())>>
	: std::true_type
{
};

/// <summary>
/// ThisType�� Ÿ���� �ִ���
/// </summary>
template <typename, typename = void>
struct SuperClassTypeDeduction
{
	using Type = void;
};
template <typename T>
struct SuperClassTypeDeduction<T, std::void_t<typename T::ThisType>>
{
	using Type = typename T::ThisType;
};

/// <summary>
/// Ÿ�� ���� �ʱ�ȭ ����ü
/// </summary>
/// <typeparam name="T">Ÿ��</typeparam>
template <typename T>
struct TypeInfoInitializer
{
	// �̸�
	const char* m_name = nullptr;
	// �θ� Ŭ����
	const TypeInfo* m_super = nullptr;

	// �̸��� �����ڿ��� �ʱ�ȭ
	TypeInfoInitializer(const char* _name)
		: m_name(_name)
	{
		// ���� super�� �ִٸ�
		if constexpr (HasSuperType<T>::value)
		{
			// ���� super�� void�� �ƴ϶��
			// �� �θ� Ŭ������ �ִٸ�
			using Type = typename T::Super;
			if constexpr (!std::is_void_v<Type>)
			{
				// �θ� Ŭ�����������´�.
				m_super = &(Type::StaticTypeInfo());
			}
		}
	}
};

/// <summary>
/// Ÿ�� ����
/// </summary>
class TypeInfo
{
private:
	// �Լ� ����
	friend Method;
	// ���� ����
	friend Property;
	// string�� ���� ������ �����ϱ� ����
	using MethodMap = std::map<std::string_view, const Method*>;
	using PropertyMap = std::map<std::string_view, const Property*>;


	// Ÿ�� �̸� �ؽð�
	size_t m_typeHash;
	// �̸�
	const char* m_name = nullptr;
	// Ÿ���� ��ü �̸� (typeid.name())
	std::string m_fullName;
	// Ÿ���� ����
	std::string m_alias;
	// �θ� Ŭ������ ���� (�ִٸ�)
	const TypeInfo* m_super = nullptr;

	// �ش� Ÿ���� �迭����
	bool m_isArray = false;

	// �Լ� �����̳� 
	std::vector<const Method*> m_methods;
	MethodMap m_methodMap;

	// ���� �����̳�
	std::vector<const Property*> m_properties;
	PropertyMap m_propertyMap;

public:
	static std::unique_ptr<Truth::ComponentFactory> g_componentFactory;
	static std::unique_ptr<Truth::ButtonFactory> g_buttonFactory;

public:
	std::string Dump(void* _object, int _indent = 0) const;

	// ������ 
	// Ÿ�� ���� �̴ϼȶ��������� �ʿ��� ������ �޾ƿ´�.
	// ������ �ð� �ȿ� �Ͼ�� ����
	template <typename T>
	explicit TypeInfo(const TypeInfoInitializer<T>& initializer)
		: m_typeHash(typeid(T).hash_code())
		, m_name(initializer.m_name)
		, m_fullName(typeid(T).name())
		, m_super(initializer.m_super)
		, m_isArray(std::is_array_v<T>)
	{
		// ���� �θ� Ÿ���� �����ϴ� ���
		// ��ũ�� �Լ��� ���ǵ� �ִ� ���
		if constexpr (HasSuperType<T>::value)
		{
			using Type = typename T::Super;
			// �ش� �θ� Ÿ���� void�� �ƴ� ���
			if constexpr (!std::is_void_v<Type>)
			{
				CollectSuperMethods();
				CollectSuperProperties();
			}
		}

		if constexpr (std::is_base_of_v<Truth::Component, T> && !std::is_abstract_v<T>)
		{
			if (g_componentFactory == nullptr)
				g_componentFactory = std::make_unique<Truth::ComponentFactory>();

			g_componentFactory->RegisterType(m_name, []() { return std::make_shared<T>(); });
		}

		if constexpr (std::is_base_of_v<Truth::ButtonBehavior, T> && !std::is_abstract_v<T>)
		{
			if (g_buttonFactory == nullptr)
				g_buttonFactory = std::make_unique<Truth::ButtonFactory>();

			g_buttonFactory->RegisterType(m_name, []() { return std::make_shared<T>(); });
		}
	}

	static const std::shared_ptr<Truth::Component> CreateComponent(std::string& _name)
	{
		return g_componentFactory->Create(_name);
	}

	// �θ��� Ÿ�� ���� ��������
	const TypeInfo* GetSuper() const;

	// ���� ������ Ÿ�� ������ �ִٸ� �����´�.
	template <typename T, std::enable_if_t<HasStaticTypeInfoFunc<T>::value>* = nullptr>
	static const TypeInfo& GetStaticTypeInfo()
	{
		return T::StaticTypeInfo();
	}

	// �ش� Ÿ���� �����Ͷ�� �����͸� �����ϰ� �����´�.
	template
		<
		typename T,
		std::enable_if_t<std::is_pointer_v<T>>* = nullptr,
		std::enable_if_t<HasStaticTypeInfoFunc<std::remove_pointer_t<T>>::value>* = nullptr
		>
		static const TypeInfo& GetStaticTypeInfo()
	{
		return std::remove_pointer_t<T>::StaticTypeInfo();
	}

	// ���ٸ� ���÷��� ���� Ÿ���� �ƴѰ�
	template
		<
		typename T,
		std::enable_if_t<!HasStaticTypeInfoFunc<T>::value>* = nullptr,
		std::enable_if_t<!HasStaticTypeInfoFunc<std::remove_pointer_t<T>>::value>* = nullptr
		>
		static const TypeInfo& GetStaticTypeInfo()
	{
// 		if constexpr (is_container<T>::value)
// 		{
// 			std::string origin = typeid(T).name();
// 			std::string result = "";
// 		}
// 		else if constexpr (std::is_class_v<T>)
// 		{
// 			std::string origin = typeid(T).name();
// 			std::string result = "";
// 			if (origin[0] == 'c')
// 			{
// 				result += origin.substr(6);
// 			}
// 			else if (origin[0] == 's')
// 			{
// 				result += origin.substr(7);
// 			}
// // 			static TypeInfo typeInfo{ TypeInfoInitializer<T>(result.c_str()) };
// // 			return typeInfo;
// 		}

		static TypeInfo typeInfo{ TypeInfoInitializer<T>("unreflected_type_variable") };
		return typeInfo;
	}

	// ohter�� ���� Ÿ������
	bool IsA(const TypeInfo& other) const;
	// other�� �ڽ�����
	bool IsChildOf(const TypeInfo& other) const;

	template <typename T>
	bool IsA() const
	{
		return IsA(GetStaticTypeInfo<T>());
	}

	template <typename T>
	bool IsChildOf() const
	{
		return IsChildOf(GetStaticTypeInfo<T>());
	}

	const std::vector<const Method*>& GetMethods() const
	{
		return m_methods;
	}

	const Method* GetMethod(const char* name) const;


	const std::vector<const Property*>& GetProperties() const
	{
		return m_properties;
	}

	const Property* GetProperty(const char* name) const;

	const char* GetName() const { return m_name; }
	std::string GetFullName() const { return m_fullName; }

	bool IsArray() const { return m_isArray; }

private:
	void CollectSuperMethods();
	void CollectSuperProperties();

	void AddProperty(const Property* property);
	void AddMethod(const Method* method);
};
