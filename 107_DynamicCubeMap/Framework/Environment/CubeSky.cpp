#include "Framework.h"
#include "CubeSky.h"

CubeSky::CubeSky(wstring file, Shader* shader)
	: shader(shader)
{
	if (shader == nullptr)
	{
		this->shader = new Shader(L"30_CubeSky.fx");
		bCreateShader = true;
	}


	sphereRender = new MeshRender(this->shader, new MeshSphere(0.5f));
	sphereRender->AddTransform()->Scale(500, 500, 500);

	wstring temp = L"../../_Textures/" + file;
	Check(D3DX11CreateShaderResourceViewFromFile
		(
			D3D::GetDevice(), temp.c_str(), nullptr, nullptr, &srv, nullptr
		));

	sSrv = this->shader->AsSRV("SkyCubeMap");
}

CubeSky::~CubeSky()
{
	if (bCreateShader == true)
		SafeDelete(shader);

	SafeRelease(srv);
	SafeDelete(sphereRender);
}

void CubeSky::Update()
{
	Vector3 position;
	Context::Get()->GetCamera()->Position(&position);
	sphereRender->GetTransform(0)->Position(position);
	sphereRender->UpdateTransforms();
}

void CubeSky::Render()
{
	sSrv->SetResource(srv);

	sphereRender->Pass(pass);
	sphereRender->Render();
}
