/////////////////////////////
// LiveShader_Wrapper.hlsl //
/////////////////////////////

#ifndef AUTO_LIVE
    #define AUTO_LIVE 0
#endif
#if !AUTO_LIVE
    #include "LiveShader.hlsl"
#else // AUTO_LIVE
float4 Main_Live(PS_INPUT Input)
{
    float4 ColorA = { 0.9, 0.9, 0.9, 1.0 };
    float4 ColorB = { 0.1, 0.1, 0.1, 1.0 };
    float TimeInterval = 1.0;

    return PulseGradientTime(ColorA, ColorB, TimeInterval);
}
#endif // AUTO_LIVE

