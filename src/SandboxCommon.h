#ifndef SANDBOXCOMMON_H
#define SANDBOXCOMMON_H

// Platform
#define PLATFORM_WINDOWS() (1)
#define PLATFORM_OTHER() (!(PLATFORM_WINDOWS()))
// Build configuration
#define BUILD_DEBUG() (_DEBUG)
#define BUILD_RELEASE() (!(BUILD_DEBUG()))

// C/C++ Std Lib
#include <cstdio>
#include <vector>

#if PLATFORM_WINDOWS()
// Win32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>
// DX11
#include <d3d11.h>
#include <d3d11_2.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>
#endif // PLATFORM_WINDOWS()

using uchar = unsigned char;
using uint = unsigned int;

#include "SandboxEngine.h"
#include "SandboxUtil.h"

#endif  // SANDBOXCOMMON_H

