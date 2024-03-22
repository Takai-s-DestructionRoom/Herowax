#include "ParticleBillboard.hlsli"

VSOutput main(float4 pos : POSITION, float3 rot : ROT
,float4 color : COLOR, float scale : TEXCOORD,float timer : TIMER)
{
	VSOutput output; // ピクセルシェーダーに渡す値
	output.pos = pos;
	output.rot = rot;
    output.color = color;
    output.scale = scale;
    output.timer = timer;
	return output;
}