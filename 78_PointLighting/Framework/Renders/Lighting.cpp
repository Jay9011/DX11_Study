#include "Framework.h"
#include "Lighting.h"

Lighting* Lighting::instance = nullptr;

Lighting* Lighting::Get()
{
	assert(instance != nullptr);

	return instance;
}

void Lighting::Create()
{
	assert(instance == nullptr);

	instance = new Lighting();
}

void Lighting::Delete()
{
	SafeDelete(instance);
}

Lighting::Lighting()
{

}

Lighting::~Lighting()
{

}

UINT Lighting::PointLights(OUT PointLight* lights)
{
	memcpy(lights, pointLights, sizeof(PointLight) * pointLightCount);

	return pointLightCount;
}

void Lighting::AddPointLight(PointLight& light)
{
	pointLights[pointLightCount] = light;
	pointLightCount++;
}

PointLight& Lighting::GetPointLight(UINT index)
{
	return pointLights[index];
}