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
    float4 color = float4(DiffuseMap.Sample(LinearSampler, input.Uv));
	float average = (color.r + color.g + color.b) / 3.0f;

    return float4(average, average, average, 1.0f);
}

float4 PS_Grayscale2(VertexOutput input) : SV_Target
{
    float4 color = float4(DiffuseMap.Sample(LinearSampler, input.Uv));

	float3 grayscale = float3(0.2627f, 0.6780f, 0.0593f);
    float average = dot(color.rgb, grayscale);

    return float4(average, average, average, 1.0f);
}

float Saturation = 0;
//Saturation = 0 : grayscale
//0 < Saturation < 1 : desaturation
//Saturation = 1 : original
//Saturation > 1 : satuaration

float4 PS_Saturation(VertexOutput input) : SV_Target
{
    float4 color = float4(DiffuseMap.Sample(LinearSampler, input.Uv));
	float3 grayscale = float3(0.2627f, 0.6780f, 0.0593f);

    float temp = dot(color.rgb, grayscale);

    color.rgb = lerp(temp, color.rgb, Saturation);
    color.a = 1.0f;

    return color;
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


float4x4 ColorToSepiaMatrix = float4x4
(
    0.393, 0.769, 0.189, 0,
    0.349, 0.686, 0.168, 0,
    0.272, 0.534, 0.131, 0,
    0, 0, 0, 1
);

float4 PS_Sepia(VertexOutput input) : SV_Target
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);
    
    return mul(ColorToSepiaMatrix, color);
}


float Power = 2;    // 1 = Linear, 1 > Non-Linear
float2 Scale = float2(2, 2);    // 비네팅 크기

float4 PS_Vignette(VertexOutput input) : SV_Target
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);
    
    float radius = length((input.Uv - 0.5f) * 2 / Scale);
    float vignette = pow(abs(radius + 1e-6f), Power);
    
    return saturate(1 - vignette) * color;
}

float LensPower = 1;
float3 Distortion = -0.02f;

float4 PS_LensDistortion(VertexOutput input) : SV_Target
{
    float2 uv = input.Uv * 2 - 1;
    
    float2 vpSize = float2(1.0f / PixelSize.x, 1.0f / PixelSize.y);
    float aspect = vpSize.x / vpSize.y;
    float radiusSquared = aspect * aspect + uv.x * uv.x + uv.y * uv.y;
    float radius = sqrt(radiusSquared);

    float3 f = Distortion * pow(abs(radius + 1e-6f), LensPower) + 1;
    
    float2 r = (f.r * uv + 1) * 0.5f;
    float2 g = (f.g * uv + 1) * 0.5f;
    float2 b = (f.b * uv + 1) * 0.5f;
    
    float4 color = 0;
    color.r = DiffuseMap.Sample(LinearSampler, r).r;
    color.ga = DiffuseMap.Sample(LinearSampler, g).ga;
    color.b = DiffuseMap.Sample(LinearSampler, b).b;
    
    return color;
}

technique11 T0
{
    P_VP(P0, VS, PS_Diffuse)
    P_VP(P1, VS, PS_Inverse)
    P_VP(P2, VS, PS_Grayscale)
    P_VP(P3, VS, PS_Grayscale2)
    P_VP(P4, VS, PS_Saturation)
    P_VP(P5, VS, PS_Sharpness)
    P_VP(P6, VS, PS_Sepia)
    P_VP(P7, VS, PS_Vignette)
    P_VP(P8, VS, PS_LensDistortion)
}