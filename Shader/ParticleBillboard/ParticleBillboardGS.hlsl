#include "ParticleBillboard.hlsli"

//�l�p�`�̒��_��
static const uint kVnum = 4;

//�Z���^�[����̃I�t�Z�b�g
static const float4 kOffset_array[kVnum] =
{
    float4(-0.5f, -0.5f, 0.f, 0), //����
    float4(-0.5f, +0.5f, 0.f, 0), //����
    float4(+0.5f, -0.5f, 0.f, 0), //�E��
    float4(+0.5f, +0.5f, 0.f, 0), //�E��
};

//���オ0,0�@�E����1,1
static const float2 kUV_array[kVnum] =
{
    float2(0.0f, 1.0f), //����
    float2(0.0f, 0.0f), //����
    float2(1.0f, 1.0f), //�E��
    float2(1.0f, 0.0f), //�E��
};

static const float3 kNormal_array[kVnum] =
{
    float3(0.0f, 0.0f, -1.0f), //����
    float3(0.0f, 0.0f, -1.0f), //����
    float3(0.0f, 0.0f, -1.0f), //�E��
    float3(0.0f, 0.0f, -1.0f), //�E��
};

[maxvertexcount(kVnum)]
void main(
	point VSOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    GSOutput element; //�o�͗p���_�f�[�^
   
    for (uint i = 0; i < kVnum; i++)
    {
        //���S����̃I�t�Z�b�g���X�P�[�����O
        float4 offset = kOffset_array[i] * input[0].scale;
        
        //Z����]�s��
        float sinZ = sin(input[0].rot.z);
        float cosZ = cos(input[0].rot.z);

        float4x4 matZ = float4x4(
        cosZ, sinZ, 0, 0,
        -sinZ, cosZ, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
        
        //Z���̉�]�͓K�p
        offset = mul(matBillboard, mul(matZ, offset));
        
        //�V�X�e���p���_���W
        //�I�t�Z�b�g�����炷(���[���h���W)
        float4 svpos = input[0].pos + offset;
        
        //�r���[�A�ˉe�ϊ�
        svpos = mul(matViewProjection, svpos);
        float4 wpos = mul(matWorld, input[0].pos + offset);
        element.svpos = svpos;
        element.worldpos = wpos;
        element.color = input[0].color;
        float4 wnormal = normalize(float4(kNormal_array[i], 0));
        element.normal = wnormal.xyz;
        element.uv = kUV_array[i];
        element.timer = input[0].timer;
        
        output.Append(element);
    }
}