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
/// �б� �Ǵ� ���� ��忡 ���� ������ �����ϰų� ����, ���� ������ �����Ѵ�.
/// ����� ������ m_Handle �� ��� �ִ´�.
/// </summary>
void TFileUtils::Open(std::wstring _filePath, FileMode _mode)
{
	if (_mode == FileMode::Write)
	{
		m_Handle = ::CreateFile
		(
			_filePath.c_str(), // ���� ���
			GENERIC_WRITE, // ���� ����
			0,
			nullptr,
			CREATE_ALWAYS, // ������ ������ �ִٸ� ���� ���� ���ٸ� ���� �������
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
			OPEN_EXISTING,  // ������ �ִ� ��쿡�� �д´�. ������ ���� ��ȯ
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
	// ���� üũ ����
	uint32 numOfBytes = 0;

	BOOL writeData = ::WriteFile(m_Handle, _data, _dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr);

	// ������ �����Ͱ� �ϳ��� ���ٸ� numOfBytes �� 0 ����Ʈ �� ���̰�,
	// numOfBytes �� 0 �̶�� false �� ��ȯ�ؼ� assert
	assert(writeData && "���� ���� �������� ���� �߻�");
}

void TFileUtils::Write(const std::string& _data)
{
	// ���ڿ� ���� ������ ���� ���Ͽ� ����.
	uint32 size = (uint32)_data.size();
	Write(size);

	if (_data.size() == 0)
		return;

	// ���ڿ��� ���Ͽ� ����.
	Write((void*)_data.data(), size);
}

void TFileUtils::Read(void** _data, uint32 _dataSize)
{
	// ���� üũ ����
	uint32 numOfBytes = 0;

	BOOL readData = ::ReadFile(m_Handle, *_data, _dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr);

	// ���� �����Ͱ� �ϳ��� ���ٸ� numOfBytes �� 0 ����Ʈ �� ���̰�,
	// numOfBytes �� 0 �̶�� false �� ��ȯ�ؼ� assert
	assert(readData && "���� �б� �������� ���� �߻�");
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

