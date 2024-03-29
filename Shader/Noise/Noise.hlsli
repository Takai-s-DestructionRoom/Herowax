cbuffer ConstBufferData : register(b0)
{
    float fineness;
    float time;
};


struct OutputVS
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};