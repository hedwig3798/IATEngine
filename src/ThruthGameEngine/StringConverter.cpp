#include "StringConverter.h"
#include "Component.h"
#include "Entity.h"

template<>
std::string StringConverter::ToString(bool _val, int _indent)
{
	if (_val)
	{
		return "true";
	}
	else
	{
		return "false";
	}
}

template<>
std::string StringConverter::ToString(Vector4 _val, int _indent)
{
	std::string result = "";
	result += "{ ";
	result += ToString(_val.x);
	result += ", ";
	result += ToString(_val.y);
	result += ", ";
	result += ToString(_val.z);
	result += ", ";
	result += ToString(_val.w);
	result += " }";
	return result;
}

template<>
std::string StringConverter::ToString(Vector3 _val, int _indent)
{
	std::string result = "";
	result += "{ ";
	result += ToString(_val.x);
	result += ", ";
	result += ToString(_val.y);
	result += ", ";
	result += ToString(_val.z);
	result += " }";
	return result;
}

template<>
std::string StringConverter::ToString(Vector2 _val, int _indent)
{
	std::string result = "";
	result += "{ ";
	result += ToString(_val.x);
	result += ", ";
	result += ToString(_val.y);
	result += " }";
	return result;
}

template<>
std::string StringConverter::ToString(std::shared_ptr<Truth::Component> _val, int _indent)
{
	std::string result = "";
	result += _val->GetTypeInfo().Dump(_val.get(), _indent);
	return result;
}

template<>
std::string StringConverter::ToString(std::shared_ptr<Truth::Entity> _val, int _indent)
{
	return "Entity";
}

template<>
std::string StringConverter::ToString(std::string _val, int _indent)
{
	return _val;
}


template<>
std::string StringConverter::ToString(std::wstring _val, int _indent)
{
	return "wstring Data";
}

template<>
std::string StringConverter::ToString(std::shared_ptr<Ideal::IMaterial> _val, int _indent)
{
	return "wstring Data";
}

std::vector<std::string> StringConverter::split(std::string _str, char _delimiter)
{
	std::istringstream iss(_str);            
	std::string buffer;                

	std::vector<std::string> result;

	while (std::getline(iss, buffer, _delimiter)) 
	{
		if (buffer.empty())
		{
			continue;
		}
		result.push_back(buffer);               
	}

	return result;
}

std::vector<std::wstring> StringConverter::split(std::wstring _str, wchar_t _delimiter)
{
	std::wstringstream iss(_str);             
	std::wstring buffer;                    
	std::vector<std::wstring> result;

	while (std::getline(iss, buffer, _delimiter)) 
	{
		result.push_back(buffer);              
	}

	return result;
}

void StringConverter::DeleteAlpha(std::string& _str, char _delimiter)
{
	_str.erase(remove(_str.begin(), _str.end(), _delimiter));
}

void StringConverter::DeleteAlpha(std::wstring& _str, wchar_t _delimiter)
{
	_str.erase(remove(_str.begin(), _str.end(), _delimiter));
}

