#include "Core/TriangleApp.h"
#include "Mesh.h"

int main()
{
    Mesh mesh;
    mesh.LoadMeshFromPath("D:/Blender/mesh/block.obj");

    TriangleApp app;
    app.SetMeshVerticals(mesh.GetVerticesFloat());
    app.Run();
    return 0;
}