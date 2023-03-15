﻿#include "Framework.h"
#include "ParticleSystem.h"

#include "Utilities/Xml.h"


ParticleSystem::ParticleSystem(wstring filename)
	: Renderer(L"89_Particle.fx")
{
	ReadFile(L"../../_Textures/Particles/" + filename + L".xml");

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Particle");

	sMap = shader->AsSRV("ParticleMap");

	Reset();
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Reset()
{
	currentTime = 0.0f;
	lastAddTime = Time::Get()->Running();
	gpuCount = leadCount = activeCount = deactiveCount = 0;


	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	vertices = new VertexParticle[data.MaxParticles * 4];
	for (UINT i = 0; i < data.MaxParticles; i++)
	{
		vertices[i * 4 + 0].Corner = Vector2(-1, -1);
		vertices[i * 4 + 1].Corner = Vector2(-1, +1);
		vertices[i * 4 + 2].Corner = Vector2(+1, -1);
		vertices[i * 4 + 3].Corner = Vector2(+1, +1);
	}

	indices = new UINT[data.MaxParticles * 6];
	for (UINT i = 0; i < data.MaxParticles; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 1;
		indices[i * 6 + 5] = i * 4 + 3;
	}

	vertexBuffer = new VertexBuffer(vertices, data.MaxParticles * 4, sizeof(VertexParticle), 0, true);
	indexBuffer = new IndexBuffer(indices, data.MaxParticles * 6);
}

void ParticleSystem::Add(const Vector3& position)
{
	if(Time::Get()->Running() - lastAddTime < 60.0f / 1000.0f)
		return;

	lastAddTime = Time::Get()->Running();

	UINT count = leadCount + 1;

	if(count >= data.MaxParticles)
		count = 0;

	if(count == deactiveCount)
		return;


	Vector3 velocity = Vector3(1, 1, 1);
	velocity *= data.StartVelocity;

	float horizontalVelocity = Math::Lerp(data.MinHorizontalVelocity, data.MaxHorizontalVelocity, Math::Random(0.0f, 1.0f));
	float horizontalAngle = Math::PI * 2.0f * Math::Random(0.0f, 1.0f);

	velocity.x += horizontalAngle * cosf(horizontalAngle);
	velocity.y += horizontalAngle * sinf(horizontalAngle);
	velocity.z += Math::Lerp(data.MinHorizontalVelocity, data.MaxHorizontalVelocity, Math::Random(0.0f, 1.0f));

	Vector4 random = Math::RandomVec4(0.0f, 1.0f);

	for (UINT i = 0; i < 4; i++)
	{
		vertices[leadCount * 4 + i].Position = position;
		vertices[leadCount * 4 + i].Velocity = velocity;
		vertices[leadCount * 4 + i].Random = random;
		vertices[leadCount * 4 + i].Time = currentTime;
	}

	leadCount = count;
}

void ParticleSystem::ReadFile(wstring file)
{
	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();

	Xml::XMLElement* node = root->FirstChildElement();
	data.Type = (ParticleData::BlendType)node->IntText();
	
	node = node->NextSiblingElement();
	wstring textureFile = String::ToWString(node->GetText());
	data.TextureFile = L"Particles/" + textureFile;
	map = new Texture(data.TextureFile);

	node = node->NextSiblingElement();
	data.bLoop = node->BoolText();

	node = node->NextSiblingElement();
	data.MaxParticles = node->IntText();

	node = node->NextSiblingElement();
	data.ReadyTime = node->FloatText();

	node = node->NextSiblingElement();
	data.ReadyRandomTime = node->FloatText();

	node = node->NextSiblingElement();
	data.StartVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.EndVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MinHorizontalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxHorizontalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MinVerticalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxVerticalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.Gravity.x = node->FloatAttribute("X");
	data.Gravity.y = node->FloatAttribute("Y");
	data.Gravity.z = node->FloatAttribute("Z");

	node = node->NextSiblingElement();
	data.ColorAmount = node->FloatText();

	node = node->NextSiblingElement();
	data.MinColor.r = node->FloatAttribute("R");
	data.MinColor.b = node->FloatAttribute("G");
	data.MinColor.g = node->FloatAttribute("B");
	data.MinColor.a = node->FloatAttribute("A");

	node = node->NextSiblingElement();
	data.MaxColor.r = node->FloatAttribute("R");
	data.MaxColor.b = node->FloatAttribute("G");
	data.MaxColor.g = node->FloatAttribute("B");
	data.MaxColor.a = node->FloatAttribute("A");

	node = node->NextSiblingElement();
	data.MinRotateSpeed = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxRotateSpeed = node->FloatText();

	node = node->NextSiblingElement();
	data.MinStartSize = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxStartSize = node->FloatText();

	node = node->NextSiblingElement();
	data.MinEndSize = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxEndSize = node->FloatText();

	SafeDelete(document);
}
