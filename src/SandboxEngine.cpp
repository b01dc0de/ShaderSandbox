#include "SandboxEngine.h"
#include "SandboxGraphics.h"
#if PLATFORM_WINDOWS()
	#include "Sandbox_mswin.h"
#endif // PLATFORM

namespace ShaderSandbox
{
	bool bRunning = false;
	HWND hWindow;
	double AppTime_s = 0.0f;
	double DeltaTime_ms = 0.0f;
	int WinResX = Defaults::WinResX;
	int WinResY = Defaults::WinResY;
	int MousePosX = Defaults::MousePosX;
	int MousePosY = Defaults::MousePosY;

	GlobalPlatformStateType GlobalPlatformState;

	// Local/'private' engine functions
	namespace AtollEngine
	{
		AtollClock Clock;
		bool bMaximized = false;

		bool Init();
		bool MainLoop();
		bool Term();

		void PollInput();
		void UpdateState();
		void Rest();

		void ToggleMaximize();
		void TriggerShaderRecompile();
		void SwitchActiveShader(bool bInc = true);
		void SwitchActiveExample(bool bInc = true);
	}

#if PLATFORM_WINDOWS()
	int AtollEngine::Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
#elif PLATFORM_OTHER()
	int AtollEngine::Run(int argc, const char* argv[])
#else
	int AtollEngine::Run()
#endif // PLATFORM
	{
#if PLATFORM_WINDOWS()
		GlobalPlatformState.hInstance = hInstance;
		GlobalPlatformState.hPrevInstance = hPrevInstance;
		GlobalPlatformState.lpCmdLine = lpCmdLine;
		GlobalPlatformState.nCmdShow = nCmdShow;
#elif PLATFORM_OTHER()
		GlobalPlatformState.argc = argc;
		GlobalPlatformState.argv = argv;
#endif // PLATFORM

		if (!Init()) { return (int)AtollErrorType::ERR_STARTUP; }

		if (!MainLoop()) { return (int)AtollErrorType::ERR_RUNTIME; }

		if (!Term()) { return (int)AtollErrorType::ERR_SHUTDOWN; }

		return (int)AtollErrorType::NO_ERR;
	}

#if PLATFORM_WINDOWS()
	LRESULT AtollEngine::HandleKeyInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		(void)wParam;
		(void)lParam;
		static bool bShift = false;
		LRESULT Result = 0;
		if (wParam == VK_SHIFT)
		{
			bShift = WM_KEYDOWN == uMsg;
		}
		else if (WM_KEYUP == uMsg)
		{
			switch (wParam)
			{
				case VK_ESCAPE:
				{
					ShaderSandbox::bRunning = false;
				} break;
				case VK_F11:
				{
					ToggleMaximize();
				} break;
				case VK_F2:
				{
					TriggerShaderRecompile();
				} break;
				case VK_TAB:
				{
					ATOLL_STATE_CHANGE DeltaState = bShift ? DEC_MODE : INC_MODE;
					AtollGraphics::ChangeState(DeltaState);
				} break;
				case VK_LEFT:
				{
					AtollGraphics::ChangeState(ATOLL_STATE_CHANGE::DEC_EXP);
				} break;
				case VK_RIGHT:
				{
					AtollGraphics::ChangeState(ATOLL_STATE_CHANGE::INC_EXP);
				} break;
			}
		}
		return Result;
	}
	LRESULT AtollEngine::HandleMouseInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		(void)wParam;
		LRESULT Result = 0;
		if (WM_MOUSEMOVE == uMsg)
		{
			/*
			bool bCtrl = (bool)(wParam & MK_CONTROL);
			bool bShift = (bool)(wParam & MK_SHIFT);
			bool bLB = (bool)(wParam & MK_LBUTTON);
			bool bMB = (bool)(wParam & MK_MBUTTON);
			bool bRB = (bool)(wParam & MK_RBUTTON);
			*/
			int PosX = GET_X_LPARAM(lParam);
			int PosY = GET_Y_LPARAM(lParam);

			ShaderSandbox::MousePosX = PosX;
			ShaderSandbox::MousePosY = PosY;
		}
		return Result;
	}
#elif PLATFORM_OTHER()
	void HandleKeyInput()
	{
	}
	void HandleMouseInput()
	{
	}
#endif // PLATFORM

	bool AtollEngine::Init()
	{
		bool bResult = true;

		if (HWND hWnd = InitWindow(GlobalPlatformState.hInstance))
		{
			ShaderSandbox::hWindow = hWnd;

			HRESULT Result = AtollGraphics::InitGraphics();
			if (!SUCCEEDED(Result))
			{
				DebugBreak();
			}

			ShowWindow(ShaderSandbox::hWindow, GlobalPlatformState.nCmdShow);

			Clock.Init();

			ShaderSandbox::bRunning = true;
		}
		else
		{
			bResult = false;
		}

		return bResult;
	}

	bool AtollEngine::MainLoop()
	{
		bool bResult = true;

		while (ShaderSandbox::bRunning)
		{
			// Update
			PollInput();
			UpdateState();

			// Draw
			AtollGraphics::Draw();

			// Rest
			Rest();
		}

		return bResult;
	}

	bool AtollEngine::Term()
	{
		bool bResult = true;
		bResult &= 0 == AtollGraphics::TermGraphics();
		return bResult;
	}

	void AtollEngine::PollInput()
	{
		// Get input
		MSG msg;
		BOOL MsgResult;
		while ((MsgResult = PeekMessage(&msg, ShaderSandbox::hWindow, 0, 0, PM_REMOVE)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (MsgResult == -1)
		{
			ShaderSandbox::bRunning = false;
		}
	}

	void AtollEngine::UpdateState()
	{
		Clock.Tick();
		AppTime_s = Clock.fLast_s;
		DeltaTime_ms = Clock.fDelta_ms;

		UpdateWindow(ShaderSandbox::hWindow);
	}

	void AtollEngine::Rest()
	{
		Clock.Rest();
	}

	void AtollEngine::ToggleMaximize()
	{
		bMaximized = !bMaximized;
		if (bMaximized)
		{
			WinResX = GlobalSysInfo.PrimaryWorkAreaX;
			WinResY = GlobalSysInfo.PrimaryWorkAreaY;
		}
		else
		{
			WinResX = Defaults::WinResX;
			WinResY = Defaults::WinResY;
		}
		UINT WndFlags = 0;
		SetWindowPos(ShaderSandbox::hWindow, HWND_TOP, 0, 0, WinResX, WinResY, WndFlags);
	}

	void AtollEngine::TriggerShaderRecompile()
	{
		AtollGraphics::RecompileShaders();
	}
}