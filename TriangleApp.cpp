#include "Core/TriangleApp.h"
#include <glad/glad.h>
#include <iostream>
#include <cmath>
#include "imgui.h"
#include "Vector3.h"

#define PI 3.1415926535897

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    
    out float vZCoord;

    void main()
    {
        gl_Position = vec4(aPos * 0.2, 1.0);
        vZCoord = gl_Position.z;
    }
    )";


const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    uniform vec3 uObjectColor;
    uniform vec3 uBackgroundColor;
    in float vZCoord;

    void main()
    {
        //float normalizedZ = (vZCoord+1) * 0.5;
        float normalizedZ = (vZCoord+1) * 1;
        vec3 finalColor = mix(uObjectColor, uBackgroundColor, normalizedZ);
        FragColor = vec4(finalColor, 1.0);
    }
    )";

TriangleApp::TriangleApp()
    : Application("Triangle Engine", 800, 800),
    m_VAO(0), m_VBO(0), m_ShaderProgram(0)
{
    // 初始化ImGui控制变量
    m_ClearColor[0] = 0.2f;  // R
    m_ClearColor[1] = 0.3f;  // G
    m_ClearColor[2] = 0.3f;  // B
    m_ClearColor[3] = 1.0f;  // A

    // 初始化三角形顶点颜色（三个顶点，每个顶点RGB三个值）
    // 顶点1: 红色
    m_TriangleColors[0] = 1.0f; m_TriangleColors[1] = 0.0f; m_TriangleColors[2] = 0.0f;
    // 顶点2: 绿色
    m_TriangleColors[3] = 0.0f; m_TriangleColors[4] = 1.0f; m_TriangleColors[5] = 0.0f;
    // 顶点3: 蓝色
    m_TriangleColors[6] = 0.0f; m_TriangleColors[7] = 0.0f; m_TriangleColors[8] = 1.0f;

    m_ShowDemoWindow = false;
    m_ShowControlWindow = true;
}

void TriangleApp::Initialize() {
    SetupShaders();
    SetupBuffers();
}

void TriangleApp::Render()
{
    // 1. 设置背景色
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 2. 激活着色器程序
    glUseProgram(m_ShaderProgram);

    // 设置物体颜色（比如红色）
    int objectColorLoc = glGetUniformLocation(m_ShaderProgram, "uObjectColor");
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);  // 红色，可修改为其他非黑色

    // 新增：设置深度渐变色 Uniform（补充缺失的赋值）
    int bgColorLoc = glGetUniformLocation(m_ShaderProgram, "uBackgroundColor");
    if (bgColorLoc != -1) {
        glUniform3f(bgColorLoc, 0.0f, 0.0f, 0.0f);
    }

    // 5. 绘制三角形
    glBindVertexArray(m_VAO);
    int vertexCount = renderVerticals.size() / 3;
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void TriangleApp::Shutdown() {
    std::cout << "Shutting down Triangle App..." << std::endl;

    // 清理资源
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_ShaderProgram);
}

void TriangleApp::OnImGuiRender()
{
    // 1. 控制窗口是否显示ImGui演示窗口
    if (m_ShowDemoWindow)
    {
        ImGui::ShowDemoWindow(&m_ShowDemoWindow);
    }

    // 2. 主控制窗口
    if (m_ShowControlWindow)
    {
        ImGui::Begin("Triangle Controls", &m_ShowControlWindow);

        // 2.1 背景颜色控制
        ImGui::ColorEdit4("Background Color", m_ClearColor);
        ImGui::Separator();

        // 2.2 三角形顶点颜色控制
        ImGui::Text("Vertex Colors:");
        ImGui::ColorEdit3("Vertex 1 (Red)", &m_TriangleColors[0]);
        ImGui::ColorEdit3("Vertex 2 (Green)", &m_TriangleColors[3]);
        ImGui::ColorEdit3("Vertex 3 (Blue)", &m_TriangleColors[6]);
        ImGui::Separator();

        // 2.3 窗口控制
        ImGui::Checkbox("Show Demo Window", &m_ShowDemoWindow);
        ImGui::Separator();

        // 2.4 性能信息
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);

        ImGui::End();
    }
}

