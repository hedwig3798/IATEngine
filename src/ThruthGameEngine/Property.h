#pragma once
#include "TypeInfoMacros.h"
#include "Types.h"
#include <cassert>
#include <string>
#include "StringConverter.h"
#include "TypeUI.h"

template<typename T> struct is_array_custom
{
	static constexpr bool value = false;
	static constexpr size_t size = -1;
};

template<typename T, size_t N>
struct is_array_custom<T[N]>
{
	static constexpr bool value = true;
	static constexpr size_t size = N;
};

/// <summary>
/// ���� ���� �ڵ鷯 ���̽�
/// </summary>
class PropertyHandlerBase
{
	GENERATE_CLASS_TYPE_INFO(PropertyHandlerBase)

public:
	virtual ~PropertyHandlerBase() = default;

	virtual std::string Dump(void* _object, int _indent = 0) const abstract;
	virtual bool DisplayUI(void* _object, const char* _name, float _min, float _max) const abstract;
};

/// <summary>
/// ���� ���� �ڵ鷯 �������̽�
/// </summary>
/// <typeparam name="T"></typeparam>
template <typename T>
class IPropertyHandler
	: public PropertyHandlerBase
{
	GENERATE_CLASS_TYPE_INFO(IPropertyHandler)

private:
	// Ÿ���� �迭 ���� �����ϰ� ���� Ÿ�Ը� �����ؼ� ���� 
	using ElementType = std::remove_all_extents_t<T>;

public:
	// ������ �� Getter Setter
	virtual ElementType& Get(void* object, size_t index = 0) const abstract;
	virtual void Set(void* object, const ElementType& value, size_t index = 0) const abstract;
	virtual std::string Dump(void* _object, int _indent = 0) const abstract;
	virtual bool DisplayUI(void* _object, const char* _name, float _min, float _max) const abstract;
};

/// <summary>
/// ���� �ڵ鷯
/// </summary>
/// <typeparam name="TClass">������ ���� Ŭ����</typeparam>
/// <typeparam name="T">���� Ÿ��</typeparam>
template <typename TClass, typename T>
class PropertyHandler
	: public IPropertyHandler<T>
{
	// �� Ŭ������ refelction ���� 
	GENERATE_CLASS_TYPE_INFO(PropertyHandler)

private:
	// ����� ���� �ּ� ������ Ÿ��(ex: int Transform::position *)
	using MemberPtr = T TClass::*;
	// �迭 ���� ��Ұ� ���ŵ� ���� Ÿ�� 
	using ElementType = std::remove_all_extents_t<T>;
	// ���
	MemberPtr m_ptr = nullptr;

public:
	virtual bool DisplayUI(void* _object, const char* _name, float _min, float _max) const override
	{
		if constexpr (std::is_array_v<T>)
		{
			bool isSelect = false;
			for (int i = 0; i < is_array_custom<T>::size; i++)
			{
				auto& obj = Get(_object, i);
				isSelect |= TypeUI::DisplayType(obj, _name, _min, _max);
			}
			return isSelect;
		}
		else if constexpr (IsStdVector<T>::value)
		{
			auto& obj = Get(_object);
			bool isSelect = false;
			for (size_t i = 0; i < obj.size(); i++)
			{
				auto o = obj[i];
				isSelect |= TypeUI::DisplayType(o, _name, _min, _max, static_cast<uint32>(i));
			}
			return isSelect;
		}
		else
			return TypeUI::DisplayType(Get(_object), _name, _min, _max);
	}

	virtual std::string Dump(void* _object, int _indent = 0) const override
	{
		if constexpr (std::is_array_v<T>)
		{
			std::string result = "";
			result += "\n";

			for (int i = 0; i < is_array_custom<T>::size; i++)
			{
				auto& obj = Get(_object, i);
				result += std::string(_indent, '\t');
				result += "[";
				result += StringConverter::ToString(i);
				result += "] : ";
				result += StringConverter::ToString(obj, _indent);
				result += "\n";
			}
			return result;
		}
		else
		{
			std::string result;
			auto& obj = Get(_object);
			result += StringConverter::ToString(obj, _indent);
			return result;
		}
	}

	// Getter
	virtual ElementType& Get(void* _object, size_t _index = 0) const override
	{
		// �迭�� ��� �ε��� ���� �޾ƿͼ� ó��
		if constexpr (std::is_array_v<T>)
			return (static_cast<TClass*>(_object)->*m_ptr)[_index];
		else
			return static_cast<TClass*>(_object)->*m_ptr;
	}

	// Setter
	virtual void Set(void* _object, const ElementType& _value, size_t _index = 0) const override
	{
		if constexpr (std::is_array_v<T>)
			Set((static_cast<TClass*>(_object)->*m_ptr)[_index], _value);
		else
			Set(static_cast<TClass*>(_object)->*m_ptr, _value);
	}

	// ������ 
	explicit PropertyHandler(MemberPtr _ptr) :
		m_ptr(_ptr) {}

private:
	template <typename T>
	void Set(T& _dest, const T& _src) const
	{
		_dest = _src;
	}

	template <typename KeyType, typename ValueType, typename Pred, typename Alloc>
	void Set(std::map<KeyType, ValueType, Pred, Alloc>& _dest, const std::map<KeyType, ValueType, Pred, Alloc>& _src) const
	{
		if constexpr (std::is_copy_assignable_v<KeyType> && std::is_copy_assignable_v<ValueType>)
			_dest = _src;
	}
};

