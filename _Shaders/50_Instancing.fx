#include "00_Global.fx"

float3 Direction = float3(-1, -1, +1);

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    
    matrix Transform : Inst1;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    
    // instancing을 사용하면 world는 CPU를 통해서 매번 받아오는 것이 아니라
    // VRAM에 저장되어 있는 InstanceBuffer의 World를 사용할 것입니다.
    World = input.Transform;
    
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Normal = WorldNormal(input.Normal);
    
    output.Uv = input.Uv;
    
    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float3 light = -Direction;
    
    return DiffuseMap.Sample(LinearSampler, input.Uv) * dot(light, normal);
}

technique11 T0
{
    P_VP(P0, VS, PS)
    P_RS_VP(P1, FillMode_WireFrame, VS, PS)
}