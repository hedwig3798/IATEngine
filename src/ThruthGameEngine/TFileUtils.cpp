#include "TFileUtils.h"
#include "Utils.h"
#include <assert.h>
#include <errhandlingapi.h>


TFileUtils::TFileUtils()
{

}

TFileUtils::~TFileUtils()
{
	if (m_Handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_Handle);
		m_Handle = INVALID_HANDLE_VALUE;
	}
}

/// <summary>
/// 읽기 또는 쓰기 모드에 따라서 파일을 생성하거나 쓰기, 읽을 파일을 지목한다.
/// 지목된 파일은 m_Handle 이 쥐고 있는다.
/// </summary>
void TFileUtils::Open(std::wstring _filePath, FileMode _mode)
{
	if (_mode == FileMode::Write)
	{
		m_Handle = ::CreateFile
		(
			_filePath.c_str(), // 파일 경로
			GENERIC_WRITE, // 쓰기 전용
			0,
			nullptr,
			CREATE_ALWAYS, // 지정된 파일이 있다면 덮어 쓰고 없다면 새로 만들어짐
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}
	else
	{
		m_Handle = ::CreateFile
		(
			_filePath.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,  // 파일이 있는 경우에만 읽는다. 없으면 실패 반환
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}
	if (m_Handle == INVALID_HANDLE_VALUE)
	{
		HRESULT hr =  HRESULT_FROM_WIN32(::GetLastError());
		int a = 1;
	}
	assert(m_Handle != INVALID_HANDLE_VALUE);
}

void TFileUtils::Close()
{
	if (m_Handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_Handle);
		m_Handle = INVALID_HANDLE_VALUE;
	}
}

void TFileUtils::Write(void* _data, uint32 _dataSize)
{
	// 오류 체크 변수
	uint32 numOfBytes = 0;

	BOOL writeData = ::WriteFile(m_Handle, _data, _dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr);

	// 쓰여진 데이터가 하나도 없다면 numOfBytes 는 0 바이트 일 것이고,
	// numOfBytes 가 0 이라면 false 를 반환해서 assert
	assert(writeData && "파일 쓰기 과정에서 오류 발생");
}

void TFileUtils::Write(const std::string& _data)
{
	// 문자열 길이 정보를 먼저 파일에 쓴다.
	uint32 size = (uint32)_data.size();
	Write(size);

	if (_data.size() == 0)
		return;

	// 문자열을 파일에 쓴다.
	Write((void*)_data.data(), size);
}

void TFileUtils::Read(void** _data, uint32 _dataSize)
{
	// 오류 체크 변수
	uint32 numOfBytes = 0;

	BOOL readData = ::ReadFile(m_Handle, *_data, _dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr);

	// 읽은 데이터가 하나도 없다면 numOfBytes 는 0 바이트 일 것이고,
	// numOfBytes 가 0 이라면 false 를 반환해서 assert
	assert(readData && "파일 읽기 과정에서 오류 발생");
}

void TFileUtils::Read(std::string& _data)
{
	uint32 size = Read<uint32>();

	if (size == 0)
		return;

	char* temp = new char[size + 1];
	temp[size] = NULL;
	Read((void**)&temp, size);
	_data = temp;
	delete[] temp;
}

