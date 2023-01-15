#pragma once

class Converter
{
public:
    Converter();
    ~Converter();

    void ReadFile(wstring file);

public:
    void ExportMesh(wstring savePath);

private:
    void ReadBoneData(aiNode* node, int index, int parent);
    void ReadMeshData(aiNode* node, int bone);
    void ReadSkinData();
    void WriteMeshData(wstring savePath);

public:
    void ExportMaterial(wstring savePath, bool isOverwrite = true);

private:
    void ReadMaterialData();
    void WriteMaterialData(wstring savePath);
    string WriteTexture(string saveFolder, string file);

private:
    wstring file;

    Assimp::Importer* importer;
    const aiScene* scene;

    vector<struct asBone*> bones;
    vector<struct asMesh*> meshes;
    vector<struct asMaterial*> materials;
};