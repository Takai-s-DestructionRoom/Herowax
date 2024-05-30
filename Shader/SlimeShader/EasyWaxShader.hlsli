// ���[���h�ϊ��s��
struct TransformData
{
    matrix matWorld;
};
ConstantBuffer<TransformData> bTransform : register(b0);

// �r���[&�ˉe�ϊ��s��
struct ViewProjectionData
{
    matrix matViewProjection;
    float3 cameraPos;
};
ConstantBuffer<ViewProjectionData> bCamera : register(b1);

// ���E�p�����[�^
struct WaxData
{
    float4 color;
    float distance;
};
ConstantBuffer<WaxData> bWax : register(b2);

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOUT
{
    float3 wpos : WORLD_POSITION; //���[���h���_���W
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
    float3 normal : NORMAL; //�@���x�N�g��
    float2 uv : TEXCOORD; // uv�l
};