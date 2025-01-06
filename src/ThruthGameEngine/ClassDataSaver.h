#pragma once
#include <string>
#include <type_traits>
#include <memory>
#include <vector>
#include <map>
#include <list>
#include "SimpleMath.h"
#include "StringConverter.h"

namespace Saver
{
	void SaveStart();
	void SaveEnd();


	template<typename T>
	void Save(T _val, const std::string& _name)
	{
		if constexpr (IsStdSharedPtr<T>::value)
			Save(*_val);
	}

	template<>
	void Save(int _val, const std::string& _name);

	template<>
	void Save(float _val, const std::string& _name);

	template<>
	void Save(bool _val, const std::string& _name);

	template<>
	void Save(std::string _val, const std::string& _name);

	template<>
	void Save(std::wstring _val, const std::string& _name);

	template<>
	void Save(Vector4 _val, const std::string& _name);

	template<>
	void Save(Vector3 _val, const std::string& _name);

	template<>
	void Save(Vector2 _val, const std::string& _name);
}


