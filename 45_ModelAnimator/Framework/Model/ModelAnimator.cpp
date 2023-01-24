#include "Framework.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(Shader* shader)
    : shader(shader)
{
    model = new Model();
    transform = new Transform(shader);

    frameBuffer = new ConstantBuffer(&tweenDesc, sizeof(TweenDesc));
    sFrameBuffer = shader->AsConstantBuffer("CB_TweenFrame");
}

ModelAnimator::~ModelAnimator()
{
    SafeDelete(model);
    SafeDelete(transform);

    SafeDeleteArray(clipTransforms);
    SafeRelease(texture);
    SafeRelease(srv);

    SafeDelete(frameBuffer);
}

void ModelAnimator::Update()
{
    TweenDesc& desc = tweenDesc;

    // 현재 진행중인 애니메이션
    {
        ModelClip* clip = model->ClipByIndex(desc.Curr.Clip);

        desc.Curr.RunningTime += Time::Delta();

        float time = 1.0f / clip->FrameRate() / desc.Curr.Speed;
        // 일정 시간이 지나면 다음 frame 진행
        if (desc.Curr.Time >= 1.0f)
        {
            desc.Curr.RunningTime = 0;

            desc.Curr.CurrFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount();
            desc.Curr.NextFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount();
        }
        desc.Curr.Time = desc.Curr.RunningTime / time;
    }

    // 전환 할 다음 동작도 플레이 시켜줘야 한다.
    if (desc.Next.Clip > -1) // clip이 존재하는 경우에만...
    {
        desc.ChangeTime += Time::Delta();
        desc.TweenTime = desc.ChangeTime / desc.TakeTime;

        // 전환이 완료된 경우
        if (desc.TweenTime >= 1.0f)
        {
            desc.Curr = desc.Next;

            desc.Next.Clip = -1;
            desc.Next.CurrFrame = 0;
            desc.Next.NextFrame = 0;
            desc.Next.Time = 0;
            desc.Next.RunningTime = 0.0f;

            desc.ChangeTime = 0.0f;
            desc.TweenTime = 0.0f;
        }
        // 전환하는 중, frame 진행
        else
        {
            ModelClip* clip = model->ClipByIndex(desc.Next.Clip);

            desc.Next.RunningTime += Time::Delta();

            float time = 1.0f / clip->FrameRate() / desc.Next.Speed;
            if (desc.Next.Time >= 1.0f)
            {
                desc.Next.RunningTime = 0;

                desc.Next.CurrFrame = (desc.Next.CurrFrame + 1) % clip->FrameCount();
                desc.Next.NextFrame = (desc.Next.CurrFrame + 1) % clip->FrameCount();
            }
            desc.Next.Time = desc.Next.RunningTime / time;
        }
    }

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
    frameBuffer->Render();
    sFrameBuffer->SetConstantBuffer(frameBuffer->Buffer());

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

void ModelAnimator::PlayTweenMode(UINT clip, float speed, float takeTime)
{
    tweenDesc.TakeTime = takeTime;

    tweenDesc.Next.Clip = clip;
    tweenDesc.Next.Speed = speed;
}

void ModelAnimator::CreateTexture()
{
    clipTransforms = new ClipTransform[model->ClipCount()];
    for (UINT i = 0; i < model->ClipCount(); i++)
        CreateClipTransform(i);

    //Create Texture
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
        desc.Width = MAX_MODEL_TRANSFORMS * 4;
        desc.Height = MAX_MODEL_KEYFRAMES;
        desc.ArraySize = model->ClipCount();
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //16Byte * 4 = 64Byte
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;

        UINT pageSize = MAX_MODEL_TRANSFORMS * 4 * 16 * MAX_MODEL_KEYFRAMES;
        void* p = VirtualAlloc(nullptr, pageSize * model->ClipCount(), MEM_RESERVE, PAGE_READWRITE);

        // MEMORY_BASIC_INFORMATION, VirtualQuery 로 예약한 사이즈를 확인할 수 있다.

        for (UINT c = 0; c < model->ClipCount(); c++)
        {
            UINT start = c * pageSize;

            for (UINT k = 0; k < MAX_MODEL_KEYFRAMES; k++)
            {
                void* temp = (BYTE*)p + MAX_MODEL_TRANSFORMS * k * sizeof(Matrix) + start;

                VirtualAlloc(temp, MAX_MODEL_TRANSFORMS * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
                memcpy(temp, clipTransforms[c].Transform[k], MAX_MODEL_TRANSFORMS * sizeof(Matrix));
            }//for(MAX_MODEL_KEYFRAMES)
        }//for(model->ClipCount())

        D3D11_SUBRESOURCE_DATA* subResources = new D3D11_SUBRESOURCE_DATA[model->ClipCount()];
        for (UINT c = 0; c < model->ClipCount(); c++)
        {
            void* temp = (BYTE*)p + c * pageSize;

            subResources[c].pSysMem = temp;
            subResources[c].SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
            subResources[c].SysMemSlicePitch = pageSize;
        }//for(model->ClipCount())
        Check(D3D::GetDevice()->CreateTexture2D(&desc, subResources, &texture));

        SafeDeleteArray(subResources);
        VirtualFree(p, 0, MEM_RELEASE);
    }

    //Create SRV
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        desc.Texture2DArray.MipLevels = 1;
        desc.Texture2DArray.ArraySize = model->ClipCount();

        Check(D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &srv));
    }

    for (ModelMesh* mesh : model->Meshes())
        mesh->TransformsSRV(srv);
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
