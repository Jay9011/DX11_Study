#include "00_Global.fx"

Texture2D ParticleMap;

struct ParticleDesc
{
    float4 MinColor;
    float4 MaxColor;
    
    float3 Gravity;
    float EndVelocity;
    
    float2 StartSize;
    float2 EndSize;
    
    float2 RotateSpeed;
    float ReadyTime;
    float ReadyRandomTime;
    
    float ColorAmount;
    float CurrentTime;
};

cbuffer CB_Particle
{
    ParticleDesc Particle;
};

struct VertexInput
{
    float4 Position : Position;
    float2 Corner : Uv;
    float3 Velocity : Velocity;
    float4 Random : Random; //x : 주기, y - 크기, z - 회전, w - 색상
    float Time : Time;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Color : Color;
    float2 Uv : Uv;
};

float ComputePosition(float3 position, float3 velocity, float age, float normalizedAge)
{
    float start = length(velocity); // 속도
    float end = start * Particle.EndVelocity;

    float amount = start * normalizedAge + (end - start) * normalizedAge * 0.5;

    position += normalize(velocity) * amount * Particle.ReadyTime;
    position += Particle.Gravity * age * normalizedAge;

    return ViewProjection(float4(position, 1));
}

float ComputeSize(float value, float normalizedAge)
{
    float start = lerp(Particle.StartSize.x, Particle.StartSize.y, value);
    float end = lerp(Particle.EndSize.x, Particle.EndSize.y, value);

    return lerp(start, end, normalizedAge);
}


VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    
    float age = Particle.CurrentTime - input.Time;
    age *= input.Random.x * Particle.ReadyRandomTime + 1;

    float normalizedAge = saturate(age / Particle.ReadyTime);

    output.Position = ComputePosition(input.Position.xyz, input.Velocity, age, normalizedAge);

    float size = ComputeSize(input.Random.y, normalizedAge);
    
    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);

    diffuse.rgb = Color.rgb * input.Alpha * 2.0f;
    diffuse.a = diffuse.a * input.Alpha * 1.5f;

    return diffuse;
}

technique11 T0
{
    P_BS_VP(P0, AlphaBlend, VS, PS)
    P_BS_VP(P1, AlphaBlend_AlphaToCoverageEnable, VS, PS)

    P_BS_VP(P2, AdditiveBlend, VS, PS)
    P_BS_VP(P3, AdditiveBlend_AlphaToCoverageEnable, VS, PS)
}