#pragma once
#include <windows.h>
#include <string>
#include <assert.h>
#include "Types.h"


enum FileMode : uint8
{
	Write,
	Read
};

class TFileUtils
{
public:
	TFileUtils();
	~TFileUtils();

public:
	void Open(std::wstring _filePath, FileMode _mode);
	void Close();

public:
	// Write
	template<typename T>
	void Write(const T& _data)
	{
		// ���� üũ ����
		uint32 numOfBytes = 0;

		// ������ �����Ͱ� �ϳ��� ���ٸ� numOfBytes �� 0 ����Ʈ �� ���̰�,
		// ���������� numOfBytes �� 0�̶�� false �� ��ȯ�ؼ� ������ ���.
		assert(::WriteFile(m_Handle, &_data, sizeof(T), (LPDWORD)&numOfBytes, nullptr) && "���� ���� �������� ���� �߻�");
	}

	template<>
	void Write<std::string>(const std::string& _data)
	{
		return Write(_data);
	}

	void Write(void* __data, uint32 _dataSize);
	void Write(const std::string& __data);

public:
	// Read
	template<typename T>
	void Read(T& _data)
	{
		// ���� üũ ����
		uint32 numOfBytes = 0;

		BOOL readData = ::ReadFile(m_Handle, &_data, sizeof(T), (LPDWORD)&numOfBytes, nullptr);

		// ���� �����Ͱ� �ϳ��� ���ٸ� numOfBytes �� 0 ����Ʈ �� ���̰�,
		// ���������� numOfBytes �� 0�̶�� false �� ��ȯ�ؼ� ������ ���.
		assert(readData && "���� �б� �������� ���� �߻�");
	}

	template<typename T>
	T Read()
	{
		T data = {};
		Read(data);
		return data;
	}

	void Read(void** _data, uint32 _dataSize);
	void Read(std::string& _data);

private:
	HANDLE m_Handle = INVALID_HANDLE_VALUE;

};