void TriangleApp::SetupShaders() {
    // 1. 创建并编译着色器
    // 顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // 检查编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // 片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // 2. 链接着色器程序
    m_ShaderProgram = glCreateProgram();
    glAttachShader(m_ShaderProgram, vertexShader);
    glAttachShader(m_ShaderProgram, fragmentShader);
    glLinkProgram(m_ShaderProgram);
    glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_ShaderProgram, 512, NULL, infoLog);
        std::cout << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    // 3. 删除着色器对象（已链接到程序）
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void TriangleApp::SetMeshVerticals(std::vector<float> verticals) {
    allMeshVerticals = verticals;
}

void TriangleApp::SetupBuffers()
{
    // 5. 创建VAO和VBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    // 绑定VAO,绑定VBO
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    //分配buffer空间
    glBufferData(GL_ARRAY_BUFFER, allMeshVerticals.size() * sizeof(float), allMeshVerticals.data(), GL_DYNAMIC_DRAW);

    // 6. 设置顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

//更新VBO数据
void TriangleApp::Update(float deltaTime)
{
    // 绑定VAO,绑定VBO
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    
    //变换矩阵
    // 绕 x 轴 45° + 绕 y 轴 45° 合并后的旋转矩阵（无变量，直接复制使用）
    float rotationMatrix[16] = {
        0.70710678f,  0.5f,          0.5f,          0.0f,
        0.0f,         0.70710678f,  -0.70710678f,  0.0f,
        -0.70710678f, 0.5f,          0.5f,          0.0f,
        0.0f,         0.0f,          0.0f,          1.0f
    };
    Vector3 screenNor = Vector3(0, 0, 1);

    //模型坐标变换世界坐标
    std::vector<float> worldVertivals;
    for (size_t i = 0; i < allMeshVerticals.size(); i+=3)
    {
        Vector3 point = Vector3(allMeshVerticals[i], allMeshVerticals[i+1], allMeshVerticals[i+2]);
        Vector3 afterTrans = point.Transform(rotationMatrix);

        worldVertivals.push_back(afterTrans.x);
        worldVertivals.push_back(afterTrans.y);
        worldVertivals.push_back(afterTrans.z);
    }

    //背面剔除
    renderVerticals.clear();
    for(int i = 0;i< worldVertivals.size();i+=9)
    {
        //计算三个点的法向量,
        Vector3 pointa = Vector3(worldVertivals[i], worldVertivals[i + 1], worldVertivals[i + 2]);
        Vector3 pointb = Vector3(worldVertivals[i+3], worldVertivals[i + 4], worldVertivals[i + 5]);
        Vector3 pointc = Vector3(worldVertivals[i+6], worldVertivals[i + 7], worldVertivals[i + 8]);



        Vector3 faceNor = Vector3::CalculatePlaneNormal(pointa, pointb, pointc);
        float dotRst = faceNor * screenNor;
        if (dotRst < 0) {
            renderVerticals.push_back(worldVertivals[i]);
            renderVerticals.push_back(worldVertivals[i+1]);
            renderVerticals.push_back(worldVertivals[i+2]);
            renderVerticals.push_back(worldVertivals[i+3]);
            renderVerticals.push_back(worldVertivals[i+4]);
            renderVerticals.push_back(worldVertivals[i+5]);
            renderVerticals.push_back(worldVertivals[i+6]);
            renderVerticals.push_back(worldVertivals[i+7]);
            renderVerticals.push_back(worldVertivals[i+8]);
        }
    }

    //传递VBO数据
    glBufferData(GL_ARRAY_BUFFER, renderVerticals.size() * sizeof(float), renderVerticals.data(), GL_DYNAMIC_DRAW);
}