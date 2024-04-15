#include "WaxMerge.hlsli"

Texture2D<float4> tex : register(t0);
Texture2D<float> depthtex : register(t1);
SamplerState smp : register(s0);

struct PS_OUT
{
    float4 color : SV_TARGET;
    float depth : SV_DEPTH;
};

struct HogeData
{
    float sigma;
    float stepwidth;
};
ConstantBuffer<HogeData> hoge : register(b0);

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

PS_OUT main(VSOUT input)
{
    PS_OUT output;
    
    float totalWeight = 0;
    float4 col;
    float depth;
    
    [loop]
    for (float py = -hoge.sigma * 2; py <= hoge.sigma * 2; py += hoge.stepwidth)
    {
        [loop]
        for (float px = -hoge.sigma * 2; px <= hoge.sigma * 2; px += hoge.stepwidth)
        {
            float2 pickUV = input.uv + float2(px, py);
            float weight = Gaussian(input.uv, pickUV, hoge.sigma);
            col += tex.Sample(smp, pickUV) * weight;
            depth += depthtex.Sample(smp, pickUV) * weight;
            totalWeight += weight;
        }
    }
    output.color = col / totalWeight;
    output.depth = depth / totalWeight;
    return output;
}