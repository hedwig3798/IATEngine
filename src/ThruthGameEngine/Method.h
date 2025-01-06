#pragma once
#include "TypeInfoMacros.h"
#include <cassert>
#include "StringConverter.h"

/// <summary>
/// �Լ� reflection Ŭ����
/// </summary>
class CallableBase
{
	// reflection ���� ��ü
	GENERATE_CLASS_TYPE_INFO(CallableBase)

public:
	// �Ҹ���
	virtual ~CallableBase() = default;
	virtual std::string Dump(void* _object, int _indent = 0) const abstract;
};

/// <summary>
/// �������̽� Ŭ����
/// </summary>
/// <typeparam name="TRet">���� Ÿ��</typeparam>
/// <typeparam name="...TArgs">���� Ÿ��</typeparam>
template <typename TRet, typename... TArgs>
class ICallable
	: public CallableBase
{
	// reflection ���� Ŭ����
	GENERATE_CLASS_TYPE_INFO(ICallable)

public:
	// �Լ� ȣ��
	virtual TRet Invoke(void* _caller, TArgs&&... _args) const abstract;
	virtual std::string Dump(void* _object, int _indent = 0) const abstract;
};

/// <summary>
/// �Լ� ȣ�� Ŭ����
/// </summary>
/// <typeparam name="TClass">Ŭ����</typeparam>
/// <typeparam name="TRet">���� Ÿ��</typeparam>
/// <typeparam name="...TArgs">���� Ÿ��</typeparam>
template <typename TClass, typename TRet, typename... TArgs>
class Callable
	: public ICallable<TRet, TArgs...>
{
	GENERATE_CLASS_TYPE_INFO(Callable)
private:
	// �Լ� ������
	using FuncPtr = TRet(TClass::*)(TArgs...);
	FuncPtr m_ptr = nullptr;

public:
	virtual std::string Dump(void* _object, int _indent = 0) const
	{
		std::string result = "";
		result += typeid(TRet).name();
		result += " ";
		result += typeid(TClass).name();
		result += " ";
		// result += typeid(TArgs...).name();
		result += "\n";
		return result;
	}

	/// <summary>
	/// �Լ� ȣ��
	/// </summary>
	/// <param name="_caller">�Լ� ��ü</param>
	/// <param name="..._args">����</param>
	/// <returns>���� �� (can void)</returns>
	virtual TRet Invoke(void* _caller, TArgs&&... _args) const override
	{
		// ���� ���ϰ��� void ���
		if constexpr (std::is_void_v<TRet>)
		{
			// ���� �� �Ѵ�.
			(static_cast<TClass*>(_caller)->*m_ptr)(std::forward<TArgs>(_args)...);
		}
		// ���ϰ��� �ִٸ�
		else
		{
			// �ش� ���� �����Ѵ�.
			return (static_cast<TClass*>(_caller)->*m_ptr)(std::forward<TArgs>(_args)...);
		}
	}

	// ������
	Callable(FuncPtr ptr) :
		m_ptr(ptr) {}
};

/// <summary>
/// ���� �Լ� ȣ�� Ŭ����
/// </summary>
/// <typeparam name="TClass">Ŭ����</typeparam>
/// <typeparam name="TRet">���� Ÿ��</typeparam>
/// <typeparam name="...TArgs">���� Ÿ��</typeparam>
template <typename TClass, typename TRet, typename... TArgs>
class StaticCallable
	: public ICallable<TRet, TArgs...>
{
	GENERATE_CLASS_TYPE_INFO(StaticCallable)

private:
	// �Լ� ������
	using FuncPtr = TRet(*)(TArgs...);
	FuncPtr m_ptr = nullptr;

public:
	virtual std::string Dump(void* _object, int _indent = 0) const
	{
		std::string result = "";
		result += typeid(TRet).name();
		result += " ";
		result += typeid(TClass).name();
		result += " ";
		// result += typeid(TArgs...).name();
		result += "\n";
		return result;
	}

	/// <summary>
	/// ȣ�� �Լ�
	/// </summary>
	/// <param name="caller">�Լ� ��ü(���� �Լ��� �ʿ� ���� ���ɼ��� ����)</param>
	/// <param name="...args">���ڵ�</param>
	/// <returns>���� ��</returns>
	virtual TRet Invoke([[maybe_unused]] void* caller, TArgs&&... args) const override
	{
		// ���� Ÿ���� void��
		if constexpr (std::is_void_v<TRet>)
		{
			(*m_ptr)(std::forward<TArgs>(args)...);
		}
		// ���� Ÿ���� �ִٸ�
		else
		{
			return (*m_ptr)(std::forward<TArgs>(args)...);
		}
	}

	// ������
	StaticCallable([[maybe_unused]] TClass* owner, FuncPtr ptr) :
		m_ptr(ptr) {}
};

