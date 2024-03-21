// ƒrƒ…[&Ë‰e•ÏŠ·s—ñ
cbuffer ConstBufferCamera : register(b2)
{
    matrix matViewProjection : packoffset(c0);
    float3 cameraPos : packoffset(c4);
};