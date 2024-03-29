#include "Noise.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

// �V�F�[�_�[�^������(fragment�V�F�[�_�[��UV����͂�0.0�`1.0��Ԃ�/texture size�����Ń��[�v���鏈����ǉ�)
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

float4 main(OutputVS i) : SV_TARGET
{
    //float2 uvMove = { i.uv.x, i.uv.y * time };
    
    
    float3 dist = normalNoise(i.uv, float2(16.f, 16.f)); // perlin�m�C�Y�ŎZ�o�����@���𓾂�
    dist = dist * 2.0 - 1.0; // �͈͂�0.0�`1.0����-1.0�`1.0�֕ϊ�
    dist *= 0.1f; // �c�݋��x����Z(�c�݋��x���V�F�[�_�[�p�����[�^�Ƃ��Ē����\�ɂ���)

    i.uv.xy += dist.xy; // �c�ݗʂ����A���C���e�N�X�`����UV�����炷

    float4 texcolor = float4(tex.Sample(smp, i.uv));
    return texcolor;

}