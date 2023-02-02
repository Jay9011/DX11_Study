ByteAddressBuffer Input;    // SRV 에 연결 됩니다.
RWByteAddressBuffer Output; // UAV 에 연결 됩니다.

struct Group
{
    uint3 GroupID;
    uint3 GroupThreadID;
    uint3 DispatchThreadID;
    uint  GroupIndex;
    float RetValue;
};

struct ComputeInput
{
    uint3 GroupID : SV_GroupID;
    uint3 GroupThreadID : SV_GroupThreadID;
    uint3 DispatchThreadID : SV_DispatchThreadID;
    uint  GroupIndex : SV_GroupIndex;
};

[numthreads(10, 8, 3)]
void CS(ComputeInput input)
{
    Group group;
    group.GroupID = asuint(input.GroupID);
    group.GroupThreadID = asuint(input.GroupThreadID);
    group.DispatchThreadID = asuint(input.DispatchThreadID);
    group.GroupIndex = asuint(input.GroupIndex);
    
    // Thread Group의 x 수와 한 Group의 Thread 만큼 계산합니다.
    uint index = input.GroupID.x * 10 * 8 * 3 + input.GroupIndex;
    // threadGroupIndex번호 * uint의 개수 * 4byte (한 Thread당 처리하는 데이터 크기)
    uint outAddress = index * 11 * 4;
    
    uint inAddress = index * 4;
    group.RetValue = asfloat(Input.Load(inAddress));
    
    
    Output.Store3(outAddress + 0, asuint(group.GroupID)); // 12byte 까지 사용
    Output.Store3(outAddress + 12, asuint(group.GroupThreadID)); // 24byte 까지 사용
    Output.Store3(outAddress + 24, asuint(group.DispatchThreadID)); // 36byte 까지 사용
    Output.Store(outAddress + 36, asuint(group.GroupIndex)); // 40byte 까지 사용
    Output.Store(outAddress + 40, asuint(group.RetValue)); // 44byte 까지 사용
}

technique11 T0
{
    pass p0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}