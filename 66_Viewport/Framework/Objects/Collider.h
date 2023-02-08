#pragma once

struct ColliderObject
{
    class Transform* Init = nullptr;
    class Transform* Transform = nullptr;
    class Collider* Collider = nullptr;
};

class Collider
{
public:
    Collider(Transform* transform, Transform* init = nullptr);
    ~Collider();

    void Update();
    void Render(Color color = Color(0, 1, 0, 1));

    Transform* GetTransform() { return transform; }

private:
    Transform* init = nullptr;
    Transform* transform = nullptr;

    Vector3 lines[8];
};