#include "../Include/Transform.hlsli"
#include "../Include/Camera.hlsli"
#include "../Include/VSStruct.hlsli"

// ワールド変換行列
struct MeltParam
{
    float factor;
    float falloff;
    float radius;
    float top;
    float bottom;
    float4x4 matMeshToAxis;
    float4x4 matAxisToMesh;
    bool clampAtBottom;
};
ConstantBuffer<MeltParam> cbMelt : register(b10);

VSOutputBasic main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	VSOutputBasic output; // ピクセルシェーダーに渡す値
    
    float4 mPos = pos;
    
    if (cbMelt.top != cbMelt.bottom)
    {
        mPos = mul(cbMelt.matMeshToAxis, pos);
    
        float range = cbMelt.top - cbMelt.bottom;
        float meltAmount = pow(1 - saturate((mPos.y - cbMelt.bottom) / range), cbMelt.falloff) * cbMelt.factor;
    
        float2 nPos = length(mPos.xz) == 0 ? float2(0, 0) : mPos.xz;
        mPos.xz += nPos * meltAmount * cbMelt.radius;
    
        if (cbMelt.clampAtBottom)
            mPos.y = max(cbMelt.bottom, mPos.y);
    
        mPos = mul(cbMelt.matAxisToMesh, mPos);
    }
	
	// 法線を回すために平行移動成分を消した行列を用意する
    matrix worldRot = matWorld;
    worldRot[0][3] = 0;
    worldRot[1][3] = 0;
    worldRot[2][3] = 0;
	
	matrix matFinal = mul(matViewProjection, matWorld);
    output.wpos = mul(matWorld, mPos).xyz;
	output.svpos = mul(matFinal, mPos); // 座標に行列を乗算
    output.normal = normalize(mul(worldRot, float4(normal, 1.0f)).xyz);
	output.uv = uv;
	return output;
}