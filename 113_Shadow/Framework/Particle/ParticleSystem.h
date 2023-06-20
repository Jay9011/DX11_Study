#pragma once

class ParticleSystem : public Renderer
{
public:
	ParticleSystem(wstring filename);
	virtual ~ParticleSystem() override;

	void Reset();
	void Add(const Vector3& position);

public:
	void Update();
	void Render();

	ParticleData& GetData() { return data; }
	void SetTexture(wstring file);

private:
	void MapVertices();
	void Activate();
	void Deactivate();

private:
	void ReadFile(wstring file);

private:
	struct VertexParticle
	{
		Vector3 Position;
		Vector2 Corner;	// (-1 ~ +1)
		Vector3 Velocity;
		Vector4 Random;	// x:주기, y:크기, z:회전, w:색상
		float Time;
	};

	struct Desc
	{
		Color MinColor;
		Color MaxColor;

		Vector3 Gravity;
		float EndVelocity;

		Vector2 StartSize;
		Vector2 EndSize;

		Vector2 RotateSpeed;
		float ReadyTime;
		float ReadyRandomTime;

		float ColorAmount;
		float CurrentTime;

		float Padding[2];
	} desc;

private:
	ParticleData data;

	Texture* map = nullptr;
	ID3DX11EffectShaderResourceVariable* sMap;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	VertexParticle* vertices = nullptr;
	UINT* indices = nullptr;

	float currentTime = 0.0f;
	float lastAddTime = 0.0f;

	UINT leadCount		= 0;
	UINT gpuCount		= 0;
	UINT activeCount	= 0;
	UINT deactiveCount	= 0;
};
