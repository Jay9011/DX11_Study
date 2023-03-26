#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float4 PS(MeshOutput input) : SV_Target
{
    return PS_AllLight(input);
}

////////////////////////////////////////////////////////////////////////////////////

struct VertexBillboard
{
    float4 Position : Position;
    float2 Scale : Scale;
};

struct VertexOutput
{
    float4 Position : Position;
    float2 Scale : Scale;
};

VertexOutput VS(VertexBillboard input)
{
    VertexOutput output;

    output.Position = WorldPosition(input.Position);
    output.Scale = input.Scale;
    
    return output;
}

struct GeometryOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

[maxvertexcount(4)]
void GS_Billboard(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    // float3 forward = float3(0, 0, 1);
    float3 forward = input[0].Position.xyz - ViewPosition();
    float3 right = normalize(cross(up, forward));


    float2 size = input[0].Scale * 0.5f;

    float4 position[4];
    position[0] = float4(input[0].Position.xyz - size.x * right - size.y * up, 1);
    position[1] = float4(input[0].Position.xyz - size.x * right + size.y * up, 1);
    position[2] = float4(input[0].Position.xyz + size.x * right - size.y * up, 1);
    position[3] = float4(input[0].Position.xyz + size.x * right + size.y * up, 1);

    float2 uv[4] =
    {
        float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
    };


    GeometryOutput output;

    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        output.Position = ViewProjection(position[i]);
        output.Uv = uv[i];

        stream.Append(output);
    }
}

float4 PS_Billboard(GeometryOutput input) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}

technique11 T0
{
    P_VP(P0, VS_Mesh, PS)
    P_VP(P1, VS_Model, PS)
    P_VP(P2, VS_Animation, PS)

    P_BS_VGP(P3, AlphaBlend, VS, GS_Billboard, PS_Billboard)
}