/// <summary>
/// �Լ� reflection Ŭ����
/// </summary>
class Method
{
private:
	const TypeInfo* m_returnType = nullptr;
	std::vector<const TypeInfo*> m_parameterTypes;
	const char* m_name = nullptr;
	const CallableBase& m_callable;

public:

	const std::string Dump(void* _object, int _indent = 0) const 
	{
		std::string result = "";
		result += std::string(_indent, '\t');
		result += m_returnType->GetFullName();
		result += " ";
		result += m_name;
		result += " (";
		if (m_parameterTypes.empty())
		{
			result += ")";
		}
		else
		{
			for (auto& p : m_parameterTypes)
			{
				result += p->GetFullName();
				result += ", ";
			}
			result.pop_back();
			result.pop_back();
			result += ")";
		}
		result += "\n";
		return result;
	}

	const char* GetName() const { return m_name; }

	const TypeInfo& GetReturnType() const { return *m_returnType; }

	const TypeInfo& GetParameterType(size_t i) const { return *m_parameterTypes[i]; }

	size_t NumParameter() const { return m_parameterTypes.size(); }

	/// <summary>
	/// �Լ� ȣ��
	/// </summary>
	/// <typeparam name="TClass">�Լ��� �ִ� Ŭ����</typeparam>
	/// <typeparam name="TRet">���� Ÿ��</typeparam>
	/// <typeparam name="...TArgs">���� Ÿ��</typeparam>
	/// <param name="caller">�Լ� ȣ����</param>
	/// <param name="...args">���ڵ�</param>
	/// <returns>���� ��</returns>
	template <typename TClass, typename TRet, typename... TArgs>
	TRet Invoke(void* _caller, TArgs&... _args) const
	{
		// ȣ������ Ÿ�� ������ �޾ƿ�
		const TypeInfo& typeinfo = m_callable.GetTypeInfo();

		// ȣ���ڰ� Callable Ŭ������ �ڽ� Ŭ�������
		if (typeinfo.IsChildOf<Callable<TClass, TRet, TArgs...>>())
		{
			// ��ĳ����
			auto concreateCallable = static_cast<const Callable<TClass, TRet, TArgs...>&>(m_callable);
			// ���ϰ��� ���� �Լ� ȣ��
			if constexpr (std::is_void_v<TRet>)
			{
				concreateCallable.Invoke(_caller, std::forward<TArgs>(_args)...);
			}
			else
			{
				return concreateCallable.Invoke(_caller, std::forward<TArgs>(_args)...);
			}
		}
		// ���� �Լ��� ���
		else if (typeinfo.IsChildOf<StaticCallable<TClass, TRet, TArgs...>>())
		{
			auto concreateCallable = static_cast<const StaticCallable<TClass, TRet, TArgs...>&>(m_callable);
			if constexpr (std::is_void_v<TRet>)
			{
				concreateCallable.Invoke(_caller, std::forward<TArgs>(_args)...);
			}
			else
			{
				return concreateCallable.Invoke(_caller, std::forward<TArgs>(_args)...);
			}
		}
		// �� �� ��찡 �ƴ϶�� ����
		else
		{
			assert(false && "Method::Invoke<TClass, TRet, TArgs...> - Invalied casting");
			if constexpr (!std::is_void_v<TRet>)
			{
				return {};
			}
		}
	}

