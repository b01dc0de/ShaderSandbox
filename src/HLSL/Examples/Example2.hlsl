///////////////////
// Example2.hlsl //
///////////////////

float4 Main_Example2(PS_INPUT Input)
{
    float2 ScrolledUV = float2(Input.TexUV.x + (AppTime / 5.0), Input.TexUV.y + (AppTime / 2.0));
    return DebugTexture.Sample(MainSampler, ScrolledUV) - Example_Gradient(Input.Pos);
}

