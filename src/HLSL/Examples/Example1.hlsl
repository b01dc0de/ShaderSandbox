///////////////////
// Example1.hlsl //
///////////////////

float4 Main_Example1(PS_INPUT Input)
{
    float2 CirclingUVs = float2(Input.TexUV.x + cos(AppTime*5)*.1, Input.TexUV.y + sin(AppTime*5)*.1);
    return (DebugTexture.Sample(MainSampler, CirclingUVs) + Example_Gradient(Input.Pos));
}

