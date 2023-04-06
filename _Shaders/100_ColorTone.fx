#include "00_Global.fx"
#include "00_Light.fx"

float2 PixelSize;

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

VertexOutput VS(float4 Position : Position)
{
    VertexOutput output;
    
    output.Position = Position;
    output.Uv.x = Position.x * 0.5f + 0.5f;
    output.Uv.y = -Position.y * 0.5f + 0.5f;
    
    return output;
}

float4 PS_Diffuse(VertexOutput input) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}

float4 PS_Inverse(VertexOutput input) : SV_Target
{
    return float4(1.0f - DiffuseMap.Sample(LinearSampler, input.Uv).rgb, 1.0f);
}

float4 PS_Grayscale(VertexOutput input) : SV_Target
{
    float4 color = float4(1.0f - DiffuseMap.Sample(LinearSampler, input.Uv));
	float average = (color.r + color.g + color.b) / 3.0f;

    return float4(average, average, average, 1.0f);
}

float4 PS_Grayscale2(VertexOutput input) : SV_Target
{
    float4 color = float4(1.0f - DiffuseMap.Sample(LinearSampler, input.Uv));

	float3 grayscale = float3(0.2627f, 0.6780f, 0.0593f);
    float average = dot(color.rgb, grayscale);

    return float4(average, average, average, 1.0f);
}

float Sharpness = 0;
float4 PS_Sharpness(VertexOutput input) : SV_Target
{
    float4 center = DiffuseMap.Sample(LinearSampler, input.Uv);
    float4 top = DiffuseMap.Sample(LinearSampler, input.Uv + float2(0, -PixelSize.y));
    float4 bottom = DiffuseMap.Sample(LinearSampler, input.Uv + float2(0, +PixelSize.y));
    float4 left = DiffuseMap.Sample(LinearSampler, input.Uv + float2(-PixelSize.x, 0));
    float4 right = DiffuseMap.Sample(LinearSampler, input.Uv + float2(+PixelSize.x, 0));

    float edge = center * 4 - top - bottom - left - right;

    return (center + Sharpness) * edge;
}


technique11 T0
{
    P_VP(P0, VS, PS_Diffuse)
    P_VP(P1, VS, PS_Inverse)
    P_VP(P2, VS, PS_Grayscale)
    P_VP(P3, VS, PS_Grayscale2)
    P_VP(P4, VS, PS_Sharpness)
}