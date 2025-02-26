////////////////////////
// ExampleCommon.hlsl //
////////////////////////

float4 Lerp_DistToMouse(float4 ScreenPos, float DistThreshold, float4 InColor)
{
    float4 Result = InColor;
    float DistToMouseX = (MouseX - ScreenPos.x);
    float DistToMouseY = (MouseY - ScreenPos.y);
    float DistFromMouse = sqrt(DistToMouseX*DistToMouseX + DistToMouseY*DistToMouseY);
    if (DistFromMouse < DistThreshold)
    {
        float BlendFactor = (DistThreshold - DistFromMouse) / DistThreshold;
        float4 Color_White = 1.0;
        Result = lerp(Result, Color_White, BlendFactor);
    }
    return Result;
}

float4 Example_Gradient(float4 ScreenPos)
{
    const float PI_DIV_3 = _PI / 3.0;
    float4 Result =
    {
        cos(AppTime) * (ScreenPos.x / FrameWidth),
        (cos(AppTime - _PI) + sin(AppTime + _PI)) / 2.0 * (ScreenPos.x + ScreenPos.y) / (FrameWidth + FrameHeight),
        sin((AppTime * 2.0) - _PI) * ScreenPos.y / FrameHeight,
        1.0
    };

    return Lerp_DistToMouse(ScreenPos, 50.0, Result);
}
