// 頂点シェーダーの出力構造体
struct VSOUT
{
    //float3 wpos : WORLD_POSITION; //ワールド頂点座標
    float4 svpos : SV_POSITION; // システム用頂点座標
    float2 uv : TEXCOORD; // uv値
};