#include "../Include/Material.hlsli"
#include "../Include/Transform.hlsli"
#include "../Include/Camera.hlsli"
#include "../Include/VSStruct.hlsli"
#include "Billboard.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

//�[�x�̌v�Z
float getDepth(float3 pos)
{
    float4 vpPos = mul(matViewProjection, float4(pos, 1.0f));

    //OpenGL�����Ƃ������̎����g��Ȃ��Ƃ����Ȃ��炵��
    //return (vpPos.z / vpPos.w) * 0.5f + 0.5f;
    //DirectX���Ȃ炱����
    return (vpPos.z / vpPos.w);
}

float4 main(VSOutput input)
{ 
    float4 texcolor = float4(tex.Sample(smp, input.uv));
    texcolor = texcolor * m_color;

    return texcolor;
}