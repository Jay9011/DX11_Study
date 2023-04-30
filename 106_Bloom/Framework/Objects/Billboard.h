#pragma once
#define MAX_BILLBOARD_COUNT 10000

class Billboard : public Renderer
{
public:
	Billboard(Shader* shader);
	~Billboard() override;

	void Update() override;
	void Render() override;

	void Add(const Vector3& position, const Vector2& scale, UINT mapIndex);
	void AddTexture(wstring file);


private:
	struct VertexBillboard
	{
		Vector3 Position;
		Vector2 Scale;
		UINT MapIndex;
	};

private:
	vector<VertexBillboard> vertices;

	vector<wstring> textureNames;
	TextureArray* textureArray = nullptr;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;

};
