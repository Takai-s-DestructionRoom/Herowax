#include "DepthTest.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
Texture2D<float> depthTex : register(t1); //1番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

cbuffer DepthTestBuffer : register(b0)
{
    bool useDepthColor;
    float focusDepth;
    float nearFocusWidth;
    float farFocusWidth;
};

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 GaussianBlur(float2 uv, float sigma, float stepWidth, float focusWidth)
{
    float totalWeight = 0;
    float4 col = float4(0, 0, 0, 1);
    
    for (float py = -sigma * 2; py <= sigma * 2; py += stepWidth)
    {
        for (float px = -sigma * 2; px <= sigma * 2; px += stepWidth)
        {
            float2 pickUV = uv + float2(px, py);
            float pickDepth = depthTex.Sample(smp, pickUV);
            float pickFocus = smoothstep(0, focusWidth, abs(pickDepth - focusDepth));
            
            float weight = Gaussian(uv, pickUV, sigma) * pickFocus;
            col += tex.Sample(smp, pickUV) * weight;
            totalWeight += weight;
        }
    }
    
    col.rgb = col.rgb / totalWeight;
    return col;
}

float4 main(OutputVS i) : SV_TARGET
{
    float depth = depthTex.Sample(smp, i.uv);
    float inFocus = 1 - smoothstep(0, nearFocusWidth, abs(depth - focusDepth));
    float outFocus = smoothstep(nearFocusWidth, farFocusWidth, abs(depth - focusDepth));
    float middleFocus = 1 - inFocus - outFocus;
    float4 inFocusColor = tex.Sample(smp, i.uv);
    float4 middleFocusColor = GaussianBlur(i.uv, 0.001, 0.0005, nearFocusWidth);
    float4 outFocusColor = GaussianBlur(i.uv, 0.005, 0.001, farFocusWidth);
    if (useDepthColor)
    {
        inFocusColor = float4(1, 0, 0, 1);
        middleFocusColor = float4(0, 1, 0, 1);
        outFocusColor = float4(0, 0, 1, 1);
    }
    return inFocus * inFocusColor + middleFocus * middleFocusColor + outFocus * outFocusColor;
}