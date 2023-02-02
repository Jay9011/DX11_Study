#include "Framework.h"
#include "Buffers.h"

/* * * * * * * * * * * * * * * * * * * * *
* VertexBuffer
* * * * * * * * * * * * * * * * * * * * */
VertexBuffer::VertexBuffer(void* data, UINT count, UINT stride, UINT slot, bool bCpuWrite, bool bGpuWrite)
    : data(data), count(count), stride(stride), slot(slot)
    , bCpuWrite(bCpuWrite), bGpuWrite(bGpuWrite)
{
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
    desc.ByteWidth = stride * count;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    if (bCpuWrite == false && bGpuWrite == false)
    {
        desc.Usage = D3D11_USAGE_IMMUTABLE; // CPU나 GPU에서 쓰기가 불가능하지만, (GPU 읽기만 가능)
                                            // 가장 빠르다.
    }
    else if (bCpuWrite == true && bGpuWrite == false)
    {
        desc.Usage = D3D11_USAGE_DYNAMIC;   // CPU 쓰기, GPU 읽기
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    else if (bCpuWrite == false && bGpuWrite == true)
    {
        // UpdateSubResource - CPU 쓰기 가능
        desc.Usage = D3D11_USAGE_DEFAULT;   // GPU 에서 쓰기 처리하고 CPU로 보내주기 위해 사용
    }
    else
    {
        desc.Usage = D3D11_USAGE_STAGING;   // CPU 와 GPU 에서 읽고 쓰기 가능 (가장 느리다)
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    }

    D3D11_SUBRESOURCE_DATA subResource = { 0 };
    subResource.pSysMem = data;

    Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &buffer));
}

VertexBuffer::~VertexBuffer()
{
    SafeRelease(buffer);
}

void VertexBuffer::Render()
{
    UINT offset = 0;
    D3D::GetDC()->IASetVertexBuffers(slot, 1, &buffer, &stride, &offset);
}

/* * * * * * * * * * * * * * * * * * * * *
* IndexBuffer
* * * * * * * * * * * * * * * * * * * * */
IndexBuffer::IndexBuffer(void* data, UINT count)
    : data(data), count(count)
{
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

    desc.ByteWidth = sizeof(UINT) * count;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.Usage = D3D11_USAGE_IMMUTABLE;


    D3D11_SUBRESOURCE_DATA subResource = { 0 };
    subResource.pSysMem = data;

    Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &buffer));
}

IndexBuffer::~IndexBuffer()
{
    SafeRelease(buffer);
}

void IndexBuffer::Render()
{
    D3D::GetDC()->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
}

/* * * * * * * * * * * * * * * * * * * * *
* ConstantBuffer
* * * * * * * * * * * * * * * * * * * * */
ConstantBuffer::ConstantBuffer(void* data, UINT dataSize)
    : data(data), dataSize(dataSize)
{
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

    desc.ByteWidth = dataSize;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    Check(D3D::GetDevice()->CreateBuffer(&desc, nullptr, &buffer));
}

ConstantBuffer::~ConstantBuffer()
{
    SafeRelease(buffer);
}

void ConstantBuffer::Render()
{
    D3D11_MAPPED_SUBRESOURCE subResource;
    // MAP을 해주면 반드시 바로 Unmap을 해줘야 GPU가 터지지 않는다.
    D3D::GetDC()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
    {
        memcpy(subResource.pData, data, dataSize);
    }
    D3D::GetDC()->Unmap(buffer, 0);
}

/* * * * * * * * * * * * * * * * * * * * *
* Compute Shader Resource
* * * * * * * * * * * * * * * * * * * * */
CsResource::CsResource()
{
}

CsResource::~CsResource()
{
    SafeRelease(input);
    SafeRelease(srv);

    SafeRelease(output);
    SafeRelease(uav);

    SafeRelease(result);
}

