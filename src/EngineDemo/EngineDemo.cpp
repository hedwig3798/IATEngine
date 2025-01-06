// EngineDemo.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "EngineDemo.h"
#include "Processor.h"

#define EDITOR_MODE

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	std::unique_ptr<Processor> processor = std::make_unique<Processor>();

	processor->Initialize(hInstance);

	processor->Loop();

	return 0;
}
