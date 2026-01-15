#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>

Mesh::Mesh()
{
}
std::vector<float> Mesh::GetVerticesFloat() {
    std::vector<float> result;
    for (size_t i = 0; i < _verticeArray.size(); i++)
    {
        result.push_back(_verticeArray[i].x);
        result.push_back(_verticeArray[i].y);
        result.push_back(_verticeArray[i].z);
    }
    return result;
}

void Mesh::LoadMeshFromPath(const std::string& filepath)
{
    _verticeArray.clear();
    // 打开文件
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "错误：无法打开文件: " << filepath << std::endl;
        return;
    }

    // 读取文件内容到stringstream
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    // 解析OBJ
    ParseObjFile(buffer);

    std::cout << "加载完成: " << _verticeArray.size() << " 个顶点" << std::endl;
}

void Mesh::ParseObjFile(std::stringstream& ss)
{
    std::vector<Vector3> tempPositions;  // 临时存储顶点位置
    std::string line;

    while (std::getline(ss, line)) {
        // 跳过空行和注释
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream lineStream(line);
        std::string prefix;
        lineStream >> prefix;

        if (prefix == "v") {  // 顶点位置
            float x, y, z;
            if (lineStream >> x >> y >> z) {
                tempPositions.push_back(Vector3(x, y, z));
            }
        }
        else if (prefix == "f") {  // 面数据
            // 处理三角形或四边形面
            std::vector<std::string> faceTokens;
            std::string token;

            while (lineStream >> token) {
                faceTokens.push_back(token);
            }

            // 四边形面：三角化为两个三角形
            if (faceTokens.size() == 4) {
                // 三角形1: v0, v1, v2
                ParseFace(faceTokens[0], tempPositions);
                ParseFace(faceTokens[1], tempPositions);
                ParseFace(faceTokens[2], tempPositions);

                // 三角形2: v0, v2, v3
                ParseFace(faceTokens[0], tempPositions);
                ParseFace(faceTokens[2], tempPositions);
                ParseFace(faceTokens[3], tempPositions);
            }
        }
        // 忽略其他数据(vt, vn, usemtl等)
    }
}

void Mesh::ParseFace(const std::string& token,
    const std::vector<Vector3>& tempPositions)
{
    // OBJ面格式: "顶点索引/纹理索引/法线索引" 或 "顶点索引//法线索引"
    // 我们只关心顶点索引（第一个数字）

    // 找到第一个斜杠
    size_t slashPos = token.find('/');
    std::string vertexPart = token;

    if (slashPos != std::string::npos) {
        vertexPart = token.substr(0, slashPos);
    }

    try {
        // OBJ索引从1开始，C++从0开始
        int idx = std::stoi(vertexPart) - 1;

        if (idx >= 0 && idx < static_cast<int>(tempPositions.size())) {
            _verticeArray.push_back(tempPositions[idx]);
        }
        else {
            std::cerr << "警告：顶点索引超出范围: " << idx << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "错误：解析面数据失败: " << token << std::endl;
    }
}