void CsResource::CreateBuffer()
{
    CreateInput();
    CreateSRV();

    CreateOutput();
    CreateUAV();

    CreateResult();
}

/* * * * * * * * * * * * * * * * * * * * *
* RawBuffer
* * * * * * * * * * * * * * * * * * * * */

RawBuffer::RawBuffer(void* inputData, UINT inputByte, UINT outputByte)
    : CsResource()
    , inputData(inputData), inputByte(inputByte), outputByte(outputByte)
{
    CreateBuffer();
}

RawBuffer::~RawBuffer()
{
}

void RawBuffer::CreateInput()
{
    if (inputByte < 1) return;

    ID3D11Buffer* buffer = nullptr;

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

    desc.ByteWidth = inputByte;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    // Shader에서 byte Address로 받을 수 있는 버퍼 용도입니다.
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA subResource = { 0 };
    subResource.pSysMem = inputData;

    Check(D3D::GetDevice()->CreateBuffer(&desc, inputData != nullptr ? &subResource : nullptr, &buffer));

    input = (ID3D11Resource*)buffer;
}

void RawBuffer::CreateSRV()
{
    if (inputByte < 1) return;

    ID3D11Buffer* buffer = (ID3D11Buffer*)input;

    D3D11_BUFFER_DESC desc;
    buffer->GetDesc(&desc);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    // GPU 에서는 기본 Byte는 float 크기인 4byte가 기본 크기 입니다.
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;  // 단, type을 미리 알 수 없기에 Typeless를 사용합니다.
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;   // srv를 Raw 형으로 다루기 위해 사용합니다.
    srvDesc.BufferEx.NumElements = desc.ByteWidth / 4;

    Check(D3D::GetDevice()->CreateShaderResourceView(buffer, &srvDesc, &srv));
}

void RawBuffer::CreateOutput()
{
    ID3D11Buffer* buffer = nullptr;

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

    desc.ByteWidth = outputByte;
    desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

    Check(D3D::GetDevice()->CreateBuffer(&desc, nullptr, &buffer));

    output = (ID3D11Resource*)buffer;
}

void RawBuffer::CreateUAV()
{
    ID3D11Buffer* buffer = (ID3D11Buffer*)output;

    D3D11_BUFFER_DESC desc;
    buffer->GetDesc(&desc);

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
    uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
    uavDesc.Buffer.NumElements = desc.ByteWidth / 4;

    Check(D3D::GetDevice()->CreateUnorderedAccessView(buffer, &uavDesc, &uav));
}

void RawBuffer::CreateResult()
{
    ID3D11Buffer* buffer;

    D3D11_BUFFER_DESC desc;
    // output 데이터가 ID3D11Resource 이므로 사용에 필요한 ID3D11Buffer 형태로 바꾸어서 가져옵니다.
    ((ID3D11Buffer*)output)->GetDesc(&desc);
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.BindFlags = D3D11_USAGE_DEFAULT;
    desc.MiscFlags = 0;

    Check(D3D::GetDevice()->CreateBuffer(&desc, nullptr, &buffer));

    result = (ID3D11Resource*)buffer;
}

// 데이터를 입력용으로 사용합니다.
void RawBuffer::CopyToInput(void* data)
{
    D3D11_MAPPED_SUBRESOURCE subResource;
    D3D::GetDC()->Map(input, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
    {
        memcpy(subResource.pData, data, inputByte);
    }
    D3D::GetDC()->Unmap(input, 0);
}

void RawBuffer::CopyFromOuput(void* data)
{
    // output 의 resource 를 result 에 복사합니다.
    D3D::GetDC()->CopyResource(result, output);

    D3D11_MAPPED_SUBRESOURCE subResource;
    D3D::GetDC()->Map(result, 0, D3D11_MAP_READ, 0, &subResource);
    {
        memcpy(data, subResource.pData, outputByte);
    }
    D3D::GetDC()->Unmap(result, 0);
}
