#include "Particle3D.hlsli"

//�l�p�`�̒��_��
static const uint kVnum = 24;

//�Z���^�[����̃I�t�Z�b�g
static const float4 kOffset_array[kVnum] =
{
    //--------------- ��O ---------------//
    float4(-0.5f, -0.5f, -0.5f, 0), //����
    float4(-0.5f, +0.5f, -0.5f, 0), //����
    float4(+0.5f, -0.5f, -0.5f, 0), //�E��
    float4(+0.5f, +0.5f, -0.5f, 0), //�E��
    
    //---------------  ��  ---------------//
    float4(-0.5f, -0.5f, +0.5f, 0), //����
    float4(-0.5f, +0.5f, +0.5f, 0), //����
    float4(+0.5f, -0.5f, +0.5f, 0), //�E��
    float4(+0.5f, +0.5f, +0.5f, 0), //�E��
    
    //---------------  ��  ---------------//
    float4(-0.5f, -0.5f, -0.5f, 0), //����
    float4(-0.5f, -0.5f, +0.5f, 0), //����
    float4(-0.5f, +0.5f, -0.5f, 0), //�E��
    float4(-0.5f, +0.5f, +0.5f, 0), //�E��
    
    //---------------  �E  ---------------//
    float4(+0.5f, -0.5f, -0.5f, 0), //����
    float4(+0.5f, -0.5f, +0.5f, 0), //����
    float4(+0.5f, +0.5f, -0.5f, 0), //�E��
    float4(+0.5f, +0.5f, +0.5f, 0), //�E��
    
    //---------------  ��  ---------------//
    float4(-0.5f, -0.5f, -0.5f, 0), //����
    float4(+0.5f, -0.5f, -0.5f, 0), //����
    float4(-0.5f, -0.5f, +0.5f, 0), //�E��
    float4(+0.5f, -0.5f, +0.5f, 0), //�E��
    
    //---------------  ��  ---------------//
    float4(-0.5f, +0.5f, -0.5f, 0), //����
    float4(+0.5f, +0.5f, -0.5f, 0), //����
    float4(-0.5f, +0.5f, +0.5f, 0), //�E��
    float4(+0.5f, +0.5f, +0.5f, 0), //�E��
};

//���オ0,0�@�E����1,1
static const float2 kUV_array[kVnum] =
{
    //--------------- ��O ---------------//
    float2(0.0f, 1.0f), //����
    float2(0.0f, 0.0f), //����
    float2(1.0f, 1.0f), //�E��
    float2(1.0f, 0.0f), //�E��
    //---------------  ��  ---------------//
    float2(0.0f, 1.0f), //����
    float2(0.0f, 0.0f), //����
    float2(1.0f, 1.0f), //�E��
    float2(1.0f, 0.0f), //�E��
    //---------------  ��  ---------------//
    float2(0.0f, 1.0f), //����
    float2(0.0f, 0.0f), //����
    float2(1.0f, 1.0f), //�E��
    float2(1.0f, 0.0f), //�E��
    //---------------  �E  ---------------//
    float2(0.0f, 1.0f), //����
    float2(0.0f, 0.0f), //����
    float2(1.0f, 1.0f), //�E��
    float2(1.0f, 0.0f), //�E��
    //---------------  ��  ---------------//
    float2(0.0f, 1.0f), //����
    float2(0.0f, 0.0f), //����
    float2(1.0f, 1.0f), //�E��
    float2(1.0f, 0.0f), //�E��
    //---------------  ��  ---------------//
    float2(0.0f, 1.0f), //����
    float2(0.0f, 0.0f), //����
    float2(1.0f, 1.0f), //�E��
    float2(1.0f, 0.0f), //�E��
};

static const float3 kNormal_array[kVnum] =
{
    //--------------- ��O ---------------//
    float3(0.0f, 0.0f, -1.0f), //����
    float3(0.0f, 0.0f, -1.0f), //����
    float3(0.0f, 0.0f, -1.0f), //�E��
    float3(0.0f, 0.0f, -1.0f), //�E��
    //---------------  ��  ---------------//
    float3(0.0f, 0.0f, +1.0f), //����
    float3(0.0f, 0.0f, +1.0f), //����
    float3(0.0f, 0.0f, +1.0f), //�E��
    float3(0.0f, 0.0f, +1.0f), //�E��
    //---------------  ��  ---------------//
    float3(-1.0f, 0.0f, 0.0f), //����
    float3(-1.0f, 0.0f, 0.0f), //����
    float3(-1.0f, 0.0f, 0.0f), //�E��
    float3(-1.0f, 0.0f, 0.0f), //�E��
    //---------------  �E  ---------------//
    float3(+1.0f, 0.0f, 0.0f), //����
    float3(+1.0f, 0.0f, 0.0f), //����
    float3(+1.0f, 0.0f, 0.0f), //�E��
    float3(+1.0f, 0.0f, 0.0f), //�E��
    //---------------  ��  ---------------//
    float3(0.0f, -1.0f, 0.0f), //����
    float3(0.0f, -1.0f, 0.0f), //����
    float3(0.0f, -1.0f, 0.0f), //�E��
    float3(0.0f, -1.0f, 0.0f), //�E��
    //---------------  ��  ---------------//
    float3(0.0f, +1.0f, 0.0f), //����
    float3(0.0f, +1.0f, 0.0f), //����
    float3(0.0f, +1.0f, 0.0f), //�E��
    float3(0.0f, +1.0f, 0.0f), //�E��
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
        
        //X����]�s��
        float sinX = sin(input[0].rot.x);
        float cosX = cos(input[0].rot.x);

        float4x4 matX = float4x4(
        1, 0, 0, 0,
        0, cosX, sinX, 0,
        0, -sinX, cosX, 0,
        0, 0, 0, 1);
        
        //Y����]�s��
        float sinY = sin(input[0].rot.y);
        float cosY = cos(input[0].rot.y);

        float4x4 matY = float4x4(
        cosY, 0, sinY, 0,
        0, 1, 0, 0,
        -sinY, 0, cosY, 0,
        0, 0, 0, 1);
        
        offset = mul(matZ, offset);
        offset = mul(matX, offset);
        offset = mul(matY, offset);
        
        float4x4 rotMat = matWorld;
        rotMat = mul(matZ, rotMat);
        rotMat = mul(matX, rotMat);
        rotMat = mul(matY, rotMat);
        
        //�V�X�e���p���_���W
        //�I�t�Z�b�g�����炷(���[���h���W)
        float4 svpos = input[0].pos + offset;
        
        //�r���[�A�ˉe�ϊ�
        svpos = mul(matViewProjection, svpos);
        float4 wpos = mul(matWorld, input[0].pos + offset);
        element.svpos = svpos;
        element.worldpos = wpos;
        element.color = input[0].color;
        float4 wnormal = normalize(mul(rotMat, float4(kNormal_array[i], 0)));
        element.normal = wnormal.xyz;
        element.uv = kUV_array[i];
        
        output.Append(element);
    }
}