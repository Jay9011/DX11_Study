float4 PS_Sky(MeshOutput input) : SV_Target
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition);
}
///////////////////////////////////////////////////////////////////////////////
///////////////           Mesh                             ////////////////////
///////////////////////////////////////////////////////////////////////////////
struct VertexMesh
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    
    matrix Transform : Inst1_Transform;
    float4 Color : Inst2_Color;
};

///////////////////////////////////////////////////////////////////////////////

#define VS_GENERATE \
output.oPosition = input.Position.xyz;\
\
output.Position = WorldPosition(input.Position);\
output.wPosition = output.Position.xyz;\
output.Position = ViewProjection(output.Position);\
output.wvpPosition = output.Position;\
output.wvpPosition_Sub = output.Position;\
\
output.sPosition = WorldPosition(input.Position);\
output.sPosition = mul(output.sPosition, ShadowView);\
output.sPosition = mul(output.sPosition, ShadowProjection);\
\
output.Normal = WorldNormal(input.Normal);\
output.Tangent = WorldTangent(input.Tangent);\
\
output.Uv = input.Uv;\
output.Color = input.Color;\

///////////////////////////////////////////////////////////////////////////////

struct DepthOutput
{
    float4 Position : SV_Position;
    float4 sPosition : Position1;
};

float4 PS_Depth(DepthOutput input) : SV_Target
{
    float depth = input.Position.z / input.Position.w;
    
    return float4(depth, depth, depth, 1.0f);
}

#define VS_DEPTH_GENERATE \
output.Position = WorldPosition(input.Position);\
output.Position = mul(output.Position, ShadowView);\
output.Position = mul(output.Position, ShadowProjection);\
\
output.Position = output.Position;\

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

DepthOutput VS_Depth_Mesh(VertexMesh input)
{
    DepthOutput output;
    
    SetMeshWorld(World, input);
    VS_DEPTH_GENERATE
    
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
    
    uint InstanceID : SV_InstanceID;
    
    matrix Transform : Inst1_Transform;
    float4 Color : Inst2_Color;
};

Texture2DArray TransformsMap;
#define MAX_MODEL_TRANSFORMS 250

cbuffer CB_Bone
{
    // Bone 행렬이 Instance로 넘어오기에 Matrix 배열이 필요 없어졌습니다.
    //matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
    
    uint BoneIndex;
};

void SetModelWorld(inout matrix world, VertexModel input)
{
    // Texture로 보낸 Model 정보 배열에서 데이터를 매칭 해옵니다.
    float4 m0 = TransformsMap.Load(int4(BoneIndex * 4 + 0, input.InstanceID, 0, 0));
    float4 m1 = TransformsMap.Load(int4(BoneIndex * 4 + 1, input.InstanceID, 0, 0));
    float4 m2 = TransformsMap.Load(int4(BoneIndex * 4 + 2, input.InstanceID, 0, 0));
    float4 m3 = TransformsMap.Load(int4(BoneIndex * 4 + 3, input.InstanceID, 0, 0));
    
    // 가져온 데이터들로 World 데이터를 보내줍니다.
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

DepthOutput VS_Depth_Model(VertexModel input)
{
    DepthOutput output;
    
    SetModelWorld(World, input);
    VS_DEPTH_GENERATE
    
    return output;
}

///////////////////////////////////////////////////////////////////////////////
///////////////           Animation                        ////////////////////
///////////////////////////////////////////////////////////////////////////////

#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500

struct AnimationFrame
{
    int Clip;

    uint CurrFrame;
    uint NextFrame;

    float Time;
    float Running;

    float3 Padding;
};

struct TweenFrame
{
    float TakeTime;
    float TweenTime;
    float RunningTime;
    float Padding;

    AnimationFrame Curr;
    AnimationFrame Next;
};

cbuffer CB_TweenFrame
{
    TweenFrame TweenFrames[MAX_MODEL_INSTANCE];
};

void SetTweenWorld(inout matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };
    
    
    int clip[2];
    int currFrame[2];
    int nextFrame[2];
    float time[2];
    
    clip[0] = TweenFrames[input.InstanceID].Curr.Clip;
    currFrame[0] = TweenFrames[input.InstanceID].Curr.CurrFrame;
    nextFrame[0] = TweenFrames[input.InstanceID].Curr.NextFrame;
    time[0] = TweenFrames[input.InstanceID].Curr.Time;
    
    clip[1] = TweenFrames[input.InstanceID].Next.Clip;
    currFrame[1] = TweenFrames[input.InstanceID].Next.CurrFrame;
    nextFrame[1] = TweenFrames[input.InstanceID].Next.NextFrame;
    time[1] = TweenFrames[input.InstanceID].Next.Time;
    
    
    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    matrix curr = 0, next = 0;
    matrix currAnim = 0;
    matrix nextAnim = 0;
    
    matrix transform = 0;
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        // x가 나눠진 Bone, y가 KeyFrame, z가 Clip으로 들어옵니다.
        c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[0], clip[0], 0));
        c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[0], clip[0], 0));
        c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[0], clip[0], 0));
        c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[0], clip[0], 0));
        curr = matrix(c0, c1, c2, c3);
        
        n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], clip[0], 0));
        n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], clip[0], 0));
        n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], clip[0], 0));
        n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], clip[0], 0));
        next = matrix(n0, n1, n2, n3);
        
        currAnim = lerp(curr, next, time[0]);
        
        // 다음 clip 재생
        [flatten]
        if (clip[1] > -1)
        {
            c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[1], clip[1], 0));
            c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[1], clip[1], 0));
            c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[1], clip[1], 0));
            c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[1], clip[1], 0));
            curr = matrix(c0, c1, c2, c3);
        
            n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], clip[1], 0));
            n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], clip[1], 0));
            n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], clip[1], 0));
            n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], clip[1], 0));
            next = matrix(n0, n1, n2, n3);
        
            nextAnim = lerp(curr, next, time[1]);
            
            currAnim = lerp(currAnim, nextAnim, TweenFrames[input.InstanceID].TweenTime);
        }
        
        transform += mul(weights[i], currAnim);
    }
    
    world = mul(transform, input.Transform);
}

