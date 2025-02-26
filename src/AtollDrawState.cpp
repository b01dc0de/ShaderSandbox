#include "AtollDrawState.h"
#include "AtollGraphics.h"

namespace ShaderAtoll
{
	namespace ShaderUtils
	{
		LPCWSTR Base_ShaderFilename = L"src/HLSL/BaseShader.hlsl";

		D3D11_INPUT_ELEMENT_DESC VxUVColor_InputLayoutDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT VxUVColor_NumInputElements = ARRAYSIZE(VxUVColor_InputLayoutDesc);

		bool CompileShader(ID3D11Device* Device, const D3D_SHADER_MACRO* InMacroDefines, DrawPipelineState& Out_DrawState)
		{
			DrawPipelineState TmpDrawState = {};
			bool bResult = CompileDrawPipeline
			(
				Device,
				Base_ShaderFilename,
				InMacroDefines,
				VxUVColor_InputLayoutDesc,
				VxUVColor_NumInputElements,
				&TmpDrawState
			);
			if (TmpDrawState.VertexShader && TmpDrawState.InputLayout && TmpDrawState.PixelShader)
			{
				Out_DrawState = TmpDrawState;
			}
			Out_DrawState.bValid = bResult;
			return bResult;
		}

		bool CompileErrorShader(ID3D11Device* Device, DrawPipelineState& Out_ErrorDrawState)
		{
			static constexpr D3D_SHADER_MACRO HLSL_MacroDefines[] =
			{
				"ENABLE_VERTEX_COLOR", "0",
				"SELECT_ERROR_SHADER", "1",
				"SELECT_LIVE_SHADER", "0",
				"SELECT_EXAMPLE_SHADER", "0",
				NULL, NULL
			};

			bool bResult = CompileShader(Device, HLSL_MacroDefines, Out_ErrorDrawState);
			CHECK(bResult);

			return bResult;
		}

		bool CompileLiveShader(ID3D11Device* Device, bool bAutoLive, DrawPipelineState& Out_LiveDrawState)
		{
			D3D_SHADER_MACRO HLSL_MacroDefines[] =
			{
				"ENABLE_VERTEX_COLOR", "0",
				"SELECT_ERROR_SHADER", "0",
				"SELECT_LIVE_SHADER", "1",
				"SELECT_EXAMPLE_SHADER", "0",
				"AUTO_LIVE", bAutoLive ? "1" : "0",
				NULL, NULL
			};

			bool bResult = CompileShader(Device, HLSL_MacroDefines, Out_LiveDrawState);
			return bResult;
		}

		bool CompileExampleShader(ID3D11Device* Device, int ExampleNum, DrawPipelineState& Out_ExampleDrawState)
		{
			static constexpr int ExampleMacroIdx = 3;
			static constexpr int DefaultExampleNum = 1;

			D3D_SHADER_MACRO HLSL_MacroDefines[] =
			{
				"ENABLE_VERTEX_COLOR", "0",
				"SELECT_ERROR_SHADER", "0",
				"SELECT_LIVE_SHADER", "0",
				"SELECT_EXAMPLE_SHADER", NULL,
				NULL, NULL
			};
			static LPCSTR SelectedExampleDefVals[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
			int SelectedExampleNum = DefaultExampleNum;
			if (0 <= ExampleNum && ExampleNum <= 9)
			{
				SelectedExampleNum = ExampleNum;
			}
			HLSL_MacroDefines[ExampleMacroIdx].Definition = SelectedExampleDefVals[SelectedExampleNum];

			bool bResult = CompileShader(Device, HLSL_MacroDefines, Out_ExampleDrawState);
			CHECK(bResult);

			return bResult;
		}
	}


	void DrawStateManager::Init(ID3D11Device* InDevice)
	{
		CachedDevice = InDevice;
		SelectedExampleNum = 1;

		bool bShaderCompiled = ShaderUtils::CompileErrorShader
		(
			CachedDevice,
			ErrorState
		);
		if (!bShaderCompiled)
		{
			DebugBreak();
		}

		bShaderCompiled = ShaderUtils::CompileLiveShader
		(
			CachedDevice,
			true,
			LiveState
		);
		if (!bShaderCompiled) { DebugBreak(); }

		bShaderCompiled = ShaderUtils::CompileExampleShader
		(
			CachedDevice,
			SelectedExampleNum,
			ExampleState
		);
		if (!bShaderCompiled) { DebugBreak(); }

		CurrActive_DrawState = &ExampleState;
	}

