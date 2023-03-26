#pragma once
#define MAX_BILLBOARD_COUNT 10000

class Billboard : public Renderer
{
public:
	Billboard(Shader* shader);
	~Billboard() override;

	void Update() override;
	void Render() override;

	void Add(const Vector3& position, const Vector2& scale);
	void SetTexture(wstring file);


private:
	struct VertexBillboard
	{
		Vector3 Position;
		Vector2 Scale;
	};

private:
	vector<VertexBillboard> vertices;
	
	Texture* texture = nullptr;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;

};
