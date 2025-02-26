#include "LvBitmapFile.h"

#include <cstdio>

namespace Lv
{
	enum struct FallbackBitmapColors
	{
		COLOR_PINK,
		COLOR_BLACK,
		COLOR_RED,
		COLOR_GREEN,
		COLOR_BLUE,
		COLOR_WHITE
	};
	constexpr fRGBAColor fFallbackColors[] =
	{
		{ 1.0f, 73.0f / 255.0f, 173.0f / 255.0f, 1.0f }, //constexpr fRGBAColor Pink{ 1.0f, 73.0f / 255.0f, 173.0f / 255.0f, 1.0f };
		{ 0.0f, 0.0f, 0.0f, 1.0f }, //constexpr fRGBAColor Black{ 0.0f, 0.0f, 0.0f, 1.0f };
		{ 1.0f, 0.0f, 0.0f, 1.0f }, //constexpr fRGBAColor Red{ 1.0f, 0.0f, 0.0f, 1.0f };
		{ 0.0f, 1.0f, 0.0f, 1.0f }, //constexpr fRGBAColor Green{ 0.0f, 1.0f, 0.0f, 1.0f };
		{ 0.0f, 0.0f, 1.0f, 1.0f }, //constexpr fRGBAColor Blue{ 0.0f, 0.0f, 1.0f, 1.0f };
		{ 1.0f, 1.0f, 1.0f, 1.0f } //constexpr fRGBAColor White{ 1.0f, 1.0f, 1.0f, 1.0f };
	};
	constexpr RGBA32 uFallbackColors[] =
	{
		// CKA_TODO: These are pre-swizzled(?) for now to get correct result visually
		//			 Figure out why I'm not consistent with RGBA32's internal data representation
		{ 173, 73, 255, 255 },
		{ 0, 0, 0, 255 },
		{ 0, 0, 255, 255 },
		{ 0, 255, 0, 255 },
		{ 255, 0, 0, 255 },
		{ 255, 255, 255, 255 }
	};

	constexpr int FallbackTexture_NumCells = 16;
	constexpr int FallbackTexture_CellSize = FallbackTexture_Size / FallbackTexture_NumCells;
	constexpr int FallbackTexture_MaxCell = (FallbackTexture_Size - 1) / FallbackTexture_CellSize;

	FallbackBitmapColors GetPxColor(int CellX, int CellY)
	{
		bool bEvenCell = ((CellY + CellX) % 2 == 0);

		if (CellY == 0 && CellX == 0)
		{
			return FallbackBitmapColors::COLOR_RED;
		}
		else if (CellY == 0 && CellX == FallbackTexture_MaxCell)
		{
			return FallbackBitmapColors::COLOR_GREEN;
		}
		else if (CellY == FallbackTexture_MaxCell && CellX == 0)
		{
			return FallbackBitmapColors::COLOR_BLUE;
		}
		else if (CellY == FallbackTexture_MaxCell && CellX == FallbackTexture_MaxCell)
		{
			return FallbackBitmapColors::COLOR_WHITE;
		}
		else
		{
			return bEvenCell ? FallbackBitmapColors::COLOR_PINK : FallbackBitmapColors::COLOR_BLACK;
		}
	}

	void GetFallbackBitmapTexture(fRGBAColor** OutPxData)
	{
		fRGBAColor* FallbackTextureData = new fRGBAColor[FallbackTexture_Size * FallbackTexture_Size];
		{
			for (int RowIdx = 0; RowIdx < FallbackTexture_Size; RowIdx++)
			{
				int CellRow = RowIdx / FallbackTexture_CellSize;
				for (int ColIdx = 0; ColIdx < FallbackTexture_Size; ColIdx++)
				{
					int CellCol = ColIdx / FallbackTexture_CellSize;
					fRGBAColor& CurrPx = FallbackTextureData[(RowIdx * FallbackTexture_Size) + ColIdx];

					CurrPx = fFallbackColors[(int)GetPxColor(CellCol, CellRow)];
				}
			}
		}
		*OutPxData = FallbackTextureData;
	}

	void GetFallbackBitmapTexture(RGBA32** OutPxData)
	{
		RGBA32* FallbackTextureData = new RGBA32[FallbackTexture_Size * FallbackTexture_Size];
		{
			for (int RowIdx = 0; RowIdx < FallbackTexture_Size; RowIdx++)
			{
				int CellRow = RowIdx / FallbackTexture_CellSize;
				for (int ColIdx = 0; ColIdx < FallbackTexture_Size; ColIdx++)
				{
					int CellCol = ColIdx / FallbackTexture_CellSize;

					RGBA32& CurrPx = FallbackTextureData[(RowIdx * FallbackTexture_Size) + ColIdx];
					CurrPx = uFallbackColors[(int)GetPxColor(CellCol, CellRow)];
				}
			}
		}
		*OutPxData = FallbackTextureData;
	}