/// <summary>
/// ���� ���� �ڵ鷯
/// �׳� �ڵ鷯�� ũ�� �ٸ��� ����
/// </summary>
/// <typeparam name="TClass">Ŭ����</typeparam>
/// <typeparam name="T">���� Ÿ��</typeparam>
template <typename TClass, typename T>
class StaticPropertyHandler
	: public IPropertyHandler<T>
{
	GENERATE_CLASS_TYPE_INFO(StaticPropertyHandler)

private:
	// ��� ������ Ŭ������ ���� ���� �� �ʿ�� ����.
	T* m_ptr = nullptr;
	using ElementType = std::remove_all_extents_t<T>;

public:
	virtual ElementType& Get([[maybe_unused]] void* object, size_t index = 0) const override
	{
		if constexpr (std::is_array_v<T>)
			return (*m_ptr)[index];
		else
			return *m_ptr;
	}

	virtual void Set([[maybe_unused]] void* object, const ElementType& value, size_t index = 0) const override
	{
		if constexpr (std::is_array_v<T>)
			(*m_ptr)[index] = value;
		else
			*m_ptr = value;
	}

	explicit StaticPropertyHandler(T* ptr) :
		m_ptr(ptr) {}
};

/// <summary>
/// ���� �ʱ�ȭ ����ü
/// </summary>
struct PropertyInitializer
{
	const char* m_name = nullptr;
	const TypeInfo& m_type;
	const PropertyHandlerBase& m_handler;
};

/// <summary>
/// ���� Ŭ����
/// ���� ���� ������ ��� Ŭ����
/// </summary>
class Property
{
private:
	const char* m_name = nullptr;
	const TypeInfo& m_type;
	const PropertyHandlerBase& m_handler;
	const int m_arraySize;

	float m_min = 0.0f;
	float m_max = 0.0f;

public:
	const char* GetName() const { return m_name; }

	const std::string Dump(void* _object, int _indent = 0) const
	{
		std::string result = "";
		result += std::string(_indent, '\t');
		result += m_type.m_fullName;
		result += " ";
		result += m_name;
		result += " : ";
		result += m_handler.Dump(_object, _indent + 1);
		result += "\n";
		return result;
	}

	bool DisplayUI(void* _object, const std::string& _ID) const
	{
		std::string name = m_name;
		name += _ID;
		return m_handler.DisplayUI(_object, name.c_str(), m_min, m_max);
	}

	// �� ��ȯ ����ü
	template <typename T>
	struct ReturnValueWrapper
	{
	private:
		// ������ �ּ�
		T* m_value = nullptr;

	public:
		// ������
		explicit ReturnValueWrapper(T& _value)
			: m_value(&_value)
		{
		}

		// �Ҹ���
		ReturnValueWrapper() = default;

		// ���� ������
		ReturnValueWrapper& operator=(const T& _value)
		{
			*m_value = _value;
			return *this;
		}

		// �� ��������
		operator T& () { return *m_value; }

		// Getter
		T& Get() { return *m_value; }
		const T& Get() const { return *m_value; }

		// �� ������
		friend bool operator==(const ReturnValueWrapper& lhs, const ReturnValueWrapper& rhs)
		{
			return *lhs.m_value == *rhs.m_value;
		}
		friend bool operator!=(const ReturnValueWrapper& lhs, const ReturnValueWrapper& rhs)
		{
			return !(lhs == rhs);
		}
	};

	// Getter
	template <typename T>
	ReturnValueWrapper<T> Get(void* object) const
	{
		auto s = typeid(T).name();
		// �ڵ鷯�� ���� ���� �ڵ鷯 �������̽��� ��� �޾�����
		// �� reflection ��ü���	
		if (m_handler.GetTypeInfo().IsChildOf<IPropertyHandler<T>>())
		{
			// ��Ȯ�� Ÿ������ ��ȯ �� �� ���� Ŭ������ ����
			auto concreteHandler = static_cast<const IPropertyHandler<T>*>(&m_handler);
			return ReturnValueWrapper(concreteHandler->Get(object));
		}
		else
		{
			assert(false && "Property::Get<T> - Invalied casting");
			return {};
		}
	}

