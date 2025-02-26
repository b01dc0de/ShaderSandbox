#ifndef SHADERCOMMON_HLSL
#define SHADERCOMMON_HLSL

#ifndef ENABLE_VERTEX_COLOR
    #define ENABLE_VERTEX_COLOR 0
#endif

#define _PI (3.14159265359)

cbuffer AtollGlobals : register(b0)
{
    float AppTime;
    float DeltaTime;
    int FrameWidth;
    int FrameHeight;
    int MouseX;
    int MouseY;
};

Texture2D DebugTexture : register(t0);
SamplerState MainSampler : register(s0);

struct VS_INPUT
{
    linear float4 Pos : POSITION;
    float2 TexUV : TEXCOORD;
    float4 RGBA : COLOR;
};

struct PS_INPUT
{
    float4 Pos : SV_Position;
    float2 TexUV : TEXCOORD;
#if ENABLE_VERTEX_COLOR
    float4 RGBA : COLOR;
#endif //ENABLE_VERTEX_COLOR
};

float4 PulseGradientTime(float4 ColorA, float4 ColorB, float Interval)
{
    uint EvenStep = ((uint)(AppTime / Interval)) % 2 == 0;
    float TimeWithinStep = AppTime % Interval;

    float4 CurrColor = EvenStep ? ColorA : ColorB;
    float4 NextColor = EvenStep ? ColorB : ColorA;

    return lerp(CurrColor, NextColor, TimeWithinStep / Interval);
}

#endif // SHADERCOMMON_HLSL