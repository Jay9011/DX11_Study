struct LightDesc
{
    float4 Ambient;
    float4 Specular;
    float3 Direction;
    float Padding;
    float3 Position;
};

cbuffer CB_Light
{
    LightDesc GlobalLight;
};

Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;
TextureCube SkyCubeMap;

struct MaterialDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
};

cbuffer CB_Material
{
    MaterialDesc Material;
};

MaterialDesc MakeMaterial()
{
    MaterialDesc output;
    output.Ambient = float4(0, 0, 0, 0);
    output.Diffuse = float4(0, 0, 0, 0);
    output.Specular = float4(0, 0, 0, 0);
    output.Emissive = float4(0, 0, 0, 0);

    return output;
}

/////////////////////////////////////////////////////////////////////

void Texture(inout float4 color, Texture2D t, float2 uv, SamplerState samp)
{
    float4 sampling = t.Sample(samp, uv);

    color = color * sampling;
}

void Texture(inout float4 color, Texture2D t, float2 uv)
{
    Texture(color, t, uv, LinearSampler);
}

/////////////////////////////////////////////////////////////////////

void ComputeLight(out MaterialDesc output, float3 normal, float3 wPosition)
{
    output = MakeMaterial();

    float3 direction = -GlobalLight.Direction;
    float NdotL = dot(direction, normalize(normal));

    output.Ambient = GlobalLight.Ambient * Material.Ambient;

    [flatten]
    if(NdotL > 0.0f)
    {
        output.Diffuse = Material.Diffuse * NdotL;
    }
}