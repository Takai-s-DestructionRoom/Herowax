// マテリアル
struct Material
{
    float4 m_color; // 色(RGBA)
    float3 m_ambient;
    float3 m_diffuse;
    float3 m_specular;
};
ConstantBuffer<Material> bMaterial : register(b0);

struct Camera
{
    float3 pos;
    float3 dir;
    float3 up;
    matrix matViewProj;
    matrix matInvView;
    matrix matInvProj;
    matrix matInvViewport;
};
ConstantBuffer<Camera> bCamera : register(b1);

// 平行光源型
struct DirectionalLight
{
    uint active;
    float3 lightVec;
    float3 lightColor;
};

//点光源型
struct PointLight
{
    uint active;
    float3 pos;
    float3 color;
    float3 atten;
};

//スポットライト型
struct SpotLight
{
    uint active;
    float3 pos;
    float3 dir;
    float3 color;
    float3 atten;
    float2 factorAngleCos;
};

// 光源情報
static const int DIRECTIONAL_LIGHT_NUM = 8;
static const int POINT_LIGHT_NUM = 8;
static const int SPOT_LIGHT_NUM = 8;
struct LightGroup
{
    float3 ambientColor;
    DirectionalLight directionalLights[DIRECTIONAL_LIGHT_NUM];
    PointLight pointLights[POINT_LIGHT_NUM];
    SpotLight spotLights[SPOT_LIGHT_NUM];
};
ConstantBuffer<LightGroup> bLightGroup : register(b2);

// 頂点シェーダーの出力構造体
struct VSOUT
{
    //float3 wpos : WORLD_POSITION; //ワールド頂点座標
    float4 svpos : SV_POSITION; // システム用頂点座標
    float2 uv : TEXCOORD; // uv値
};

static const int MAX_SPHERE_COUNT = 1024;
struct WaxData
{
    float4 spheres[MAX_SPHERE_COUNT];
    uint sphereNum;
    uint rayMarchNum;
    float hitThreshold;
    float smoothFactor;
};
ConstantBuffer<WaxData> bWax : register(b3);