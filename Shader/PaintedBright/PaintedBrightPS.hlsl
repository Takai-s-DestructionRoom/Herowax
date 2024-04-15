#include "../Include/Material.hlsli"
#include "../Include/Transform.hlsli"
#include "../Include/Camera.hlsli"
#include "../Include/Lighting.hlsli"
#include "../Include/VSStruct.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> paintTex : register(t1); //1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

struct PaintData
{
    float3 mAmbient;
    float3 mDiffuse;
    float3 mSpecular;
    float4 mColor;
    float dissolveVal;
    float slide;
};

ConstantBuffer<PaintData> paintData : register(b10);

cbuffer ConstBufferAddColor : register(b11)
{
    float4 addColor;
};

//�f�B�U�����̖ڂׂ̍���(�{���͊O���e�N�X�`������ǂݍ��񂾕�������)
static const int pattern[16] =
{
    0, 8, 2, 10,
    12, 4, 14, 6,
    3, 11, 1, 9,
    15, 7, 13, 5
};

float3 magnitude(float3 v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z);
}

// �V�F�[�_�[�^������(UV����͂�0.0�`1.0��Ԃ�/texture size�����Ń��[�v���鏈����ǉ�)
float rand(float2 uv, float2 size)
{
    uv = frac(uv / size);
    return frac(sin(dot(frac(uv / size), float2(12.9898, 78.233))) * 43758.5453) * 0.99999;
}

// �����_����2�����x�N�g��(parlin���z�x�N�g��/0�`1�͈͂Ő�������������-1�`1�͈̔�(*2-1)�Ɋg��)
float2 randVec2(float2 uv, float2 size)
{
    return normalize(float2(rand(uv, size), rand(uv + float2(127.1, 311.7), size)) * 2.0 + -1.0);
}

// �o�C���j�A���(4�_��^���Ă��̋�`����xy�������ɐ��`���)
float2 bilinear(float f0, float f1, float f2, float f3, float fx, float fy)
{
    return lerp(lerp(f0, f1, fx), lerp(f2, f3, fx), fy);
}

