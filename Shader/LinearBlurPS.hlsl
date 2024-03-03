#include "LinearBlur.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

cbuffer ConstBufferData : register(b0)
{
    float angle;
    float pickrange = 0.06;
};

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 main(OutputVS i) : SV_TARGET
{
    float totalWeight = 0;
    float4 col;
    
    [loop]
    for (float j = -pickrange; j <= pickrange; j += 0.0005)
    {
        float x = cos(angle) * j;
        float y = sin(angle) * j;
        float2 pickUV = i.uv + float2(x, y);
        
        float weight = Gaussian(i.uv, pickUV, pickrange);
        col += tex.Sample(smp, pickUV) * weight;
        totalWeight += weight;
    }
    col.rgb = col.rgb / totalWeight;
    col.a = 1;
    return col;
}