#ifndef ATOLL_MSWIN_H
#define ATOLL_MSWIN_H

#include "SandboxCommon.h"

namespace ShaderSandbox
{
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow);

	HWND InitWindow(HINSTANCE hInstance);
}

#endif // ATOLL_MSWIN_H