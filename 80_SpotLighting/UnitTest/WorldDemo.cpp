#include "stdafx.h"
#include "WorldDemo.h"

void WorldDemo::Initialize()
{
    shader = new Shader(L"09_World.fx");

    // Local
    vertices[0].Position = Vector3(+0.0f, +0.0f, 0.0f);
    vertices[1].Position = Vector3(+0.0f, +0.5f, 0.0f);
    vertices[2].Position = Vector3(+0.5f, +0.0f, 0.0f);
    
    vertices[3].Position = Vector3(+0.5f, +0.0f, 0.0f);
    vertices[4].Position = Vector3(+0.0f, +0.5f, 0.0f);
    vertices[5].Position = Vector3(+0.5f, +0.5f, 0.0f);

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
    desc.ByteWidth = sizeof(Vertex) * 6;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subResource = { 0 };
    subResource.pSysMem = vertices;

    Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));

    D3DXMatrixIdentity(&world);
}

void WorldDemo::Destroy()
{
    SafeDelete(shader);

    SafeRelease(vertexBuffer);
}

void WorldDemo::Update()
{
    /*
    * world 좌표를 움직이는 것은 공간을 움직이면
    * Rendering 할 기준 공간을 움직이므로
    * Vertex 하나 하나 옮기는 것 보다 훨씬 빠르고 쉽다.
    */
    if (Keyboard::Get()->Press(VK_RIGHT))
    {
        world._11 += 2.0f * Time::Delta();
        world._22 += 2.0f * Time::Delta();
        //world._41 += 2.0f * Time::Delta();
    }
    else if (Keyboard::Get()->Press(VK_LEFT))
    {
        world._11 -= 2.0f * Time::Delta();
        world._22 -= 2.0f * Time::Delta();
        //world._41 -= 2.0f * Time::Delta();
    }
}

void WorldDemo::Render()
{
    shader->AsMatrix("World")->SetMatrix(world);
    shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
    shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    shader->Draw(0, 0, 6);
}