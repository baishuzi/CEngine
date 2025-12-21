#pragma once
#include <string>

class Shader
{
public:
    Shader(const char* vertexSource, const char* fragmentSource);
    ~Shader();

    void Bind() const;
    void Unbind() const;
    unsigned int GetID() const { return m_ID; }

private:
    unsigned int m_ID;
    unsigned int CompileShader(unsigned int type, const char* source);

    // 关键修改：添加unsigned int type参数
    bool CheckShaderCompileStatus(unsigned int shader, unsigned int type);

    bool CheckProgramLinkStatus(unsigned int program);
};