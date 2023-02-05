#pragma once
class ModelRender
{
public:
    ModelRender(Shader* shader);
    ~ModelRender();

    void Update();
    void Render();

public:
    void ReadMesh(wstring file);
    void ReadMaterial(wstring file);

    Model* GetModel() { return model; }

    void Pass(UINT pass);

    Transform* AddTransform();
    Transform* GetTransform(UINT index) { return transforms[index]; }
    void UpdateTransforms();    // 전체 위치 GPU로 보내주기 위함

    void UpdateTransform(UINT instanceId, UINT boneIndex, Transform& transform);

private:
    void CreateTexture();

private:
    Shader* shader;
    Model* model;

    // Instancing을 위한 목록
    vector<Transform*> transforms;
    Matrix worlds[MAX_MODEL_INSTANCE];

    VertexBuffer* instanceBuffer;

    // Texture 2D로 보낼 2차원 배열
    Matrix boneTransforms[MAX_MODEL_INSTANCE][MAX_MODEL_TRANSFORMS];

    ID3D11Texture2D* texture = nullptr;
    ID3D11ShaderResourceView* srv;
};

