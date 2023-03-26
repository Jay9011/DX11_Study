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
    void UpdateTransforms();    // ��ü ��ġ GPU�� �����ֱ� ����

    void UpdateTransform(UINT instanceId, UINT boneIndex, Transform& transform);

private:
    void CreateTexture();

private:
    Shader* shader;
    Model* model;

    // Instancing�� ���� ���
    vector<Transform*> transforms;
    Matrix worlds[MAX_MODEL_INSTANCE];

    VertexBuffer* instanceBuffer;

    // Texture 2D�� ���� 2���� �迭
    Matrix boneTransforms[MAX_MODEL_INSTANCE][MAX_MODEL_TRANSFORMS];

    ID3D11Texture2D* texture = nullptr;
    ID3D11ShaderResourceView* srv;
};

