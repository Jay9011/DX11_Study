#include "Framework.h"
#include "Render2D.h"

Render2D::Render2D()
	: Renderer(L"62_Render2D.fx")
{
	// 전방 방향을 바라보는 2D로 렌더링 합니다.
	D3DXMatrixLookAtLH(&desc.View, &Vector3(0, 0, -1), &Vector3(0, 0, 0), &Vector3(0, 1, 0));
	D3DXMatrixOrthoOffCenterLH(&desc.Projection, 0, D3D::Width(), 0, D3D::Height(), -1, +1);
	// Ortho : 크기만으로 중앙점을 알아서 계산합니다.
	// OrthoOffCenterLH : 사용자가 직접 크기와 깊이를 넘겨줍니다.

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	shader->AsConstantBuffer("CB_Render2D")->SetConstantBuffer(buffer->Buffer());

	VertexTexture vertices[6];
	vertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[3].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[4].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[5].Position = Vector3(+0.5f, +0.5f, 0.0f);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 1);
	vertices[4].Uv = Vector2(0, 0);
	vertices[5].Uv = Vector2(1, 0);

	vertexBuffer = new VertexBuffer(vertices, 6, sizeof(VertexTexture));
	sDiffuseMap = shader->AsSRV("DiffuseMap");
}

Render2D::~Render2D()
{
	SafeDelete(buffer);
}

void Render2D::Update()
{
	Super::Update();
}

void Render2D::Render()
{
	Super::Render();

	buffer->Render();
	shader->Draw(0, Pass(), 6);
}

void Render2D::SRV(ID3D11ShaderResourceView* srv)
{
	sDiffuseMap->SetResource(srv);
}
