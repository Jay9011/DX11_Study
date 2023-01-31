#include "Framework.h"
#include "MeshQuad.h"

MeshQuad::MeshQuad()
{
}

MeshQuad::~MeshQuad() = default;

void MeshQuad::Create()
{
    float w = 0.5f;
    float h = 0.5f;

    vector<MeshVertex> v;
    v.push_back(MeshVertex(-w, -h, .0f, 0, 1, .0f, .0f, -1.0f));
    v.push_back(MeshVertex(-w, +h, .0f, 0, 0, .0f, .0f, -1.0f));
    v.push_back(MeshVertex(+w, -h, .0f, 1, 1, .0f, .0f, -1.0f));
    v.push_back(MeshVertex(+w, +h, .0f, 1, 0, .0f, .0f, -1.0f));

    vertices = new MeshVertex[v.size()];
    vertexCount = v.size();

    copy(v.begin(), v.end(), stdext::checked_array_iterator<MeshVertex*>(vertices, vertexCount));

    indexCount = 6;
    indices = new UINT[indexCount]{ 0, 1, 2, 2, 1, 3 };
}
