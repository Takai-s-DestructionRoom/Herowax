#include "MultiRenderTest.hlsli"

Texture2D<float4> tex0 : register(t0); //0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); //1番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(OutputVS i) : SV_TARGET
{
    float4 color0 = tex0.Sample(smp, i.uv);
    float4 color1 = tex1.Sample(smp, i.uv);
    
    float4 color = color0;
    if (fmod(i.uv.y, 0.1f) < 0.05f)
    {
        color = color1;
    }
    
    return color;
}