#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    float2 uvPixelSize = float2(1 / 1280.0f, 1 / 720.0f);
    float4 finalColor = { 0, 0, 0, 0 };
    
    int sizeX = 10;
    int sizeY = 10;
	
    for (int y = 0; y <= sizeY; y++)
    {
        for (int x = 0; x <= sizeX; x++)
        {
            float4 texcolor = float4(tex.Sample(smp, input.uv + float2(-uvPixelSize.x + uvPixelSize.x * x, -uvPixelSize.y + uvPixelSize.y * y)));

            texcolor = texcolor * m_color;
            
            finalColor += texcolor;
        }
    }
    
    return float4(finalColor.rgb / (sizeX * sizeY), 1);
}