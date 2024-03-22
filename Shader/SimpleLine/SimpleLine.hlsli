struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float thickness : THICKNESS;
};

struct GSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

cbuffer ConstBufferDataViewProjection : register(b0)
{
    matrix matViewProjection : packoffset(c0);
    float3 cameraPos : packoffset(c4); //‚±‚±‚Å‚Í‚Â‚©‚í‚ñ
};