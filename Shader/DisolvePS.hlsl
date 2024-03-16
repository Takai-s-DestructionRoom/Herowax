#include "Disolve.hlsli"

Texture2D<float4> mainTex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> maskTex : register(t1); //1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

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

float4 main(VSOutput input) : SV_TARGET
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
    
    //---------- �f�B�]���u�̏��� ----------//
    float4 mask = maskTex.Sample(smp, input.uv);
    //���̃s�N�Z���̃}�X�N�̐Ԑ������Q�Ƃ��āA�`��j�����s��
    clip(mask.r - disolve);
    //---------- �f�B�]���u�̏����I��� ----------//
    
    float4 texcolor = float4(mainTex.Sample(smp, input.uv));
    texcolor = texcolor * m_color;
	
	//����x
    const float shininess = 4.0f;
	
	//���_�ւ̃x�N�g��
    float3 eyedir = normalize(cameraPos - input.wpos.xyz);
	
	//�����ˌ�
    float3 ambient = m_ambient;
	
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
            float3 diffuse = saturate(dotNormal) * m_diffuse;
		    //���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
		
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
    
    return texcolor * shadecolor;
}