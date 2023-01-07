#pragma once

class Mesh : public Renderer
{
public:
    typedef VertexTextureNormal MeshVertex;

public:
    Mesh(Shader* shader);
    virtual ~Mesh();

    void Update();
    void Render();

public:
    void DiffuseMap(wstring file);

protected:
    virtual void Create() = 0;

protected:
    MeshVertex* vertices = nullptr;
    UINT*       indices  = nullptr;

private:
    Texture* diffuseMap = nullptr;
    ID3DX11EffectShaderResourceVariable* sDiffuseMap;
};

