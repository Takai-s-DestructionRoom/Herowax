#include "../Include/Material.hlsli"
#include "../Include/Transform.hlsli"
#include "../Include/Camera.hlsli"
#include "../Include/VSStruct.hlsli"
#include "BillboardRadial360.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
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

float4 main(VSOutput input) : SV_Target
{ 
    //�l�������Ă���ꍇ�̂ݏ���
    if (radian > 0.f)
    {
        //uv�̈ʒu�����炷
        float uvX = (input.uv.x - 0.5f);
        float uvY = (input.uv.y - 0.5f) * -1.0f;
    
        //�s�N�Z���̊p�x���o��
        float blue = -1 * uvY;
        float red = -1 * uvX;
        //��r�������s�N�Z���̊p�x
        float thisAngle = atan2(red, blue);
    
        //�S�Đ��̒l�̕��������₷���̂�180�������Z
        thisAngle += 3.141592f;
    
        //�萔�o�b�t�@�œn���Ă���l��菬�����Ȃ�`��L�����Z��
        if (thisAngle < radian)
        {
            discard;
        }
    }
    
    float4 texcolor = float4(tex.Sample(smp, input.uv));
    texcolor = texcolor * m_color;

    return texcolor;
}