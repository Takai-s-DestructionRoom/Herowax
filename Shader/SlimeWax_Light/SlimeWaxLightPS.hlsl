#include "SlimeWaxLight.hlsli"

Texture2D<float4> mainTex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
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

struct PS_OUT
{
    float4 color : SV_TARGET;
    float depth : SV_DEPTH;
};

PS_OUT main(VSOutput input)
{
    //float4 texcolor = float4(mainTex.Sample(smp, input.uv));
    //texcolor = texcolor * m_color;
	
    //���C�̊J�n�_���I�u�W�F�N�g����ɂ��邱�Ƃ�
    //�ǂꂾ������Ă�raymarch���񂷉񐔂����ɂȂ邽�߁A�`�悳���悤�ɂȂ�
    float3 pos = input.wpos.xyz;
    //��������L�΂��x�N�g��
    float3 rayDir = normalize(input.wpos.xyz - cameraPos);
	
    //�������C�g������
    float3 norm = input.normal;
    float3 baseColor = waxColor.rgb;

    //�������C�g�̋���(�����Ă镔���̑傫��)
    const float rimRate = pow(1 - abs(dot(norm, rayDir)), rimPower); //�֊s���ۂ������̎Z�o  
            
    float3 color = clamp(lerp(baseColor, rimColor.rgb, rimRate), 0, 1); //�F
            
    PS_OUT output;
    output.color = float4(color, 1);
    output.depth = getDepth(pos);
            
    return output;
}