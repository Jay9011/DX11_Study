#include "Framework.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(Shader* shader)
{
    model = new Model();
    transform = new Transform(shader);
}

ModelAnimator::~ModelAnimator()
{
    SafeDelete(model);
    SafeDelete(transform);
}

void ModelAnimator::Update()
{
    if (texture == nullptr)
    {
        for (ModelMesh* mesh : model->Meshes())
            mesh->SetShader(shader);

        CreateTexture();
    }

    for (ModelMesh* mesh : model->Meshes())
        mesh->Update();
}

void ModelAnimator::Render()
{
    for (ModelMesh* mesh : model->Meshes())
    {
        mesh->SetTransform(transform);
        mesh->Render();
    }
}

void ModelAnimator::ReadMesh(wstring file)
{
    model->ReadMesh(file);
}

void ModelAnimator::ReadMaterial(wstring file)
{
    model->ReadMaterial(file);
}

void ModelAnimator::ReadClip(wstring file)
{
    model->ReadClip(file);
}

void ModelAnimator::Pass(UINT pass)
{
    for (ModelMesh* mesh : model->Meshes())
        mesh->Pass(pass);
}

void ModelAnimator::CreateTexture()
{
    clipTransforms = new ClipTransform[model->ClipCount()];
    for (UINT i = 0; i < model->ClipCount(); i++)
        CreateClipTransform(i);
}

void ModelAnimator::CreateClipTransform(UINT index)
{
    Matrix* bones = new Matrix[MAX_MODEL_TRANSFORMS];

    ModelClip* clip = model->ClipByIndex(index);
    for (UINT f = 0; f < clip->FrameCount(); f++)
    {
        for (UINT b = 0; b < model->BoneCount(); b++)
        {
            ModelBone* bone = model->BoneByIndex(b);

            Matrix parent;
            Matrix invGlobal = bone->Transform();
            D3DXMatrixInverse(&invGlobal, nullptr, &invGlobal);

            int parentIndex = bone->ParentIndex();
            if (parentIndex < 0)
                D3DXMatrixIdentity(&parent);
            else
                parent = bones[parentIndex];

            Matrix animation;
            ModelKeyframe* frame = clip->Keyframe(bone->Name());

            if (frame != nullptr)
            {
                ModelKeyframeData& data = frame->Transforms[f];

                Matrix S, R, T;
                D3DXMatrixScaling(&S, data.Scale.x, data.Scale.y, data.Scale.z);
                D3DXMatrixRotationQuaternion(&R, &data.Rotation);
                D3DXMatrixTranslation(&T, data.Translation.x, data.Translation.y, data.Translation.z);

                animation = S * R * T;
            }
            else
            {
                D3DXMatrixIdentity(&animation);
            }

            bones[b] = animation * parent;
            clipTransforms[index].Transform[f][b] = invGlobal * bones[b];
        }//for(b)
    }//for(f)
}
