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
    void Render(UINT drawCount);

    wstring Name() { return name; }

    int BoneIndex() { return boneIndex; }
    class ModelBone* Bone() { return bone; }

    void TransformsSRV(ID3D11ShaderResourceView* srv) { transformsSRV = srv; }

private:
    struct BoneDesc
    {
        UINT Index;
        float Padding[3];
    } boneDesc;

private:
    wstring name;

    Shader* shader;
    UINT pass = 0;

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

    ID3D11ShaderResourceView* transformsSRV = nullptr;
    ID3DX11EffectShaderResourceVariable* sTransformsSRV;
};