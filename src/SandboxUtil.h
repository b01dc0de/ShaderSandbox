#ifndef ATOLLUTIL_H
#define ATOLLUTIL_H

#include "SandboxCommon.h"

#if UNICODE
	#define APPNAME() (L"ShaderSandbox")
#else // UNICODE
	#define APPNAME() ("ShaderSandbox")
#endif // UNICODE

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))
#define CHECK(Exp) if (!(Exp)) { OutputDebugStringA(#Exp); DebugBreak(); }

namespace ShaderSandbox
{
	namespace Consts
	{
		static constexpr bool bUnicode = (bool)UNICODE;
	};

	namespace Defaults
	{
		static constexpr UINT WinResX = 1024U;
		static constexpr UINT WinResY = 1024U;
		static constexpr int MousePosX = -1;
		static constexpr int MousePosY = -1;
	};

	struct SysInfo
	{
		// Primary display resolution
		UINT PrimaryResX = 0;
		UINT PrimaryResY = 0;

		// Display resolution without the taskbar, toolbars, etc.
		UINT PrimaryWorkAreaX = 0;
		UINT PrimaryWorkAreaY = 0;

		SysInfo();
		SysInfo(const SysInfo&) = delete;
		SysInfo& operator=(const SysInfo&) = delete;
		~SysInfo() = default;
	};

	extern SysInfo GlobalSysInfo;

	struct AtollClock
	{
		static constexpr __int64 Ticks_to_us = 100000; // ticks -> microseconds
		static constexpr __int64 us_per_ms = 1000;
		static constexpr __int64 ms_per_s = 1000;

		using TsType = LARGE_INTEGER;
		static TsType Freq;

		TsType T0 = {};
		TsType Ts_Last = {};
		TsType Ts_Delta = {};
		double fLast_s = 0.0;
		double fDelta_ms = 0.0;

		static double ConvertTs_s(TsType InTs);
		void Tick();
		void Rest();
		void Init();
	};

	template <typename T>
	struct DynArray
	{
		static constexpr int Default_InitCapacity = 32;
		static constexpr float Default_IncFactor = 2.0f;

		T* Data = nullptr;
		int Capacity = Default_InitCapacity;
		int NumItems = 0;
		const float IncFactor = Default_IncFactor;

		DynArray(int InitCapacity = Default_InitCapacity, float InIncFactor = Default_IncFactor)
			: Data(new T[InitCapacity])
			, Capacity(InitCapacity)
			, NumItems(0)
			, IncFactor(InIncFactor)
		{
		};
		~DynArray()
		{
			if (Data)
			{
				delete[] Data;
				Data = nullptr;
			}
		};
		DynArray(DynArray&& Other)
		{
			this->operator=(Other);
		};
		DynArray& operator=(DynArray&& Other)
		{
			if (Data)
			{
				delete[] Data;
				Data = nullptr;
			}

			Data = Other.Data;
			Capacity = Other.Capacity;
			NumItems = Other.NumItems;

			Other.Data = nullptr;
			Other.Capacity = -1;
			Other.NumItems = -1;
		};
		DynArray(const DynArray&) = delete;
		DynArray& operator=(const DynArray&) = delete;

		void Reset()
		{
			NumItems = 0;
		}
		void AddItem(const T& NewItem)
		{
			if (NumItems + 1 > Capacity)
			{
				Inc();
			}
			Data[NumItems++] = NewItem;
		}
		void AddItem(T&& MoveItem)
		{
			if (NumItems + 1 > Capacity)
			{
				Inc();
			}
			Data[NumItems++] = MoveItem;
		}
		void Inc()
		{
			Inc(int(Capacity * IncFactor));
		}
		void Inc(int NewSize)
		{
			if (NewSize > Capacity)
			{
				T* OldData = Data;
				Data = new T[NewSize];
				Capacity = NewSize;

				for (int ItemIdx = 0; ItemIdx < NumItems; ItemIdx++)
				{
					Data[ItemIdx] = OldData[ItemIdx];
				}

				delete[] OldData;
			}
			else
			{
				DebugBreak();
			}
		}
		T& operator[](int Idx)
		{
			// CKA_TODO: Range check
			return Data[Idx];
		}
		const T& operator[](int Idx) const
		{
			// CKA_TODO: Range check
			return Data[Idx];
		}
	};
}

#endif // ATOLLUTIL_H
