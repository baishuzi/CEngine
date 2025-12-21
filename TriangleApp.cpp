#include "Core/TriangleApp.h"
#include <glad/glad.h>
#include <iostream>
#include "imgui.h"

// 顶点着色器源码
//const char* vertexShaderSource = R"(
//#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
//out vec3 ourColor;
//void main() {
//    gl_Position = vec4(aPos, 1.0);
//    ourColor = aColor;
//}
//)";

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main()
    {
        gl_Position = vec4(aPos, 1.0);
    }
    )";

// 片段着色器源码
//const char* fragmentShaderSource = R"(
//#version 330 core
//out vec4 FragColor;
//in vec3 ourColor;
//void main() {
//    FragColor = vec4(ourColor, 1.0);
//}
//)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 uColor1;  // 顶点1颜色
    uniform vec3 uColor2;  // 顶点2颜色
    uniform vec3 uColor3;  // 顶点3颜色
    uniform float uInterpolation;  // 插值因子（可选）
    
    void main()
    {
        // 简单的三角形颜色插值（重心坐标插值）
        vec3 color = mix(mix(uColor1, uColor2, gl_FragCoord.x/800.0), 
                         uColor3, gl_FragCoord.y/600.0);
        FragColor = vec4(color, 1.0);
    }
    )";

TriangleApp::TriangleApp()
    : Application("Triangle Engine", 800, 600),
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
    std::cout << "Initializing Triangle App..." << std::endl;

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

    // 4. 设置顶点数据
    //float vertices[] = {
    //    // 位置              // 颜色
    //     0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // 顶点1：红色
    //    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // 顶点2：绿色
    //     0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f   // 顶点3：蓝色
    //};
    float vertices[] = {
        // 位置             
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };


    // 5. 创建VAO和VBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    // 绑定VAO
    glBindVertexArray(m_VAO);

    // 绑定VBO并复制数据
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 6. 设置顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 7. 解绑VBO和VAO（可选）
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::cout << "Triangle App initialized successfully!" << std::endl;
}

void TriangleApp::Render()
{
    // 使用ImGui控制的背景色
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    // 使用着色器程序
    glUseProgram(m_ShaderProgram);

    // 绘制三角形
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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

void TriangleApp::SetupBuffers()
{
    // 顶点位置数据（不变）
    float positions[] = {
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f   // top
    };

    // 创建VAO和VBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    // 只上传位置数据到VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 注意：颜色数据现在通过uniform传递，而不是顶点属性
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TriangleApp::Update(float deltaTime)
{
    // 更新着色器中的颜色uniform
    glUseProgram(m_ShaderProgram);

    // 设置顶点1颜色
    int location1 = glGetUniformLocation(m_ShaderProgram, "uColor1");
    glUniform3f(location1, m_TriangleColors[0], m_TriangleColors[1], m_TriangleColors[2]);

    // 设置顶点2颜色
    int location2 = glGetUniformLocation(m_ShaderProgram, "uColor2");
    glUniform3f(location2, m_TriangleColors[3], m_TriangleColors[4], m_TriangleColors[5]);

    // 设置顶点3颜色
    int location3 = glGetUniformLocation(m_ShaderProgram, "uColor3");
    glUniform3f(location3, m_TriangleColors[6], m_TriangleColors[7], m_TriangleColors[8]);
}