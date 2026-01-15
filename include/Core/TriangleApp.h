#pragma once
#include "../Core/Application.h"
#include <vector>

class TriangleApp : public Application
{
public:
    TriangleApp();
    void SetMeshVerticals(std::vector<float> verticals);

protected:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;
    void OnImGuiRender() override;  // 实现ImGui渲染
    void Shutdown() override;

private:
    void SetupShaders();
    void SetupBuffers();

private:
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_ShaderProgram;

    // 新增：ImGui控制变量
    float m_ClearColor[4];      // 背景颜色
    float m_TriangleColors[9];  // 三个顶点的RGB颜色
    bool m_ShowDemoWindow;      // 是否显示ImGui演示窗口
    bool m_ShowControlWindow;   // 是否显示控制窗口

    std::vector<float> allMeshVerticals;    //mesh顶点数据
    std::vector<float> renderVerticals;
};