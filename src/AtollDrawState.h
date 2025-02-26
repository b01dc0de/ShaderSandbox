#ifndef ATOLLDRAWPIPELINESTATE_H
#define ATOLLDRAWPIPELINESTATE_H

#include "AtollCommon.h"
#include "AtollGraphicsTypes.h"

namespace ShaderAtoll
{
	struct DrawPipelineState
	{
		ID3D11VertexShader* VertexShader = nullptr;
		ID3D11PixelShader* PixelShader = nullptr;
		ID3D11InputLayout* InputLayout = nullptr;
		bool bValid = false;

		void Release()
		{
			if (VertexShader) { VertexShader->Release(); VertexShader = nullptr; }
			if (PixelShader) { PixelShader->Release(); PixelShader = nullptr; }
			if (InputLayout) { InputLayout->Release(); InputLayout = nullptr; }
			bValid = false;
		}
	};

	struct DrawStateManager
	{
		DrawPipelineState LiveState = {};
		DrawPipelineState ErrorState = {};
		DrawPipelineState ExampleState = {};

		static constexpr int NumExamples = 10;
		DrawPipelineState* CurrActive_DrawState = nullptr;
		int SelectedExampleNum = 1;

		ID3D11Device* CachedDevice = nullptr;

		void Init(ID3D11Device* InDevice);
		void Term();
		DrawPipelineState* GetCurrState();
		SHADER_MODE_TYPE GetCurrMode();
		void ChangeState(ATOLL_STATE_CHANGE DeltaState);
		void RecompileState();
	};

	bool CompileDrawPipeline(
		ID3D11Device* InDevice,
		LPCWSTR SrcFilename,
		const D3D_SHADER_MACRO* MacroDefines,
		const D3D11_INPUT_ELEMENT_DESC* InputLayoutDesc,
		uint NumInputElements,
		DrawPipelineState* OutDrawPipelineState
	);
}

#endif // ATOLLGPUPIPELINESTATE_H