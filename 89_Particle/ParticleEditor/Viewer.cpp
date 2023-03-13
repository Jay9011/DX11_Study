#include "stdafx.h"
#include "Viewer.h"

void Viewer::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, -20, 0);
    Context::Get()->GetCamera()->Position(1, 36, -85);
	((Freedom*)Context::Get()->GetCamera())->Speed(50, 2);

    shader = new Shader(L"82_NormalMapping.fx");

    sky = new CubeSky(L"Environment/GrassCube1024.dds");

	Mesh();

}

void Viewer::Destroy()
{

}

void Viewer::Update()
{
	sky->Update();

	sphere->Update();
	grid->Update();
}

void Viewer::Render()
{
	sky->Render();

	stone->Render();
	sphere->Render();

	floor->Render();
	grid->Render();
}

void Viewer::Mesh()
{
	floor = new Material(shader);
	floor->DiffuseMap("Floor.png");
	floor->SpecularMap("Floor_Specular.png");
	floor->NormalMap("Floor_Normal.png");
	floor->Specular(1, 1, 1, 20);

	stone = new Material(shader);
	stone->DiffuseMap("Stones.png");
	stone->SpecularMap("Stones_Specular.png");
	stone->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
	stone->NormalMap("Stones_Normal.png");
	stone->Specular(1, 1, 1, 20);


	Transform* transform = nullptr;

	grid = new MeshRender(shader, new MeshGrid(5, 5));
	transform = grid->AddTransform();
	transform->Position(0, 0, 0);
	transform->Scale(12, 1, 12);
	grid->UpdateTransforms();

	sphere = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
	transform = sphere->AddTransform();
	transform->Position(0, 5, 0);
	transform->Scale(5, 5, 5);
	sphere->UpdateTransforms();

}
