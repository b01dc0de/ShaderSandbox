#ifndef ATOLLGRAPHICS_H
#define ATOLLGRAPHICS_H

#include "SandboxCommon.h"
#include "SandboxGraphicsTypes.h"
#include "SandboxDrawState.h"

#define DXCHECK(Result) if (FAILED(Result)) { DebugBreak(); }
#define DXCHECKMSG(Result, Msg) if (FAILED(Result)) { OutputDebugStringA((Msg)); DebugBreak(); }

namespace ShaderSandbox
{
	struct AtollGraphics
	{
		static DXHandle<IDXGISwapChain1> DX_SwapChain1;
		static DXHandle<ID3D11Device> DX_Device;
		static DXHandle<ID3D11DeviceContext> DX_ImmediateContext;
		static DXHandle<ID3D11Texture2D> DX_BackBuffer;
		static DXHandle<ID3D11Texture2D> DX_RenderTargetTexture;
		static DXHandle<ID3D11RenderTargetView> DX_RenderTargetView;
		static DXHandle<IDXGIFactory2> DX_Factory2;
		static DXHandle<ID3D11RasterizerState> DX_RasterizerState;
		static DXHandle<ID3D11Texture2D> DX_DepthStencil;
		static DXHandle<ID3D11DepthStencilView> DX_DepthStencilView;
		static DXHandle<ID3D11BlendState> DX_BlendState;
		static DXHandle<ID3D11Buffer> DX_VertexBuffer;
		static DXHandle<ID3D11Buffer> DX_IndexBuffer;
		static DXHandle<ID3D11Buffer> DX_GlobalsBuffer;

		static DXHandle<ID3D11Texture2D> TestTexture;
		static DXHandle<ID3D11ShaderResourceView> TestTexture_SRV;
		static DXHandle<ID3D11SamplerState> TestSamplerState;


		static DrawStateManager DrawStateMgr;

		static int InitGraphics();
		static void UpdateGraphicsState();
		static void RecompileShaders();
		static void ChangeState(ATOLL_STATE_CHANGE DeltaState);
		static SHADER_MODE_TYPE GetCurrShaderMode();
		static void Draw();
		static int TermGraphics();
	};
}

#endif // ATOLLGRAPHICS_H