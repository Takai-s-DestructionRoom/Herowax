#include "../Include/Transform.hlsli"
#include "../Include/Camera.hlsli"
#include "../Include/VSStruct.hlsli"

// ���[���h�ϊ��s��
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
	VSOutputBasic output; // �s�N�Z���V�F�[�_�[�ɓn���l
    
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
	
	// �@�����񂷂��߂ɕ��s�ړ��������������s���p�ӂ���
    matrix worldRot = matWorld;
    worldRot[0][3] = 0;
    worldRot[1][3] = 0;
    worldRot[2][3] = 0;
	
	matrix matFinal = mul(matViewProjection, matWorld);
    output.wpos = mul(matWorld, mPos).xyz;
	output.svpos = mul(matFinal, mPos); // ���W�ɍs�����Z
    output.normal = normalize(mul(worldRot, float4(normal, 1.0f)).xyz);
	output.uv = uv;
	return output;
}