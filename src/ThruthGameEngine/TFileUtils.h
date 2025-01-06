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
		// 오류 체크 변수
		uint32 numOfBytes = 0;

		// 쓰여진 데이터가 하나도 없다면 numOfBytes 는 0 바이트 일 것이고,
		// 내부적으로 numOfBytes 가 0이라면 false 를 반환해서 오류가 뜬다.
		assert(::WriteFile(m_Handle, &_data, sizeof(T), (LPDWORD)&numOfBytes, nullptr) && "파일 쓰기 과정에서 오류 발생");
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
		// 오류 체크 변수
		uint32 numOfBytes = 0;

		BOOL readData = ::ReadFile(m_Handle, &_data, sizeof(T), (LPDWORD)&numOfBytes, nullptr);

		// 읽은 데이터가 하나도 없다면 numOfBytes 는 0 바이트 일 것이고,
		// 내부적으로 numOfBytes 가 0이라면 false 를 반환해서 오류가 뜬다.
		assert(readData && "파일 읽기 과정에서 오류 발생");
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

