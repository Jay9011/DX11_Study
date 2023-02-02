ByteAddressBuffer Input;    // SRV �� ���� �˴ϴ�.
RWByteAddressBuffer Output; // UAV �� ���� �˴ϴ�.

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
    
    // Thread Group�� x ���� �� Group�� Thread ��ŭ ����մϴ�.
    uint index = input.GroupID.x * 10 * 8 * 3 + input.GroupIndex;
    // threadGroupIndex��ȣ * uint�� ���� * 4byte (�� Thread�� ó���ϴ� ������ ũ��)
    uint outAddress = index * 11 * 4;
    
    uint inAddress = index * 4;
    group.RetValue = asfloat(Input.Load(inAddress));
    
    
    Output.Store3(outAddress + 0, asuint(group.GroupID)); // 12byte ���� ���
    Output.Store3(outAddress + 12, asuint(group.GroupThreadID)); // 24byte ���� ���
    Output.Store3(outAddress + 24, asuint(group.DispatchThreadID)); // 36byte ���� ���
    Output.Store(outAddress + 36, asuint(group.GroupIndex)); // 40byte ���� ���
    Output.Store(outAddress + 40, asuint(group.RetValue)); // 44byte ���� ���
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