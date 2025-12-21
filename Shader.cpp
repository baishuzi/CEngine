#include "Graphics/Shader.h"
#include <glad/glad.h>
#include <iostream>
#include <vector>

Shader::Shader(const char* vertexSource, const char* fragmentSource)
    : m_ID(0)
{
    // 1. 编译顶点着色器
    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    if (vertexShader == 0) return;

    // 2. 编译片段着色器
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        return;
    }

    // 3. 创建着色器程序
    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertexShader);
    glAttachShader(m_ID, fragmentShader);
    glLinkProgram(m_ID);

    // 4. 检查链接状态
    if (!CheckProgramLinkStatus(m_ID))
    {
        glDeleteProgram(m_ID);
        m_ID = 0;
    }

    // 5. 删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    if (m_ID != 0)
    {
        glDeleteProgram(m_ID);
    }
}

void Shader::Bind() const
{
    if (m_ID != 0)
    {
        glUseProgram(m_ID);
    }
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

unsigned int Shader::CompileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // 检查编译状态
    if (!CheckShaderCompileStatus(shader, type))
    {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool Shader::CheckShaderCompileStatus(unsigned int shader, unsigned int type)
{
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        const int logSize = 512;
        char infoLog[logSize];
        glGetShaderInfoLog(shader, logSize, nullptr, infoLog);

        // 正确判断着色器类型
        const char* shaderTypeName = (type == GL_VERTEX_SHADER)
            ? "VERTEX" : "FRAGMENT";

        std::cerr << "ERROR::SHADER::" << shaderTypeName << "::COMPILATION_FAILED\n"
            << infoLog << std::endl;
        return false;
    }

    return true;
}

bool Shader::CheckProgramLinkStatus(unsigned int program)
{
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        const int logSize = 512;
        char infoLog[logSize];
        glGetProgramInfoLog(program, logSize, nullptr, infoLog);

        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
            << infoLog << std::endl;
        return false;
    }

    return true;
}