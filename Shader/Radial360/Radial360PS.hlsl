#include "Radial360.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
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
   
    //�ȉ��`��
    float4 texcolor = float4(tex.Sample(smp, input.uv));

    texcolor = texcolor * m_color;
      
    return float4(texcolor.rgb, texcolor.a);
}