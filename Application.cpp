#include "Core/Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// ImGui包含
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// 静态回调函数
static void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Application::Application(const std::string& title, int width, int height)
    : m_Title(title), m_Width(width), m_Height(height), m_Window(nullptr), m_LastFrameTime(0.0f)
{
}

Application::~Application()
{
}

void Application::Run()
{
    // 1. 初始化GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // 2. 配置GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 3. 创建窗口
    GLFWwindow* window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    m_Window = window;  // 保存窗口指针
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 开启垂直同步

    // 4. 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return;
    }

    // 5. 设置视口和回调
    glViewport(0, 0, m_Width, m_Height);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // 6. 调用子类的初始化
    Initialize();

    // 7. 初始化ImGui
    if (!InitializeImGui())
    {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        return;
    }

    // 8. 主循环
    m_LastFrameTime = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        // 计算deltaTime
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - m_LastFrameTime;
        m_LastFrameTime = currentTime;

        // 开始ImGui帧
        BeginImGuiFrame();

        // 更新逻辑
        Update(deltaTime);

        // 渲染游戏内容
        Render();

        // 渲染ImGui
        OnImGuiRender();

        // 结束ImGui帧
        EndImGuiFrame();

        // 交换缓冲区和事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 9. 清理
    Shutdown();
    ShutdownImGui();

    glfwDestroyWindow(window);
    glfwTerminate();
}

// ============ ImGui相关方法实现 ============

bool Application::InitializeImGui()
{
    GLFWwindow* window = static_cast<GLFWwindow*>(m_Window);

    // 初始化ImGui上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // 设置ImGui样式
    ImGui::StyleColorsDark();

    // 初始化平台/渲染器后端
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
        return false;

    const char* glsl_version = "#version 330";
    if (!ImGui_ImplOpenGL3_Init(glsl_version))
        return false;

    return true;
}

void Application::ShutdownImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::BeginImGuiFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Application::EndImGuiFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}