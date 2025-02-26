//////////////////////
// ErrorShader.hlsl //
//////////////////////

float4 Main_Error(PS_INPUT Input)
{
    float4 Color_HotPink = { 1.0, 0.286275, 0.678431, 1.0 };
    float4 Color_Black = 0.0;
    float TimeInterval = 1.0; // seconds

    return PulseGradientTime(Color_HotPink, Color_Black, TimeInterval);
}
