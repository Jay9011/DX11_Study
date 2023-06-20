#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float4 PS(MeshOutput input) : SV_Target
{
    float4 color = float4(1, 1, 1, 1);

    float4 position = input.sPosition;

    position.xyz /= position.w;

    position.x = position.x + 0.5f + 0.5f;
    position.y = -position.y + 0.5f + 0.5f;

    float depth = 0;
    float z = position.z;
    float factor = 0;

    depth = ShadowMap.Sample(LinearSampler, position.xy).r;
    factor = (float) (depth >= z);

    return color * factor;
}

technique11 T0
{
    //1Pass - Depth Rendering
    P_VP(P0, VS_Depth_Mesh, PS_Depth)
    P_VP(P1, VS_Depth_Model, PS_Depth)
    P_VP(P2, VS_Depth_Animation, PS_Depth)

    P_VP(P3, VS_Mesh, PS)
    P_VP(P4, VS_Model, PS)
    P_VP(P5, VS_Animation, PS)
}