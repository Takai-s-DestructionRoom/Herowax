#include "Noise.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

//�����_��
float random(float2 p, float extend = 1.0f);
float random(float2 p, float extend)
{
    return frac(sin(dot(p, float2(12.9898f, 78.233f))) * (43758.5453f + extend));
}

//�m�C�Y
float noise(float2 st, float extend = 1.0f);
float noise(float2 st, float extend)
{
    float2 p = floor(st);
    return random(p, extend); //
}

float4 main(OutputVS i) : SV_TARGET
{
    float4 color = tex.Sample(smp, i.uv);
    float c = noise(i.uv * 256.0f, 0);
    float4 noiseCol = float4(c, c, c, 1.0f);
    
    return noiseCol;
}