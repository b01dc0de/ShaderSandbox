#include "Sandbox_mswin.h"
#include "SandboxGraphics.h"
#include "SandboxEngine.h"

namespace ShaderSandbox
{
	HWND InitWindow(HINSTANCE hInstance)
	{
		WNDCLASSEX WndClass = {};
		WndClass.cbSize = sizeof(WNDCLASSEX);
		WndClass.style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW;
		WndClass.lpfnWndProc = WindowProc;
		WndClass.hInstance = hInstance;
		WndClass.lpszClassName = APPNAME();

		//ATOM RegClassResult = 
		RegisterClassEx(&WndClass);

		RECT WndRect = { 0, 0, (LONG)ShaderSandbox::WinResX, (LONG)ShaderSandbox::WinResY };
		UINT WndStyle = WS_CAPTION;
		UINT WndExStyle = WS_EX_OVERLAPPEDWINDOW;
		AdjustWindowRectEx(&WndRect, WndStyle, FALSE, WndExStyle);

		HWND NewWindow = CreateWindowEx(
			WndExStyle,
			APPNAME(),
			APPNAME(),
			WndStyle,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			WndRect.right - WndRect.left,
			WndRect.bottom - WndRect.top,
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);

		return NewWindow;
	}

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
#define ENABLE_EXTENDED_MOUSE_INPUT() (0)
		LRESULT Result = 0;
		switch (uMsg)
		{
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				Result = AtollEngine::HandleKeyInput(uMsg, wParam, lParam);
			} break;
#if ENABLE_EXTENDED_MOUSE_INPUT()
			case WM_LBUTTONDBLCLK:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_MBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_RBUTTONDBLCLK:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MOUSEWHEEL:
			case WM_MOUSEACTIVATE:
			case WM_MOUSEHOVER:
			case WM_MOUSEHWHEEL:
			case WM_MOUSELEAVE:
#endif // ENABLE_EXTENDED_MOUSE_INPUT
			case WM_MOUSEMOVE:
			{
				Result = AtollEngine::HandleMouseInput(uMsg, wParam, lParam);
			}
			default:
			{
				Result = DefWindowProc(hwnd, uMsg, wParam, lParam);
			} break;
		}

		return Result;
	}

	int WINAPI WinMain
	(
		HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		PSTR lpCmdLine,
		int nCmdShow
	)
	{
		return AtollEngine::Run(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	}
}
