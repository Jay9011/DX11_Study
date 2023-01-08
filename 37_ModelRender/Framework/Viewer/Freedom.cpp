#include "Framework.h"
#include "Freedom.h"

Freedom::Freedom() : Camera()
{}

Freedom::~Freedom() = default;

void Freedom::Update()
{
    if (Mouse::Get()->Press(1) == false) return;

    Vector3 f = Forward();
    Vector3 u = Up();
    Vector3 r = Right();

    // Move
    {
        Vector3 P;
        Position(&P);

        float moveSpeed = move * Time::Delta();

        if (Keyboard::Get()->Press('W'))
            P = P + f * moveSpeed;
        else if (Keyboard::Get()->Press('S'))
            P = P - f * moveSpeed;

        if (Keyboard::Get()->Press('D'))
            P = P + r * moveSpeed;
        else if (Keyboard::Get()->Press('A'))
            P = P - r * moveSpeed;

        if (Keyboard::Get()->Press('E'))
            P = P + u * moveSpeed;
        else if (Keyboard::Get()->Press('Q'))
            P = P - u * moveSpeed;

        Position(P);
    }

    // Rotation
    {
        Vector3 R;
        Rotation(&R);

        float rotateSpeed = rotation * Time::Delta();

        Vector3 val = Mouse::Get()->GetMoveValue();
        R.x = R.x + val.y * rotateSpeed;
        R.y = R.y + val.x * rotateSpeed;
        R.z = 0.0f;

        Rotation(R);
    }
}

void Freedom::Speed(float move, float rotation)
{
    this->move = move;
    this->rotation = rotation;
}
