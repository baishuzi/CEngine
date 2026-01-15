#include "Vector3.h"
#include "cmath"

Vector3::Vector3() : x(0), y(0), z(0)
{
}

Vector3::Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
{
}

float Vector3::operator*(const Vector3& other) const
{
    return this->x * other.x + this->y * other.y + this->z * other.z;
}

// 实现 4x4 矩阵变换函数（核心）
Vector3 Vector3::Transform(const float modelMatrix[16]) const
{
    // 步骤 1：将 3D Vector3 扩展为 4D 齐次坐标 (x, y, z, 1.0)
    const float w = 1.0f;

    // 步骤 2：列主序 4x4 矩阵 × 4D 列向量（核心计算，对应矩阵的 4 列）
    // 计算变换后的 x 分量（矩阵第 1 列 × 齐次向量）
    float newX = modelMatrix[0] * this->x + modelMatrix[4] * this->y + modelMatrix[8] * this->z + modelMatrix[12] * w;
    // 计算变换后的 y 分量（矩阵第 2 列 × 齐次向量）
    float newY = modelMatrix[1] * this->x + modelMatrix[5] * this->y + modelMatrix[9] * this->z + modelMatrix[13] * w;
    // 计算变换后的 z 分量（矩阵第 3 列 × 齐次向量）
    float newZ = modelMatrix[2] * this->x + modelMatrix[6] * this->y + modelMatrix[10] * this->z + modelMatrix[14] * w;

    // 步骤 3：返回变换后的 3D Vector3（若需透视除法，可添加：newX/w, newY/w, newZ/w，当前适配仿射变换）
    return Vector3(newX, newY, newZ);
}

// 1. 实现叉乘函数（核心：遵循叉乘数学公式）
Vector3 Vector3::Cross(const Vector3& other) const
{
    float nx = this->y * other.z - this->z * other.y;
    float ny = this->z * other.x - this->x * other.z;
    float nz = this->x * other.y - this->y * other.x;
    return Vector3(nx, ny, nz);
}

// 2. 实现归一化函数（计算模长，避免除零错误）
Vector3 Vector3::Normalize() const
{
    // 计算向量模长：|v| = sqrt(x² + y² + z²)
    float magnitude = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);

    // 避免模长为 0 时的除零错误
    if (magnitude < 1e-6f) {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    // 单位化：每个分量除以模长
    return Vector3(
        this->x / magnitude,
        this->y / magnitude,
        this->z / magnitude
    );
}

// 3. 实现静态函数：通过三个点计算平面单位法向量（核心业务函数）
Vector3 Vector3::CalculatePlaneNormal(const Vector3& P0, const Vector3& P1, const Vector3& P2)
{
    // 步骤 1：构造平面内的两个向量 u 和 v
    Vector3 u(P1.x - P0.x, P1.y - P0.y, P1.z - P0.z);
    Vector3 v(P2.x - P0.x, P2.y - P0.y, P2.z - P0.z);

    // 步骤 2：计算叉乘，得到未归一化的法向量
    Vector3 unnormalizedNormal = u.Cross(v);

    // 步骤 3：归一化，得到单位法向量（推荐）
    return unnormalizedNormal.Normalize();
}