#include "SandboxCommon.h"
#include "Sandbox_mswin.h"

#if PLATFORM_WINDOWS()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	int ExitCode = ShaderSandbox::WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	return ExitCode;
}
#elif PLATFORM_OTHER()
int main(int argc, const char* argv[])
{
	return 0;
}
#endif // PLATFORM