// fade�֐�(ease�Ȑ�������Beasing�֐�)
float fade(float t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

// �P�ʃp�[�����m�C�Y(�����_���Ɍ��肵���i�q�_�̌��z�x�N�g���ƁA�i�q�_��������_�ւ̃x�N�g���Ƃ̓��ς��o�C���j�A�⊮)
float perlinNoiseOne(float2 uv, float2 size)
{
    float2 p = floor(uv);
    float2 f = frac(uv);

    float d00 = dot(randVec2(p + float2(0, 0), size), f - float2(0, 0));
    float d10 = dot(randVec2(p + float2(1, 0), size), f - float2(1, 0));
    float d01 = dot(randVec2(p + float2(0, 1), size), f - float2(0, 1));
    float d11 = dot(randVec2(p + float2(1, 1), size), f - float2(1, 1));
    return bilinear(d00, d10, d01, d11, fade(f.x), fade(f.y)) + 0.5f;
}

// �p�[�����m�C�Y(�I�N�^�[�u�̈قȂ�m�C�Y�e�N�X�`����5������)
float perlinNoise(float2 uv, float2 size)
{
    float f = 0;
    f += perlinNoiseOne(uv * 2, size) / 2;
    f += perlinNoiseOne(uv * 4, size) / 4;
    f += perlinNoiseOne(uv * 8, size) / 8;
    f += perlinNoiseOne(uv * 16, size) / 16;
    f += perlinNoiseOne(uv * 32, size) / 32;
    return f;
}


// �@���}�b�v�p�m�C�Y(x:0.0 �` 1.0, x:0.0 �` 1.0, z:1.0)
float3 normalNoise(float2 uv, float2 size)
{
    float3 result = float3(perlinNoise(uv.xy, size),
                            perlinNoise(uv.xy + float2(1, 1), size),
                            1.0);
    return result;
}

float4 main(VSOutputBasic input) : SV_TARGET
{
    //----------�f�B�U�����̏��X�̏���-------------//
    int ditherUV_x = (int) fmod(input.svpos.x, 4.0f); //�p�^�[���̑傫���Ŋ������Ƃ��̗]������߂�
    int ditherUV_y = (int) fmod(input.svpos.y, 4.0f); //����̃p�^�[���T�C�Y��4x4�Ȃ̂�4�ŏ��Z
    float dither = pattern[ditherUV_x + ditherUV_y * 4]; //���߂��]�肩��p�^�[���l���擾
    
    float camlength = magnitude(cameraPos - input.wpos.xyz);
    
    float clamp_dither = saturate(dither / 4);
    float clamp_length = saturate(camlength / 50);
    
    clip(clamp_length - clamp_dither); //臒l��0�ȉ��Ȃ�`�悵�Ȃ�
    
    //----------�f�B�U�����̏��X�̏����I���----------//
    
    float4 texcolor = float4(tex.Sample(smp, input.uv));
    texcolor = texcolor * m_color;
    
    //�y�C���g�����̃}�e���A������
    float3 m_pAmbient = paintData.mAmbient;
    float3 m_pDiffuse = paintData.mDiffuse;
    float3 m_pSpecular = paintData.mSpecular;
    float4 paintcolor = paintData.mColor;
    float paintRatio = 0;
    
    float3 dist = normalNoise(input.uv + paintData.slide * 0.02f, float2(16.f, 16.f)); // perlin�m�C�Y�ŎZ�o�����@���𓾂�
    dist = dist * 2 - 1; // �͈͂�0.0�`1.0����-1.0�`1.0�֕ϊ�
    dist *= 0.1f;

    float dissolveTexVal = paintTex.Sample(smp, input.uv + dist.xy).r;
    if (dissolveTexVal <= paintData.dissolveVal)
    {
        paintRatio = paintcolor.a;
        paintcolor.a = 1;
        texcolor = texcolor * (1 - paintRatio) + paintcolor * paintRatio;
    }
	
	//����x
    const float shininess = 4.0f;
	
	//���_�ւ̃x�N�g��
    float3 eyedir = normalize(cameraPos - input.wpos.xyz);
	
	//�����ˌ�
    float3 ambient = m_ambient * (1 - paintRatio) + m_pAmbient * paintRatio;
	
	//�V�F�[�f�B���O���ʂ̐F
    float4 shadecolor = float4(ambientColor * ambient, 1);
	
    //���s����
    for (int i = 0; i < DIRECTIONAL_LIGHT_NUM; i++)
    {
        if (directionalLights[i].active)
        {
            //�����ւ̃x�N�g���Ɩ@���̓���
            float dotNormal = dot(-directionalLights[i].lightVec, input.normal);
		    //���ˌ��x�N�g��
            float3 reflect = normalize(directionalLights[i].lightVec + 2.0f * dotNormal * input.normal);
		    //�g�U���ˌ�
            float3 diffuse = saturate(dotNormal) * (m_diffuse * (1 - paintRatio) + m_pDiffuse * paintRatio);
		    //���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * (m_specular * (1 - paintRatio) + m_pSpecular * paintRatio);
		
            shadecolor.rgb += (diffuse + specular) * directionalLights[i].lightColor;
        }
    }
    
    //�_����
    for (i = 0; i < POINT_LIGHT_NUM; i++)
    {
        if (pointLights[i].active)
        {
            float3 lightVec = pointLights[i].pos - input.wpos;
            float d = length(lightVec);
            lightVec = normalize(lightVec);
            
            //���������W��
            float atten = 1.0f / (pointLights[i].atten.x + pointLights[i].atten.y * d + pointLights[i].atten.z * d * d);
            
            float dotNormal = dot(lightVec, input.normal);
            //���ˌ��x�N�g��
            float3 reflect = normalize(-lightVec + 2.0f * dotNormal * input.normal);
            //�g�U���ˌ�
            float3 diffuse = saturate(dotNormal) * m_diffuse;
            //���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
            
            shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].color;
        }
    }

    //�X�|�b�g���C�g
    for (i = 0; i < SPOT_LIGHT_NUM; i++)
    {
        if (spotLights[i].active)
        {
            float3 lightVec = spotLights[i].pos - input.wpos;
            float d = length(lightVec);
            lightVec = normalize(lightVec);
            
            //���������W��
            float atten = saturate(1.0f / (spotLights[i].atten.x + spotLights[i].atten.y * d + spotLights[i].atten.z * d * d));
            
            //�p�x����
            float cos = dot(lightVec, -spotLights[i].dir);
            
            //�����J�n�p�x����A�����I���p�x�ɂ����Č���
            float angleatten = smoothstep(spotLights[i].factorAngleCos.y, spotLights[i].factorAngleCos.x, cos);
            
            //�p�x��������Z
            atten *= angleatten;
            
            float dotNormal = dot(lightVec, input.normal);
            //���ˌ��x�N�g��
            float3 reflect = normalize(-lightVec + 2.0f * dotNormal * input.normal);
            //�g�U���ˌ�
            float3 diffuse = saturate(dotNormal) * m_diffuse;
            //���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
            
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[i].color;
        }
    }
    
    return texcolor * shadecolor + addColor;
}