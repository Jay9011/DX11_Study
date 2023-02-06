#pragma once
class ModelAnimator
{
public:
    ModelAnimator(Shader* shader);
    ~ModelAnimator();

    void Update();
    void Render();

private:
	void UpdateTweenMode(UINT index);
	void UpdateBlendMode(UINT index);

public:
    void ReadMesh(wstring file);
    void ReadMaterial(wstring file);
    void ReadClip(wstring file);

    Model* GetModel() { return model; }

    void Pass(UINT pass);

	Transform* AddTransform();
	Transform* GetTransform(UINT index) { return transforms[index]; }
	void UpdateTransforms();

	void PlayTweenMode(UINT index, UINT clip, float speed = 1.0f, float takeTime = 1.0f);
	void PlayBlendMode(UINT index, UINT clip, UINT clip1, UINT clip2);
	void SetBlendAlpha(UINT index, float alpha);

private:
	void CreateTexture();
	void CreateClipTransform(UINT index);
	void CreateComputeBuffer();

private:
	struct ClipTransform
	{
		Matrix** Transform;

		ClipTransform()
		{
			Transform = new Matrix * [MAX_MODEL_KEYFRAMES];

			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				Transform[i] = new Matrix[MAX_MODEL_TRANSFORMS];
		}

		~ClipTransform()
		{
			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				SafeDeleteArray(Transform[i]);

			SafeDeleteArray(Transform);
		}
	};
	ClipTransform* clipTransforms = nullptr;

	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

private:
	struct KeyframeDesc
	{
		int Clip = 0;

		UINT CurrFrame = 0;
		UINT NextFrame = 0;

		float Time = 0.0f;
		float RunningTime = 0.0f;

		float Speed = 1.0f;

		Vector2 Padding;
	}; //keyframeDesc;

	ConstantBuffer* frameBuffer;
	ID3DX11EffectConstantBuffer* sFrameBuffer;

	struct TweenDesc
	{
		float TakeTime = 1.0f;	// �ִϸ��̼� ��ȯ �ð�
		float TweenTime = 0.0f;	// ���� ��ȯ�Ǿ� ���� �ð�
		float ChangeTime = 0.0f;
		float Padding;

		KeyframeDesc Curr;	// ������ �� ���� Keyframe
		KeyframeDesc Next;	// ������ �� ���� Keyframe

		TweenDesc()
		{
			Curr.Clip = 0;
			Next.Clip = -1;	// -1 : ���� clip�� ����
		}
	} tweenDesc[MAX_MODEL_INSTANCE];

	struct BlendDesc
	{
		UINT Mode = 0;	// Blending ����
		float Alpha = 0;// Blending ����
		Vector2 Padding;

		KeyframeDesc Clip[3];	// Blending �� animation�� ����Ʈ
	} blendDesc[MAX_MODEL_INSTANCE];

	ConstantBuffer* blendBuffer;
	ID3DX11EffectConstantBuffer* sBlendBuffer;

private:
    Shader* shader;
    Model* model;

	vector<Transform*> transforms;
	Matrix worlds[MAX_MODEL_INSTANCE];

	VertexBuffer* instanceBuffer;

private:
	struct CS_InputDesc
	{
		Matrix Bone;
	};

	struct CS_OutputDesc
	{
		Matrix Result;
	};

	struct AttachDesc
	{
		UINT BoneIndex = 40;
		float Padding[3];
	} attachDesc;

private:
	Shader* computeShader;
	StructuredBuffer* computeBuffer = nullptr;

	CS_InputDesc* csInput = nullptr;
	CS_OutputDesc* csOutput = nullptr;

	ID3DX11EffectShaderResourceVariable* sInputSRV;
	ID3DX11EffectUnorderedAccessViewVariable* sOutputUAV;

	ConstantBuffer* computeAttachBuffer;

	ID3DX11EffectConstantBuffer* sComputeAttachBuffer;
	ID3DX11EffectConstantBuffer* sComputeTweenBuffer;
	ID3DX11EffectConstantBuffer* sComputeBlendBuffer;
};

