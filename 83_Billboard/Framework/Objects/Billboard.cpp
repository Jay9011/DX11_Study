#include "Framework.h"
#include "Billboard.h"
#include "Billboard.h"

Billboard::Billboard(wstring file)
	: Renderer(shader)
{
	UINT vertexCount = MAX_BILLBOARD_COUNT * 4;
	vertices = new VertexBillboard[vertexCount];
	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexBillboard), 0, true);


	UINT indexCount = MAX_BILLBOARD_COUNT * 6;
	indices = new UINT[indexCount];

	for(UINT i = 0; i < MAX_BILLBOARD_COUNT; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 3;
		indices[i * 6 + 4] = i * 4 + 4;
		indices[i * 6 + 5] = i * 4 + 5;
	}

	indexBuffer = new IndexBuffer(indices, indexCount);

}

Billboard::~Billboard()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);
}

void Billboard::Update()
{
}

void Billboard::Render()
{
}

void Billboard::Add(const Vector3& position, const Vector2& scale)
{
	vertices[drawCount * 4 + 0].Position = position;
	vertices[drawCount * 4 + 1].Position = position;
	vertices[drawCount * 4 + 2].Position = position;
	vertices[drawCount * 4 + 3].Position = position;

	vertices[drawCount * 4 + 0].Uv = Vector2(0, 1);
	vertices[drawCount * 4 + 1].Uv = Vector2(0, 0);
	vertices[drawCount * 4 + 2].Uv = Vector2(1, 1);
	vertices[drawCount * 4 + 3].Uv = Vector2(1, 0);

	vertices[drawCount * 4 + 0].Scale = scale;
	vertices[drawCount * 4 + 1].Scale = scale;
	vertices[drawCount * 4 + 2].Scale = scale;
	vertices[drawCount * 4 + 3].Scale = scale;

	++drawCount;
}
