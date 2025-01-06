#pragma once
#include "TypeInfoMacros.h"
#include <cassert>
#include "StringConverter.h"

/// <summary>
/// 함수 reflection 클래스
/// </summary>
class CallableBase
{
	// reflection 가능 객체
	GENERATE_CLASS_TYPE_INFO(CallableBase)

public:
	// 소멸자
	virtual ~CallableBase() = default;
	virtual std::string Dump(void* _object, int _indent = 0) const abstract;
};

/// <summary>
/// 인터페이스 클래스
/// </summary>
/// <typeparam name="TRet">리턴 타입</typeparam>
/// <typeparam name="...TArgs">인자 타입</typeparam>
template <typename TRet, typename... TArgs>
class ICallable
	: public CallableBase
{
	// reflection 가능 클래스
	GENERATE_CLASS_TYPE_INFO(ICallable)

public:
	// 함수 호출
	virtual TRet Invoke(void* _caller, TArgs&&... _args) const abstract;
	virtual std::string Dump(void* _object, int _indent = 0) const abstract;
};

/// <summary>
/// 함수 호출 클래스
/// </summary>
/// <typeparam name="TClass">클래스</typeparam>
/// <typeparam name="TRet">리턴 타입</typeparam>
/// <typeparam name="...TArgs">인자 타입</typeparam>
template <typename TClass, typename TRet, typename... TArgs>
class Callable
	: public ICallable<TRet, TArgs...>
{
	GENERATE_CLASS_TYPE_INFO(Callable)
private:
	// 함수 포인터
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
	/// 함수 호출
	/// </summary>
	/// <param name="_caller">함수 주체</param>
	/// <param name="..._args">인자</param>
	/// <returns>리턴 값 (can void)</returns>
	virtual TRet Invoke(void* _caller, TArgs&&... _args) const override
	{
		// 만일 리턴값이 void 라면
		if constexpr (std::is_void_v<TRet>)
		{
			// 실행 만 한다.
			(static_cast<TClass*>(_caller)->*m_ptr)(std::forward<TArgs>(_args)...);
		}
		// 리턴값이 있다면
		else
		{
			// 해당 값을 리턴한다.
			return (static_cast<TClass*>(_caller)->*m_ptr)(std::forward<TArgs>(_args)...);
		}
	}

	// 생성자
	Callable(FuncPtr ptr) :
		m_ptr(ptr) {}
};

/// <summary>
/// 정적 함수 호출 클래스
/// </summary>
/// <typeparam name="TClass">클래스</typeparam>
/// <typeparam name="TRet">리턴 타입</typeparam>
/// <typeparam name="...TArgs">인자 타입</typeparam>
template <typename TClass, typename TRet, typename... TArgs>
class StaticCallable
	: public ICallable<TRet, TArgs...>
{
	GENERATE_CLASS_TYPE_INFO(StaticCallable)

private:
	// 함수 포인터
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
	/// 호출 함수
	/// </summary>
	/// <param name="caller">함수 주체(정적 함수라 필요 없을 가능성이 높음)</param>
	/// <param name="...args">인자들</param>
	/// <returns>리턴 값</returns>
	virtual TRet Invoke([[maybe_unused]] void* caller, TArgs&&... args) const override
	{
		// 리턴 타입이 void면
		if constexpr (std::is_void_v<TRet>)
		{
			(*m_ptr)(std::forward<TArgs>(args)...);
		}
		// 리턴 타입이 있다면
		else
		{
			return (*m_ptr)(std::forward<TArgs>(args)...);
		}
	}

	// 생성자
	StaticCallable([[maybe_unused]] TClass* owner, FuncPtr ptr) :
		m_ptr(ptr) {}
};

/// <summary>
/// 함수 reflection 클래스
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
	/// 함수 호출
	/// </summary>
	/// <typeparam name="TClass">함수가 있는 클래스</typeparam>
	/// <typeparam name="TRet">리턴 타입</typeparam>
	/// <typeparam name="...TArgs">인자 타입</typeparam>
	/// <param name="caller">함수 호출자</param>
	/// <param name="...args">인자들</param>
	/// <returns>리턴 값</returns>
	template <typename TClass, typename TRet, typename... TArgs>
	TRet Invoke(void* _caller, TArgs&... _args) const
	{
		// 호출자의 타입 정보를 받아옴
		const TypeInfo& typeinfo = m_callable.GetTypeInfo();

		// 호출자가 Callable 클래스의 자식 클래스라면
		if (typeinfo.IsChildOf<Callable<TClass, TRet, TArgs...>>())
		{
			// 업캐스팅
			auto concreateCallable = static_cast<const Callable<TClass, TRet, TArgs...>&>(m_callable);
			// 리턴값에 따른 함수 호출
			if constexpr (std::is_void_v<TRet>)
			{
				concreateCallable.Invoke(_caller, std::forward<TArgs>(_args)...);
			}
			else
			{
				return concreateCallable.Invoke(_caller, std::forward<TArgs>(_args)...);
			}
		}
		// 정적 함수인 경우
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
		// 위 두 경우가 아니라면 버그
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
	/// 함수 호출
	/// </summary>
	/// <typeparam name="TRet">리턴값</typeparam>
	/// <typeparam name="...TArgs">인자 타입</typeparam>
	/// <param name="owner">주인</param>
	/// <param name="...args">인자</param>
	/// <returns>리턴 값</returns>
	template <typename TRet, typename... TArgs>
	TRet Invoke(void* _owner, TArgs&... _args) const
	{
		// 호출자는 호출 인터페이스의 자식이여야한다.
		if (m_callable.GetTypeInfo().IsChildOf<ICallable<TRet, TArgs...>>())
		{
			auto concreateCallable = static_cast<const ICallable<TRet, TArgs...>*>(&m_callable);
			// 타입에 따른 함수 호출
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
	/// 생성자 
	/// </summary>
	/// <typeparam name="TRet">리턴 타입</typeparam>
	/// <typeparam name="...TArgs">인자 타입</typeparam>
	/// <param name="owner">함수 주인</param>
	/// <param name="ptr">함수 포인터</param>
	/// <param name="name">이름</param>
	/// <param name="callable">호출자</param>
	template <typename TRet, typename... TArgs>
	Method(TypeInfo& owner, [[maybe_unused]] TRet(*ptr)(TArgs...), const char* name, const CallableBase& callable) :
		m_name(name),
		m_callable(callable)
	{
		CollectFunctionSignature<TRet, TArgs...>();
		owner.AddMethod(this);
	}

	/// <summary>
	/// 생성자
	/// </summary>
	/// <typeparam name="TClass">클래스</typeparam>
	/// <typeparam name="TRet">리턴 타입</typeparam>
	/// <typeparam name="...TArgs">인자 타입</typeparam>
	/// <param name="owner">함수 주인</param>
	/// <param name="ptr">함수 포인터</param>
	/// <param name="name">이름</param>
	/// <param name="callable">호출자</param>
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

