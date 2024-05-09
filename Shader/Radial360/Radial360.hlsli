#include "../Include/Material.hlsli"
#include "../Include/Transform.hlsli"

// �r���[&�ˉe�ϊ��s��
cbuffer ConstBufferDataViewProjection : register(b2)
{
    matrix matViewProjection;
};

// �~�`�Q�[�W�ɕK�v�Ȃ���
cbuffer ConstBufferDataCircleGauge : register(b3)
{
    float radian;  //(0~2�΂܂�)
};

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOutput
{
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
    float3 normal : NORMAL; //�@���x�N�g��
    float2 uv : TEXCOORD; // uv�l
};