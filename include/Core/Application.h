#pragma once
#include <string>

class Application
{
public:
    Application(const std::string& title, int width, int height);
    virtual ~Application();

    void Run();  // 启动应用的主循环

protected:
    // 子类需要重写的生命周期方法
    virtual void Initialize() {}     // 初始化资源
    virtual void Update(float deltaTime) {}  // 每帧更新
    virtual void Render() {}         // 每帧渲染
    virtual void OnImGuiRender() {}  // 新增：ImGui渲染
    virtual void Shutdown() {}       // 清理资源

private:
    std::string m_Title;
    int m_Width;
    int m_Height;
    void* m_Window;  // GLFWwindow*
    float m_LastFrameTime;

    // 新增：ImGui相关私有方法
    bool InitializeImGui();
    void ShutdownImGui();
    void BeginImGuiFrame();
    void EndImGuiFrame();
};