	void DrawStateManager::Term()
	{
		LiveState.Release();
		ErrorState.Release();
		ExampleState.Release();
	}

	DrawPipelineState* DrawStateManager::GetCurrState()
	{
		return CurrActive_DrawState;
	}

	SHADER_MODE_TYPE DrawStateManager::GetCurrMode()
	{
		if (CurrActive_DrawState == &LiveState)
		{
			return SHADER_MODE_LIVE;
		}
		else if (CurrActive_DrawState == &ErrorState)
		{
			return SHADER_MODE_ERROR;
		}
		else if (CurrActive_DrawState == &ExampleState)
		{
			return SHADER_MODE_EXAMPLES;
		}
		return SHADER_MODE_ERROR;
	}

	void DrawStateManager::ChangeState(ATOLL_STATE_CHANGE DeltaState)
	{
		SHADER_MODE_TYPE ActiveMode = GetCurrMode();

		switch (DeltaState)
		{
			case INC_MODE:
			{
				ActiveMode = (SHADER_MODE_TYPE)((ActiveMode + 1) % SHADER_MODE_NUM);
			} break;
			case DEC_MODE:
			{
				ActiveMode = (SHADER_MODE_TYPE)((ActiveMode - 1) % SHADER_MODE_NUM);
			} break;
			case INC_EXP:
			{
				SelectedExampleNum = (SelectedExampleNum + 1) % NumExamples;
			} break;
			case DEC_EXP:
			{
				SelectedExampleNum = (SelectedExampleNum - 1) % NumExamples;
			} break;
		}

		bool bRecompile = SHADER_MODE_ERROR != ActiveMode;

		if (ActiveMode == SHADER_MODE_LIVE) { CurrActive_DrawState = &LiveState; }
		else if (ActiveMode == SHADER_MODE_EXAMPLES) { CurrActive_DrawState = &ExampleState; }
		else if (ActiveMode == SHADER_MODE_ERROR) { CurrActive_DrawState = &ErrorState; }

		if (bRecompile)
		{
			RecompileState();
		}
	}

	void DrawStateManager::RecompileState()
	{
		bool bResult = false;

		DrawPipelineState Old_DrawState = *CurrActive_DrawState;

		if (CurrActive_DrawState == &LiveState)
		{
			static bool bAutoLive = true;
			bResult = ShaderUtils::CompileLiveShader(CachedDevice, bAutoLive, LiveState);
			bAutoLive = false;
		}
		else if (CurrActive_DrawState == &ErrorState)
		{
			bResult = ShaderUtils::CompileErrorShader(CachedDevice, ErrorState);
		}
		else if (CurrActive_DrawState == &ExampleState)
		{
			bResult = ShaderUtils::CompileExampleShader(CachedDevice, SelectedExampleNum, ExampleState);
		}
		else { CHECK(false); return; }

		if (bResult)
		{
			Old_DrawState.Release();
		}
		else
		{
			CurrActive_DrawState->Release();
			*CurrActive_DrawState = Old_DrawState;
			CurrActive_DrawState = &ErrorState;
		}
	}

	int CompileShaderHelper(LPCWSTR SourceFileName, LPCSTR EntryPointFunction, LPCSTR Profile, const D3D_SHADER_MACRO* MacroDefines, ID3DBlob** ShaderBlob)
	{
		HRESULT Result = S_OK;

		if (SourceFileName == nullptr || EntryPointFunction == nullptr || Profile == nullptr || ShaderBlob == nullptr)
		{
			return E_INVALIDARG;
		}

		*ShaderBlob = nullptr;

		UINT CompileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#if BUILD_DEBUG()
		CompileFlags |= D3DCOMPILE_ALL_RESOURCES_BOUND;
		CompileFlags |= D3DCOMPILE_DEBUG;
		CompileFlags |= D3DCOMPILE_DEBUG_NAME_FOR_SOURCE;
		CompileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // BUILD_DEBUG()

		ID3DBlob* OutBlob = nullptr;
		ID3DBlob* ErrorMsgBlob = nullptr;

		Result = D3DCompileFromFile
		(
			SourceFileName,
			MacroDefines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			EntryPointFunction,
			Profile,
			CompileFlags,
			0, //UINT Flags2
			&OutBlob,
			&ErrorMsgBlob
		);

		if (FAILED(Result) && OutBlob)
		{
			OutBlob->Release();
			OutBlob = nullptr;
		}
		if (ErrorMsgBlob)
		{
			OutputDebugStringA((char*)ErrorMsgBlob->GetBufferPointer());
			ErrorMsgBlob->Release();
		}

		*ShaderBlob = OutBlob;

		return Result;
	};

