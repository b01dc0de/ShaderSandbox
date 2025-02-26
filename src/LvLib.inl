#ifndef LVLIB_INL
#define LVLIB_INL

namespace Lv
{
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

#endif // LVLIB_INL