#include "stdafx.h"
#include "VertexLine.h"

void VertexLine::Initialize()
{
    shader = new Shader(L"01_Vertex.fx");

    vertices[0].Position = Vector3(0.f, 0.f, 0.f);
    vertices[1].Position = Vector3(1.f, 0.f, 0.f);

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
    desc.ByteWidth = sizeof(Vertex) * 2;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subResource = { 0 };
    subResource.pSysMem = vertices;

    Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
}

void VertexLine::Destroy()
{
    SafeDelete(shader);
    SafeRelease(vertexBuffer);
}

void VertexLine::Update()
{
}

void VertexLine::Render()
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    shader->Draw(1, 0, 2);
}