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
