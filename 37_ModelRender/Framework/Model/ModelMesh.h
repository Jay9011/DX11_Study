#pragma once
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*       Model Bone
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
class ModelBone
{
public:
    friend class Model;

private:    // 다른 곳에서는 생성하지 못하고 Model 에서만 생성할 수 있게
    ModelBone();
    ~ModelBone();

public:
    int Index() { return index; }

    int ParentIndex() { return parentIndex; }
    ModelBone* Parent() { return parent; }

    wstring Name() { return name; }

    Matrix& Transform() { return transform; }
    void Transform(Matrix& matrix) { transform = matrix; }

    vector<ModelBone*>& Childs() { return childs; }

private:
    int index;
    wstring name;

    int parentIndex;
    ModelBone* parent;

    Matrix transform;
    vector<ModelBone*> childs;

};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*       Model Mesh
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
class ModelMesh
{
public:
    friend class Model;

private:
    ModelMesh();
    ~ModelMesh();

    void Binding(Model* model);

public:
    void Pass(UINT val) { pass = val; }
    void SetShader(Shader* shader);

    void Update();
    void Render();

    wstring Name() { return name; }

    int BoneIndex() { return boneIndex; }
    class ModelBone* Bone() { return bone; }

    void Transforms(Matrix* transforms);
    void SetTransform(Transform* transform);

private:
    struct BoneDesc
    {
        Matrix Transforms[MAX_MODEL_TRANSFORMS];

        UINT Index;
        float Padding[3];
    } boneDesc;

private:
    wstring name;

    Shader* shader;
    UINT pass = 0;

    Transform* transform = nullptr;
    PerFrame* perFrame = nullptr;

    wstring materialName = L"";
    Material* material;

    int boneIndex;
    class ModelBone* bone;

    VertexBuffer* vertexBuffer;
    UINT vertexCount;
    Model::ModelVertex* vertices;

    IndexBuffer* indexBuffer;
    UINT indexCount;
    UINT* indices;

    ConstantBuffer* boneBuffer;
    ID3DX11EffectConstantBuffer* sBoneBuffer;
};