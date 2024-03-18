#include "SimpleLine.hlsli"

VSOutput main(float4 pos : POSITION, float4 color : COLOR, float thickness : THICKNESS)
{
    VSOutput output;
    
    output.pos = float4(pos.xy, 1.0f, 1.0f);
    output.color = color;
    output.thickness = thickness;
    return output;
}