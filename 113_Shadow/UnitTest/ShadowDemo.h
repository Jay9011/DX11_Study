#pragma once
#include "Systems/IExecute.h"

class ShadowDemo : public IExecute
{
public:
    virtual void Initialize() override;
    virtual void Ready() override {}
    virtual void Destroy() override;
    virtual void Update() override;
    virtual void PreRender() override;
    virtual void Render() override;
    virtual void PostRender() override;
    virtual void ResizeScreen() override {}

private:
	void Mesh();
	void Airplane();
	void Kachujin();
	void KachujinCollider();
	void KachujinWeapon();

	void PointLighting();
	void SpotLighting();

	void Pass(UINT mesh, UINT model, UINT anim);
	
private:
	Shader* shader;

	Shadow* shadow;
	Render2D* render2D;

	CubeSky* sky;

	Material* floor;
	Material* stone;
	Material* brick;
	Material* wall;

	MeshRender* cube;
	MeshRender* cylinder;
	MeshRender* sphere;
	MeshRender* grid;

	ModelRender* airplane = nullptr;

	ModelAnimator* kachujin = nullptr;
	Transform* colliderInitTransforms;
	ColliderObject** colliders;

	ModelRender* weapon = nullptr;
	Transform* weaponInitTransform;

	vector<MeshRender*> meshes;
	vector<ModelRender*> models;
	vector<ModelAnimator*> animators;
};