#include "Test.hlsli"

struct VSInput
{
    float4 pos : POSITION0;
    float2 uv : TEXCOORD;
    float4 apos : POSITION1;
    uint instanceID : SV_InstanceID;
};

VSOutput main(VSInput input)
{
	VSOutput output; // ピクセルシェーダーに渡す値

    output.svpos = float4((input.pos + input.apos).xyz, 1.0f);
    output.svpos = mul(matViewProjection, output.svpos); //座標に行列を乗算
    output.uv = input.uv;
	return output;
}