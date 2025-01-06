#pragma once
#include <string>
#include <type_traits>
#include <memory>
#include <vector>
#include <map>
#include <list>
#include "SimpleMath.h"


using namespace DirectX::SimpleMath;

template <typename, typename = void>
struct IsStdVector
	: std::false_type
{
};
template <typename T, typename Alloc>
struct IsStdVector<std::vector<T, Alloc>>
	: std::true_type
{
};

template <typename, typename = void>
struct IsStdList
	: std::false_type
{
};
template <typename T, typename Alloc>
struct IsStdList<std::list<T, Alloc>>
	: std::true_type
{
};

template <typename, typename = void>
struct IsStdMap
	: std::false_type
{
};
template<typename T>
struct IsStdMap<T, std::void_t<typename T::key_type,
	typename T::mapped_type,
	decltype(std::declval<T&>()[std::declval<const typename T::key_type&>()])>>
	: std::true_type { };

template <typename = void>
struct IsStdSharedPtr
	: std::false_type
{
};
template <typename T>
struct IsStdSharedPtr<std::shared_ptr<T>>
	: std::true_type
{
};

namespace Truth
{
	class Entity;
	class Component;
	class Scene;
	struct Material;
}

namespace Ideal
{
	class IMaterial;
}

/// <summary>
/// ���ø� Ư��ȭ�� ���� �پ��� Ÿ���� ���ڿ�ȭ
/// �ʿ��� Ÿ���� �ִٸ� ���� �����Ͽ��� ��
/// </summary>
namespace StringConverter
{
	// ���ø� Ÿ�Կ� ���� Ÿ���� ���⼭ �����Ѵ�.
	template<typename T>
	std::string ToString(T _val, int _indent = 0)
	{
		if constexpr (std::is_integral_v<T>)
			return std::to_string(_val);

		else if constexpr (std::is_floating_point_v<T>)
			return std::to_string(_val);

		else if constexpr (IsStdSharedPtr<T>::value)
		{
			return "";
		}
		else if constexpr (IsStdVector<T>::value)
		{
			std::string result = "";
			result += "\n";

			for (int i = 0; i < _val.size(); i++)
			{
				result += std::string(_indent, '\t');
				result += "[";
				result += ToString(i, _indent);
				result += "] : ";
				result += ToString(_val[i], _indent);
				result += "\n";
			}
			return result;
		}
		else if constexpr (IsStdMap<T>::value)
		{
			std::string result = "";
			result += "\n";

			for (auto& v : _val)
			{
				result += std::string(_indent, '\t');
				result += "[";
				result += ToString(v.first, _indent);
				result += "] : ";
				result += ToString(v.second, _indent);
				result += "\n";
			}
			return result;
		}
		else
		{
			return "null";
		}
	}

	// �׿� ������ Ÿ���� �Ʒ��� ���� �����ؼ� ����Ͽ����Ѵ�.
	template<>
	std::string ToString(bool _val, int _indent);

	template<>
	std::string ToString(std::string _val, int _indent);

	template<>
	std::string ToString(std::wstring _val, int _indent);

	template<>
	std::string ToString(Vector4 _val, int _indent);

	template<>
	std::string ToString(Vector3 _val, int _indent);

	template<>
	std::string ToString(Vector2 _val, int _indent);

	template<>
	std::string ToString(std::shared_ptr<Truth::Component> _val, int _indent);

	template<>
	std::string ToString(std::shared_ptr<Truth::Entity> _val, int _indent);

	template<>
	std::string ToString(std::shared_ptr<Ideal::IMaterial> _val, int _indent);


	std::vector<std::string> split(std::string _str, char _delimiter);
	void DeleteAlpha(std::string& _str, char _delimiter);
	std::vector<std::wstring> split(std::wstring _str, wchar_t _delimiter);
	void DeleteAlpha(std::wstring& _str, wchar_t _delimiter);
}

