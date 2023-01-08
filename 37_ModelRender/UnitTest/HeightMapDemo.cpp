#include "stdafx.h"
#include "HeightMapDemo.h"

void HeightMapDemo::Initialize()
{
    Context::Get()->GetCamera()->RotationDegree(12.f, 0.f, 0.f);
    Context::Get()->GetCamera()->Position(35.f, 10.f, -55.f);

    shader = new Shader(L"19_Terrain.fx");

    //terrain = new Terrain(shader, L"Terrain/Grayscale.png");
    terrain = new Terrain(shader, L"Terrain/Gray256.png");
    terrain->Pass(1);
}

void HeightMapDemo::Destroy()
{
    SafeDelete(shader);

    SafeDelete(terrain);
}

void HeightMapDemo::Update()
{
    terrain->Update();
}

void HeightMapDemo::Render()
{
    terrain->Render();
}