	namespace ShaderCompileDefaults
	{
		LPCSTR VS_Model= "vs_5_0";
		LPCSTR PS_Model = "ps_5_0";
		LPCSTR VS_Main = "VSMain";
		LPCSTR PS_Main = "PSMain";
	}

	int CompileVertexShaderInputLayout_Defaults
	(
		ID3D11Device* InDevice,
		LPCWSTR SrcFilename,
		const D3D_SHADER_MACRO* MacroDefines,
		const D3D11_INPUT_ELEMENT_DESC* InputLayoutDesc,
		uint NumInputElements,
		ID3D11VertexShader** Out_VS,
		ID3D11InputLayout** Out_InputLayout
	)
	{
		ID3DBlob* VSCodeBlob = nullptr;
		int Result = CompileShaderHelper(
			SrcFilename,
			ShaderCompileDefaults::VS_Main,
			ShaderCompileDefaults::VS_Model,
			MacroDefines,
			&VSCodeBlob);
		DXCHECKMSG(VSCodeBlob && Result, "Failed to compile Vertex Shader! :(\n");

		CHECK(InDevice);
		Result = InDevice->CreateVertexShader(VSCodeBlob->GetBufferPointer(), VSCodeBlob->GetBufferSize(), nullptr, Out_VS);
		DXCHECKMSG(Result, "Device could not create vertex shader! :(\n");

		Result = InDevice->CreateInputLayout(InputLayoutDesc, NumInputElements, VSCodeBlob->GetBufferPointer(), VSCodeBlob->GetBufferSize(), Out_InputLayout);
		DXCHECKMSG(Result, "Device could not create input layout! :(\n");

		if (VSCodeBlob) { VSCodeBlob->Release(); }

		return SUCCEEDED(Result);
	}

	int CompilePixelShader_Defaults
	(
		ID3D11Device* InDevice,
		LPCWSTR SrcFilename,
		const D3D_SHADER_MACRO* MacroDefines,
		ID3D11PixelShader** Out_PS
	)
	{
		ID3DBlob* PSCodeBlob = nullptr;
		int Result = Result = CompileShaderHelper(
			SrcFilename,
			ShaderCompileDefaults::PS_Main,
			ShaderCompileDefaults::PS_Model,
			MacroDefines,
			&PSCodeBlob);
		DXCHECKMSG(PSCodeBlob && Result, "Failed to compile Pixel Shader! :(\n");

		CHECK(InDevice);
		Result = InDevice->CreatePixelShader(PSCodeBlob->GetBufferPointer(), PSCodeBlob->GetBufferSize(), nullptr, Out_PS);
		DXCHECKMSG(Result, "Device could not create pixel shader! :(\n");

		if (PSCodeBlob) { PSCodeBlob->Release(); }

		return SUCCEEDED(Result);
	}

	bool CompileDrawPipeline
	(
		ID3D11Device* InDevice,
		LPCWSTR SrcFilename,
		const D3D_SHADER_MACRO* MacroDefines,
		const D3D11_INPUT_ELEMENT_DESC* InputLayoutDesc,
		uint NumInputElements,
		DrawPipelineState* OutDrawPipelineState
	)
	{
		CHECK(InDevice);

		OutDrawPipelineState->VertexShader = nullptr;
		OutDrawPipelineState->PixelShader = nullptr;
		OutDrawPipelineState->InputLayout = nullptr;

		HRESULT Result = CompileVertexShaderInputLayout_Defaults(
			InDevice,
			SrcFilename,
			MacroDefines,
			InputLayoutDesc,
			NumInputElements,
			&OutDrawPipelineState->VertexShader,
			&OutDrawPipelineState->InputLayout
		);
		DXCHECK(Result);

		Result = CompilePixelShader_Defaults(
			InDevice,
			SrcFilename,
			MacroDefines,
			&OutDrawPipelineState->PixelShader);
		DXCHECK(Result);

		return SUCCEEDED(Result);
	}
}