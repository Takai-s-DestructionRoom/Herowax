// ÉrÉÖÅ[&éÀâeïœä∑çsóÒ
struct Camera
{
    float3 cameraPos;
    float3 cameraDir;
    float3 cameraUp;
    matrix matViewProjection;
    matrix matInvView;
    matrix matInvProj;
    matrix matInvViewport;
    matrix matBill;
};
ConstantBuffer<Camera> bCamera : register(b0);

struct VSOUT
{
    float3 pos : POSITION;
    float radius : RADIUS;
};

struct GSOUT
{
    float4 pos : SV_POSITION;
    float3 wpos : POSITION;
    float radius : RADIUS;
};