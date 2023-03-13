#pragma once
#define MAX_BILLBOARD_COUNT 10000

class Billboard : public Renderer
{
public:
	Billboard(wstring file);
	~Billboard() override;

	void Update() override;
	void Render() override;

	void Add(const Vector3& position, const Vector2& scale);


private:
	struct VertexBillboard
	{
		Vector3 Position;
		Vector2 Uv;
		Vector2 Scale;
	};

private:
	VertexBillboard* vertices = nullptr;
	UINT* indices = nullptr;

	UINT drawCount = 0;
	UINT prevCount = 0;

	Texture* texture = nullptr;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;

};
