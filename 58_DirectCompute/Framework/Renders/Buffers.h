#pragma once
/* * * * * * * * * * * * * * * * * * * * *
* VertexBuffer
* * * * * * * * * * * * * * * * * * * * */
class VertexBuffer
{
public:
    VertexBuffer(void* data, UINT count, UINT stride, UINT slot = 0, bool bCpuWrite = false, bool bGpuWrite = false);
    ~VertexBuffer();

    UINT Count() { return count; }
    UINT Stride() { return stride; }
    ID3D11Buffer* Buffer() { return buffer; }

    void Render();

private:
    ID3D11Buffer* buffer;

    void* data;
    UINT count;
    UINT stride;
    UINT slot;

    bool bCpuWrite;
    bool bGpuWrite;
};

/* * * * * * * * * * * * * * * * * * * * * 
* IndexBuffer
* * * * * * * * * * * * * * * * * * * * */
class IndexBuffer
{
public:
    IndexBuffer(void* data, UINT count);
    ~IndexBuffer();

    UINT Count() { return count; }
    ID3D11Buffer* Buffer() { return buffer; }

    void Render();

private:
    ID3D11Buffer* buffer;

    void* data;
    UINT count;
};

/* * * * * * * * * * * * * * * * * * * * * 
* ConstantBuffer
* * * * * * * * * * * * * * * * * * * * */
class ConstantBuffer
{
public:
    ConstantBuffer(void* data, UINT dataSize);
    ~ConstantBuffer();

    ID3D11Buffer* Buffer() { return buffer; }

    void Render();

private:
    ID3D11Buffer* buffer;

    void* data;
    UINT dataSize;
};

/* * * * * * * * * * * * * * * * * * * * *
* Compute Shader Resource
* * * * * * * * * * * * * * * * * * * * */
class CsResource
{
public:
    CsResource();
    virtual ~CsResource();

protected:
    virtual void CreateInput() {}
    virtual void CreateSRV() {}

    virtual void CreateOutput() {}
    virtual void CreateUAV() {}

    virtual void CreateResult() {}

    void CreateBuffer();

protected:
    ID3D11Resource* input = nullptr;
    ID3D11ShaderResourceView* srv = nullptr;    // input ��

    ID3D11Resource* output = nullptr;
    ID3D11UnorderedAccessView* uav = nullptr;   // output ��

    // CPU ���� ����ϱ� ���� �����͸� �����ɴϴ�. (����)
    ID3D11Resource* result = nullptr;
};

/* * * * * * * * * * * * * * * * * * * * *
* RawBuffer
* * * * * * * * * * * * * * * * * * * * */
class RawBuffer : public CsResource
{
public:
    RawBuffer(void* inputData, UINT inputByte, UINT outputByte);
    ~RawBuffer();

private:
    void CreateInput() override;
    void CreateSRV() override;

    void CreateOutput() override;
    void CreateUAV() override;

    void CreateResult() override;

public:
    void CopyToInput(void* data);
    void CopyFromOuput(void* data);

private:
    // shader�� �ѱ� ������. �ڷ����� ������ ���� �𸣴� void* �� ����մϴ�.
    void* inputData;

    UINT inputByte;
    UINT outputByte;
};