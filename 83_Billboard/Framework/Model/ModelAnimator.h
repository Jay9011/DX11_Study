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
	UINT GetTransformCount() { return transforms.size(); }

	void PlayTweenMode(UINT index, UINT clip, float speed = 1.0f, float takeTime = 1.0f);
	void PlayBlendMode(UINT index, UINT clip, UINT clip1, UINT clip2);
	void SetBlendAlpha(UINT index, float alpha);

	void GetAttachTransform(UINT instance, Matrix* outResult);

private:
	void CreateTexture();
	void CreateClipTransform(UINT index);

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

	struct TweenDesc
	{
		float TakeTime = 1.0f;	// 애니메이션 전환 시간
		float TweenTime = 0.0f;	// 현재 전환되어 가는 시간
		float ChangeTime = 0.0f;
		float Padding;

		KeyframeDesc Curr;	// 보간해 갈 현재 Keyframe
		KeyframeDesc Next;	// 보간해 갈 다음 Keyframe

		TweenDesc()
		{
			Curr.Clip = 0;
			Next.Clip = -1;	// -1 : 다음 clip이 없음
		}
	} tweenDesc[MAX_MODEL_INSTANCE];

	ConstantBuffer* tweenBuffer;
	ID3DX11EffectConstantBuffer* sTweenBuffer;


	struct BlendDesc
	{
		UINT Mode = 0;	// Blending 여부
		float Alpha = 0;// Blending 강도
		Vector2 Padding;

		KeyframeDesc Clip[3];	// Blending 할 animation들 리스트
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
    Color colors[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceBuffer2;

private:
	struct CS_InputDesc
	{
		Matrix Bone;
	};

	struct CS_OutputDesc
	{
		Matrix Result;
	};
	
private:
	const float frameRate = 30.0f;
	float frameTime = 0.0f;


	Shader* computeShader;
	ID3DX11EffectShaderResourceVariable* sTransformSRV;

	StructuredBuffer* inputWorldBuffer;
	ID3DX11EffectShaderResourceVariable* sInputWorldSRV;

	StructuredBuffer* inputBoneBuffer;
	ID3DX11EffectShaderResourceVariable* sInputBoneSRV;

	TextureBuffer* outputBuffer;
	ID3DX11EffectUnorderedAccessViewVariable* sOutputUAV;


	ID3DX11EffectConstantBuffer* sComputeTweenBuffer;
	ID3DX11EffectConstantBuffer* sComputeBlendBuffer;
};

