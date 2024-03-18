#include "GaussianBlur.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

cbuffer ConstBufferData : register(b0)
{
    float sigma;
    float stepwidth;
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
    for (float py = -sigma * 2; py <= sigma * 2; py += stepwidth)
    {
        [loop]
        for (float px = -sigma * 2; px <= sigma * 2; px += stepwidth)
        {
            float2 pickUV = i.uv + float2(px, py);
            float weight = Gaussian(i.uv, pickUV, sigma);
            col += tex.Sample(smp, pickUV) * weight;
            totalWeight += weight;
        }
    }
    col.rgb = col.rgb / totalWeight;
    col.a = 1;
    return col;
}