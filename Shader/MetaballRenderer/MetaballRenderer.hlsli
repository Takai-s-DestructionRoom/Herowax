// �r���[&�ˉe�ϊ��s��
cbuffer ConstBufferDataMetaball2D : register(b0)
{
    float4 color_;
    float4 strokecolor_;
    float cutoff_;
    float stroke_;
};

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOutput
{
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
    float2 uv : TEXCOORD; // uv�l
};