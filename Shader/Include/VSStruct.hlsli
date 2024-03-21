// 頂点シェーダー(Basic)の出力構造体
struct VSOutputBasic
{
    float3 wpos : WORLD_POSITION; //ワールド頂点座標
    float4 svpos : SV_POSITION; // システム用頂点座標
    float3 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD; // uv値
};