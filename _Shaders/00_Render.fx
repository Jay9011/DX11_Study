///////////////////////////////////////////////////////////////////////////////
///////////////           Mesh                             ////////////////////
///////////////////////////////////////////////////////////////////////////////
struct VertexMesh
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    
    matrix Transform : InstTransform;
    uint InstanceID : SV_InstanceID;
};

///////////////////////////////////////////////////////////////////////////////

#define VS_GENERATE \
output.oPosition = input.Position;\
output.Position = WorldPosition(input.Position);\
output.Position = ViewProjection(output.Position);\
output.Normal = WorldNormal(input.Normal);\
output.Uv = input.Uv;\

///////////////////////////////////////////////////////////////////////////////

void SetMeshWorld(inout matrix world, VertexMesh input)
{
    world = input.Transform;
}

MeshOutput VS_Mesh(VertexMesh input)
{
    MeshOutput output;
    
    SetMeshWorld(World, input);
    VS_GENERATE
    
    return output;
}

///////////////////////////////////////////////////////////////////////////////
///////////////           Model                            ////////////////////
///////////////////////////////////////////////////////////////////////////////

struct VertexModel
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 BlendIndices : BlendIndices;
    float4 BlendWeights : BlendWeights;
    
    matrix Transform : Inst1_Transform;
    uint InstanceID : SV_InstanceID;
    
    float4 Color : Inst2_Color;
};

Texture2DArray TransformsMap;
#define MAX_MODEL_TRANSFORMS 250

cbuffer CB_Bone
{
    // Bone ����� Instance�� �Ѿ���⿡ Matrix �迭�� �ʿ� ���������ϴ�.
    //matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
    
    uint BoneIndex;
};

void SetModelWorld(inout matrix world, VertexModel input)
{
    // Texture�� ���� Model ���� �迭���� �����͸� ��Ī �ؿɴϴ�.
    float4 m0 = TransformsMap.Load(int4(BoneIndex * 4 + 0, input.InstanceID, 0, 0));
    float4 m1 = TransformsMap.Load(int4(BoneIndex * 4 + 1, input.InstanceID, 0, 0));
    float4 m2 = TransformsMap.Load(int4(BoneIndex * 4 + 2, input.InstanceID, 0, 0));
    float4 m3 = TransformsMap.Load(int4(BoneIndex * 4 + 3, input.InstanceID, 0, 0));
    
    // ������ �����͵�� World �����͸� �����ݴϴ�.
    matrix transform = matrix(m0, m1, m2, m3);
    world = mul(transform, input.Transform);
}

MeshOutput VS_Model(VertexModel input)
{
    MeshOutput output;
    
    SetModelWorld(World, input);
    VS_GENERATE
    
    return output;
}