	/// <summary>
	/// �Լ� ȣ��
	/// </summary>
	/// <typeparam name="TRet">���ϰ�</typeparam>
	/// <typeparam name="...TArgs">���� Ÿ��</typeparam>
	/// <param name="owner">����</param>
	/// <param name="...args">����</param>
	/// <returns>���� ��</returns>
	template <typename TRet, typename... TArgs>
	TRet Invoke(void* _owner, TArgs&... _args) const
	{
		// ȣ���ڴ� ȣ�� �������̽��� �ڽ��̿����Ѵ�.
		if (m_callable.GetTypeInfo().IsChildOf<ICallable<TRet, TArgs...>>())
		{
			auto concreateCallable = static_cast<const ICallable<TRet, TArgs...>*>(&m_callable);
			// Ÿ�Կ� ���� �Լ� ȣ��
			if constexpr (std::is_void_v<TRet>)
			{
				concreateCallable->Invoke(_owner, std::forward<TArgs>(_args)...);
			}
			else
			{
				return concreateCallable->Invoke(_owner, std::forward<TArgs>(_args)...);
			}
		}
		else
		{
			assert(false && "Method::Invoke<TRet, TArgs...> - Invalied casting");
			if constexpr (!std::is_void_v<TRet>)
			{
				return {};
			}
		}
	}

	/// <summary>
	/// ������ 
	/// </summary>
	/// <typeparam name="TRet">���� Ÿ��</typeparam>
	/// <typeparam name="...TArgs">���� Ÿ��</typeparam>
	/// <param name="owner">�Լ� ����</param>
	/// <param name="ptr">�Լ� ������</param>
	/// <param name="name">�̸�</param>
	/// <param name="callable">ȣ����</param>
	template <typename TRet, typename... TArgs>
	Method(TypeInfo& owner, [[maybe_unused]] TRet(*ptr)(TArgs...), const char* name, const CallableBase& callable) :
		m_name(name),
		m_callable(callable)
	{
		CollectFunctionSignature<TRet, TArgs...>();
		owner.AddMethod(this);
	}

	/// <summary>
	/// ������
	/// </summary>
	/// <typeparam name="TClass">Ŭ����</typeparam>
	/// <typeparam name="TRet">���� Ÿ��</typeparam>
	/// <typeparam name="...TArgs">���� Ÿ��</typeparam>
	/// <param name="owner">�Լ� ����</param>
	/// <param name="ptr">�Լ� ������</param>
	/// <param name="name">�̸�</param>
	/// <param name="callable">ȣ����</param>
	template <typename TClass, typename TRet, typename... TArgs>
	Method(TypeInfo& owner, [[maybe_unused]] TRet(TClass::* ptr)(TArgs...), const char* name, const CallableBase& callable) :
		m_name(name),
		m_callable(callable)
	{
		CollectFunctionSignature<TRet, TArgs...>();
		owner.AddMethod(this);
	}

private:
	template <typename TRet, typename... Args>
	void CollectFunctionSignature()
	{
		m_returnType = &TypeInfo::GetStaticTypeInfo<TRet>();
		m_parameterTypes.reserve(sizeof...(Args));

		(m_parameterTypes.emplace_back(&TypeInfo::GetStaticTypeInfo<Args>()), ...);
	}
};

template <typename TClass, typename TPtr, TPtr ptr>
class MethodRegister
{
public:
	MethodRegister(const char* name, TypeInfo& typeInfo)
	{
		if constexpr (std::is_member_function_pointer_v<TPtr>)
		{
			static Callable callable(ptr);
			static Method method(typeInfo, ptr, name, callable);
		}
		else
		{
			TClass* forDeduction = nullptr;
			static StaticCallable callable(forDeduction, ptr);
			static Method method(typeInfo, ptr, name, callable);
		}
	}
};