struct BlendFrame
{
    uint Mode;
    float Alpha;
    float2 Padding;
    
    AnimationFrame Clip[3];
};

cbuffer CB_BlendFrame
{
    BlendFrame BlendFrames[MAX_MODEL_INSTANCE];
};

void SetBlendWorld(inout matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };
    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    matrix curr = 0, next = 0;
    matrix currAnim[3];
    matrix anim = 0;
    matrix transform = 0;
    
    BlendFrame frame = BlendFrames[input.InstanceID];
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        [unroll(3)]
        for (int k = 0; k < 3; k++)
        {
            c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, frame.Clip[k].CurrFrame, frame.Clip[k].Clip, 0));
            c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, frame.Clip[k].CurrFrame, frame.Clip[k].Clip, 0));
            c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, frame.Clip[k].CurrFrame, frame.Clip[k].Clip, 0));
            c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, frame.Clip[k].CurrFrame, frame.Clip[k].Clip, 0));
            curr = matrix(c0, c1, c2, c3);
        
            n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, frame.Clip[k].NextFrame, frame.Clip[k].Clip, 0));
            n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, frame.Clip[k].NextFrame, frame.Clip[k].Clip, 0));
            n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, frame.Clip[k].NextFrame, frame.Clip[k].Clip, 0));
            n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, frame.Clip[k].NextFrame, frame.Clip[k].Clip, 0));
            next = matrix(n0, n1, n2, n3);
        
            currAnim[k] = lerp(curr, next, frame.Clip[k].Time);
        }
       
        //Blending 강도 (0, 1, 2)
        int clipA = (int) frame.Alpha;
        int clipB = clipA + 1;
        
        float alpha = frame.Alpha;
        if (alpha >= 1.0f)
        {
            // Blending 강도가 1보다 크면 Lerp에 사용하기 위해
            // 1을 빼서 0 ~ 1로 Normalize 한다.
            alpha = frame.Alpha - 1.0f;
            
            if (frame.Alpha >= 2.0f) //2보다 큰 경우에는 재생 구간이 1, 2 사이가 된다.
            {
                clipA = 1;
                clipB = 2;
            }
        }
        
        // Blending 강도에 따라 Animation을 0 ~ 1로 Normalize 된 값으로 Lerp 시킨다.
        anim = lerp(currAnim[clipA], currAnim[clipB], alpha);
        
        transform += mul(weights[i], anim);
    }
    
    world = mul(transform, input.Transform);
}

MeshOutput VS_Animation(VertexModel input)
{
    MeshOutput output;
    
    if (BlendFrames[input.InstanceID].Mode == 0)
        SetTweenWorld(World, input);
    else
        SetBlendWorld(World, input);
    
    VS_GENERATE
    
    return output;
}

DepthOutput VS_Depth_Animation(VertexModel input)
{
    DepthOutput output;
    
    if (BlendFrames[input.InstanceID].Mode == 0)
        SetTweenWorld(World, input);
    else
        SetBlendWorld(World, input);
    
    VS_DEPTH_GENERATE
    
    return output;
}