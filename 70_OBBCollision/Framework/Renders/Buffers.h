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

public:
    ID3D11ShaderResourceView* SRV() { return srv; }
    ID3D11UnorderedAccessView* UAV() { return uav; }

protected:
    ID3D11Resource* input = nullptr;
    ID3D11ShaderResourceView* srv = nullptr;    // input 용

    ID3D11Resource* output = nullptr;
    ID3D11UnorderedAccessView* uav = nullptr;   // output 용

    // CPU 에서 사용하기 위해 데이터를 가져옵니다. (복사)
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
    // shader로 넘길 데이터. 자료형이 무엇이 될지 모르니 void* 를 사용합니다.
    void* inputData;

    UINT inputByte;
    UINT outputByte;
};

/* * * * * * * * * * * * * * * * * * * * *
* TextureBuffer
* * * * * * * * * * * * * * * * * * * * */
class TextureBuffer : public CsResource
{
public:
    TextureBuffer(ID3D11Texture2D* src);
    ~TextureBuffer();

private:
    void CreateSRV() override;

    void CreateOutput() override;
    void CreateUAV() override;

    void CreateResult() override;

public:
    UINT Width() { return width; }
    UINT Height() { return height; }
    UINT ArraySize() { return arraySize; }

    ID3D11Texture2D* Output() { return (ID3D11Texture2D*)output; }
    ID3D11ShaderResourceView* OutputSRV() { return outputSRV; }

private:
    UINT width, height, arraySize;
    DXGI_FORMAT format;

    ID3D11ShaderResourceView* outputSRV;
};

/* * * * * * * * * * * * * * * * * * * * *
* StructuredBuffer
* * * * * * * * * * * * * * * * * * * * */
class StructuredBuffer : public CsResource
{
public:
    StructuredBuffer(void* inputData, UINT inputStride, UINT inputCount, UINT outputStride = 0, UINT outputCount = 0);
    ~StructuredBuffer();

private:
    void CreateInput() override;
    void CreateSRV() override;

    void CreateOutput() override;
    void CreateUAV() override;

    void CreateResult() override;

public:
    UINT InputByteWidth() { return inputStride * inputCount; }
    UINT OutputByteWidth() { return outputStride * outputCount; }

    void CopyToInput(void* data);
    void CopyFromOutput(void* data);

private:
    void* inputData;

    UINT inputStride;
    UINT inputCount;

    UINT outputStride;
    UINT outputCount;
};