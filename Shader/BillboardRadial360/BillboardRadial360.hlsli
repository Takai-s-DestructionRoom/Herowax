// ���_�V�F�[�_�[(Basic)�̏o�͍\����
// �~�`�Q�[�W�ɕK�v�Ȃ���
cbuffer ConstBufferDataCircleGauge : register(b4)
{
    float radian; //(0~2�΂܂�)
};

struct VSOutput
{
    float3 wpos : WORLD_POSITION; //���[���h���_���W
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
    float3 normal : NORMAL; //�@���x�N�g��
    float2 uv : TEXCOORD; // uv�l
};