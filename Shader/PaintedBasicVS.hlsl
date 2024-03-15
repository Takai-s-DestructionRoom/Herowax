#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	VSOutput output; // ピクセルシェーダーに渡す値
	
	// 法線を回すために平行移動成分を消した行列を用意する
    matrix worldRot = matWorld;
    worldRot[0][3] = 0;
    worldRot[1][3] = 0;
    worldRot[2][3] = 0;
	
	matrix matFinal = mul(matViewProjection, matWorld);
    output.wpos = mul(matWorld, pos).xyz;
	output.svpos = mul(matFinal, pos); // 座標に行列を乗算
    output.normal = normalize(mul(worldRot, float4(normal, 1.0f)).xyz);
	output.uv = uv;
	return output;
}