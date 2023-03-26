#include "Framework.h"
#include "Billboard.h"
#include "Billboard.h"

Billboard::Billboard(Shader* shader)
	: Renderer(shader)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	sDiffuseMap = shader->AsSRV("DiffuseMap");
}

Billboard::~Billboard()
{
	SafeDelete(texture);
}

void Billboard::Update()
{
	Super::Update();
}

void Billboard::Render()
{
	if (vertexCount != vertices.size())
	{
		vertexCount = vertices.size();

		SafeDelete(vertexBuffer);
		vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(VertexBillboard));
	}

	Super::Render();


	sDiffuseMap->SetResource(texture->SRV());
	shader->Draw(0, Pass(), vertexCount);
}

void Billboard::Add(const Vector3& position, const Vector2& scale)
{
	VertexBillboard vertex =
		{
			position, scale
		};

	vertices.push_back(vertex);
}

void Billboard::SetTexture(wstring file)
{
	SafeDelete(texture);

	texture = new Texture(file);
}
