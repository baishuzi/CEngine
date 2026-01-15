#include <vector>
#include <string>
#include "Vector3.h"

class Mesh
{
public:
    Mesh();

    // 从文件加载网格
    void LoadMeshFromPath(const std::string& filepath);

    //// 获取顶点数据
    std::vector<Vector3> GetVertices(){ return _verticeArray; }
    std::vector<float> GetVerticesFloat();

private:
    // 解析OBJ文件
    void ParseObjFile(std::stringstream& ss);

    //解析面数据
    void ParseFace(const std::string& token,
        const std::vector<Vector3>& tempPositions);

private:
    std::vector<Vector3> _verticeArray;  // 顶点位置数组
};