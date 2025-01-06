#pragma once
#include <string>

#pragma warning (disable : 4244)

/// <summary>
/// ��ƿ��Ƽ ����� �����ϴ� Ŭ����
/// </summary>
class Utils
{
public:
	// string -> wstring
	// wstring -> string
	static std::wstring ToWString(std::string _value);
	static std::string ToString(std::wstring _value);

	// ���ڿ� ��ü 
	static void Replace(std::string& _str, std::string _comp, std::string _rep);
	static void Replace(std::wstring& _str, std::wstring _comp, std::wstring _rep);

	// Ư�� ���ڿ��� �����ϴ��� ���� �˻�
	static bool StartsWith(std::string _str, std::string _comp);
	static bool StartsWith(std::wstring _str, std::wstring _comp);
};