	// setter
	template <typename T>
	void Set(void* object, const T& value) const
	{
		// �ڵ鷯�� ���� ���� �ڵ鷯 �������̽��� ��� �޾�����
		// �� reflection ��ü���
		if (m_handler.GetTypeInfo().IsChildOf<IPropertyHandler<T>>())
		{
			auto concreteHandler = static_cast<const IPropertyHandler<T>*>(&m_handler);
			concreteHandler->Set(object, value);
		}
		else
		{
			assert(false && "Property::Set<T> - Invalied casting");
		}
	}

	// getter (Ŭ������ ���� ����)
	template <typename TClass, typename T>
	ReturnValueWrapper<T> Get(void* object) const
	{
		const TypeInfo& typeinfo = m_handler.GetTypeInfo();
		if (typeinfo.IsA<PropertyHandler<TClass, T>>())
		{
			auto concreteHandler = static_cast<const PropertyHandler<TClass, T>&>(m_handler);
			return ReturnValueWrapper(concreteHandler.Get(object));
		}
		else if (typeinfo.IsA<StaticPropertyHandler<TClass, T>>())
		{
			auto concreteHandler = static_cast<const StaticPropertyHandler<TClass, T>&>(m_handler);
			return ReturnValueWrapper(concreteHandler.Get(object));
		}
		else
		{
			assert(false && "Property::Get<TClass, T> - Invalied casting");
			return {};
		}
	}

	// setter (Ŭ������ ���� ����)
	template <typename TClass, typename T>
	void Set(void* object, const T& value) const
	{
		const TypeInfo& typeinfo = m_handler.GetTypeInfo();
		if (typeinfo.IsA<PropertyHandler<TClass, T>>())
		{
			auto concreteHandler = static_cast<const PropertyHandler<TClass, T>&>(m_handler);
			concreteHandler.Set(object, value);
		}
		else if (typeinfo.IsA<StaticPropertyHandler<TClass, T>>())
		{
			auto concreteHandler = static_cast<const StaticPropertyHandler<TClass, T>&>(m_handler);
			concreteHandler.Set(object, value);
		}
		else
			assert(false && "Property::Set<TClass, T> - Invalied casting");
	}

	// Ÿ�� ���� ��������
	const TypeInfo& GetTypeInfo() const
	{
		return m_type;
	}

	// ������
	Property(TypeInfo& owner, const PropertyInitializer& initializer, int _arraySize = 0, float _min = 0.0f, float _max = 0.0f)
		: m_name(initializer.m_name)
		, m_type(initializer.m_type)
		, m_handler(initializer.m_handler)
		, m_arraySize(_arraySize)
		, m_min(_min)
		, m_max(_max)
	{
		owner.AddProperty(this);
	}
};

template <typename T>
struct SizeOfArray
{
	constexpr static uint32 value = 1;
};

template <typename T, size_t N>
struct SizeOfArray<T[N]>
{
	constexpr static uint32 value = SizeOfArray<T>::value * N;
};

/// <summary>
/// ���� ���� Ŭ������ �����ϴ� Ŭ����
/// </summary>
/// <typeparam name="TClass">������ �ִ� Ŭ����</typeparam>
/// <typeparam name="T">���� Ÿ��</typeparam>
/// <typeparam name="TPtr">���� ������</typeparam>
/// <typeparam name="ptr">������ ���� �ּ�</typeparam>
template <typename TClass, typename T, typename TPtr, TPtr ptr>
class PropertyRegister
{
public:
	/// <summary>
	/// ������ 
	/// </summary>
	/// <param name="name">�̸�</param>
	/// <param name="typeInfo">Ÿ�� ����</param>
	PropertyRegister(const char* name, TypeInfo& typeInfo, float _min = 0.0f, float _max = 0.0f)
	{
		// ���� ��� �����Ͷ��
		if constexpr (std::is_member_pointer_v<TPtr>)
		{
			int size = 0;
			if constexpr (is_array_custom<T>::value)
			{
				size = is_array_custom<T>::size;
			}
			static PropertyHandler<TClass, T> handler(ptr);
			static PropertyInitializer intializer =
			{
			name,
			TypeInfo::GetStaticTypeInfo<T>(),
			handler
			};
			static Property property(typeInfo, intializer, size, _min, _max);
		}
		else
		{
			int size = 0;
			if constexpr (is_array_custom<T>::value)
			{
				size = is_array_custom<T>::size;
			}
			static StaticPropertyHandler<TClass, T> handler(ptr);
			static PropertyInitializer intializer =
			{
			name,
			TypeInfo::GetStaticTypeInfo<T>(),
			handler
			};
			static Property property(typeInfo, intializer, 0, _min, _max);
		}
	}
};