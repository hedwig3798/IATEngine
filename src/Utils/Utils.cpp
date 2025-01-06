#include "Utils.h"


/// <summary>
/// wstring �� string ���� ��ȯ
/// </summary>
std::string Utils::ToString(std::wstring _value)
{
	return std::string(_value.begin(), _value.end());
}

/// <summary>
/// string �� wstring ���� ��ȯ
/// </summary>
std::wstring Utils::ToWString(std::string _value)
{
	return std::wstring(_value.begin(), _value.end());
}

/// <summary>
/// ���ڿ� str ���� comp �� ã�Ƽ� rep �� ��ü�ϴ� �Լ�
/// ���ڿ��� �����ϰ� �����Ѵ�.
/// </summary>
/// <param name="str">��� ���ڿ�</param>
/// <param name="comp">��ü�� �κ� ���ڿ�</param>
/// <param name="rep">comp�� ��ü�� �� �κ� ���ڿ�</param>
void Utils::Replace(std::string& _str, std::string _comp, std::string _rep)
{
	std::string temp = _str;

	size_t startPos = 0;

	// find �Լ��� ���н� npos �� ��ȯ
	while ((startPos = temp.find(_comp, startPos)) != std::string::npos)
	{
		temp.replace(startPos, _comp.length(), _rep);
		startPos += _rep.length();
	}

	_str = temp;
}

void Utils::Replace(std::wstring& _str, std::wstring _comp, std::wstring _rep)
{
	std::wstring temp = _str;

	size_t startPos = 0;
	while ((startPos = temp.find(_comp, startPos)) != std::wstring::npos)
	{
		temp.replace(startPos, _comp.length(), _rep);
		startPos += _rep.length();
	}

	_str = temp;
}

/// <summary>
/// �־��� ���ڿ��� Ư�� ���ڿ��� �����ϴ��� ���θ� Ȯ���ϴ� �Լ�
/// </summary>
/// <param name="str">�־��� ���ڿ�</param>
/// <param name="comp">���� ���� ���� ���ڿ�</param>
/// <returns></returns>
bool Utils::StartsWith(std::string _str, std::string _comp)
{
	std::wstring::size_type index = _str.find(_comp);
	if (index != std::wstring::npos && index == 0)
	{
		return true;
	}

	return false;
}

bool Utils::StartsWith(std::wstring _str, std::wstring _comp)
{
	std::wstring::size_type index = _str.find(_comp);
	if (index != std::wstring::npos && index == 0)
	{
		return true;
	}

	return false;
}