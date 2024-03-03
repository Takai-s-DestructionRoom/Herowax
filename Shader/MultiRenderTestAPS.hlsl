#include "MultiRenderTest.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

struct PSOutput
{
    float4 target0 : SV_Target0;
    float4 target1 : SV_Target1;
};

PSOutput main(OutputVS i)
{
    PSOutput output;
    
    float2 uvPixelSize = float2(1 / 1280.0f, 1 / 720.0f);
    float4 finalColor = { 0, 0, 0, 0 };
    
    int sizeX = 10;
    int sizeY = 10;
	
    for (int y = 0; y <= sizeY; y++)
    {
        for (int x = 0; x <= sizeX; x++)
        {
            float4 texcolor = float4(tex.Sample(smp, i.uv + float2(-uvPixelSize.x + uvPixelSize.x * x, -uvPixelSize.y + uvPixelSize.y * y)));
            
            finalColor += texcolor;
        }
    }
    
    output.target1 = float4(finalColor.rgb / (sizeX * sizeY), 1);
     
    float4 color = tex.Sample(smp, i.uv);
    output.target0 = float4(1 - color.rgb, 1);
    return output;
}