	constexpr u16 BitmapFileTypeValue = 0x4D42;
	constexpr u16 SupportedBPP = 32;

	void WriteBMP(const char* OutFilename, const Image32& InImage)
	{
		u32 PxBytes = sizeof(u32) * InImage.PxCount;

		BMP BMP_Data = {};

		BMP_Data.FileHeader.Type = BitmapFileTypeValue;
		BMP_Data.FileHeader.SizeInBytes = sizeof(BMP_Data) + PxBytes;
		BMP_Data.FileHeader.Res1 = 0;
		BMP_Data.FileHeader.Res2 = 0;
		BMP_Data.FileHeader.OffsetBytes = sizeof(BMP_Data);

		BMP_Data.InfoHeader.StructSize = sizeof(BMP_Data) - 14;
		BMP_Data.InfoHeader.Width = (s32)InImage.Width;
		BMP_Data.InfoHeader.Height = -(s32)InImage.Height;
		BMP_Data.InfoHeader.Planes = 1;
		BMP_Data.InfoHeader.BitsPerPixel = 32;
		BMP_Data.InfoHeader.Compression = 0;
		BMP_Data.InfoHeader.Unused_ImgSize = PxBytes;
		BMP_Data.InfoHeader.HRes = 0;
		BMP_Data.InfoHeader.VRes = 0;
		BMP_Data.InfoHeader.ColorsUsed = 0;
		BMP_Data.InfoHeader.ColorsImportant = 0;

		FILE* BMP_File = nullptr;
		fopen_s(&BMP_File, OutFilename, "wb");
		if (BMP_File != nullptr)
		{
			fwrite(&BMP_Data.FileHeader, sizeof(BMP_Data.FileHeader), 1, BMP_File);
			fwrite(&BMP_Data.InfoHeader, sizeof(BMP_Data.InfoHeader), 1, BMP_File);
			fwrite(InImage.PixelBuffer, InImage.PxBytes, 1, BMP_File);
			fclose(BMP_File);
		}
		else
		{
			printf("ERROR: Cannot open file \"%s\" for write\n", OutFilename);
		}
	}

	void ReadBMP(const char* InFilename, Image32& OutImage)
	{
		FILE* BMP_File = nullptr;
		fopen_s(&BMP_File, InFilename, "rb");

		if (nullptr != BMP_File)
		{
			
			// Get file size in bytes
			fpos_t FileSizeBytes;
			{
				int Result = fseek(BMP_File, 0, SEEK_END);
				// CKA_TODO: Assert Result == 0
				fgetpos(BMP_File, &FileSizeBytes);

				// Set curr pos to beginning of file
				Result = fseek(BMP_File, 0, SEEK_SET);
			}

			// Parse .BMP file into BMP struct
			BMP ReadBMP = {};
			fread_s(&ReadBMP, sizeof(BMP), sizeof(BMP), 1, BMP_File);

			if (SupportedBPP != ReadBMP.InfoHeader.BitsPerPixel)
			{
				*(int*)0 = 1;
			}

			if (BitmapFileTypeValue == ReadBMP.FileHeader.Type)
			{
				using ullong = unsigned long long;

				// Extract pixel data into OutImage struct
				OutImage.Width = ReadBMP.InfoHeader.Width;
				OutImage.Height = ~ReadBMP.InfoHeader.Height + 1; // Two's complement
				OutImage.PxCount = OutImage.Width * OutImage.Height;
				OutImage.PxBytes = u32(FileSizeBytes - sizeof(BMP));
				OutImage.PixelBuffer = (RGBA32*)(new u8[OutImage.PxBytes]);

				fread_s(OutImage.PixelBuffer, OutImage.PxBytes, sizeof(u32), OutImage.PxCount, BMP_File);

				// Unswizzle(?) the pixel data
				for (unsigned int PxIdx = 0; PxIdx < OutImage.PxCount; PxIdx++)
				{
					RGBA32& CurrPx = OutImage.PixelBuffer[PxIdx];
					CurrPx = RGBA32::Unswizzle(CurrPx);
				}
			}
			else
			{
				// CKA_TODO: Error handling
			}
		}
		else
		{
			// CKA_TODO: Error handling
		}

	}

}

