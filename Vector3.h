#pragma once

class Vector3
{
public:
    float x, y, z;

    Vector3();
    Vector3(float _x, float _y, float _z);
    float operator*(const Vector3& other) const;
    Vector3 Transform(const float modelMatrix[16]) const;

    // 1. 新增：叉乘运算符重载（返回新的 Vector3，对应叉乘结果）
    Vector3 Cross(const Vector3& other) const;

    // 2. 新增：归一化函数（将当前向量单位化，返回新的单位向量）
    Vector3 Normalize() const;

    // 3. 新增：静态函数（通过三个点计算平面法向量，直接返回单位法向量）
    static Vector3 CalculatePlaneNormal(const Vector3& P0, const Vector3& P1, const Vector3& P2);
};