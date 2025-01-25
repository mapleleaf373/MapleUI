#include "opengl_util/general.h"

#include <glad/gl.h>

namespace maple
{
namespace gl
{

std::shared_ptr<VertexBuffer> VertexBuffer::create()
{
    struct MakeSharedEnabler : public VertexBuffer {};
    return std::make_shared<MakeSharedEnabler>();
}

std::shared_ptr<VertexArray> VertexArray::create()
{
    struct MakeSharedEnabler : public VertexArray {};
    return std::make_shared<MakeSharedEnabler>();
}

std::shared_ptr<Shader> Shader::create(const std::string& vertex_, const std::string& fragment_)
{
    struct MakeSharedEnabler : public Shader {
        MakeSharedEnabler(const std::string& vertex_, const std::string& fragment_)
            : Shader(vertex_, fragment_) {}
    };
    return std::make_shared<MakeSharedEnabler>(vertex_, fragment_);
}

// ====================================================================================================================
// 
// ====================================================================================================================

VertexBuffer::VertexBuffer()
    : m_id{ 0 }
{
    glGenBuffers(1, &m_id);
    bind();
}

VertexBuffer::~VertexBuffer()
{
    unbind();
    glDeleteBuffers(1, &m_id);
}

void VertexBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

unsigned int VertexBuffer::get_id()
{
    return m_id;
}

VertexArray::VertexArray()
    : m_id{ 0 }
{
    glGenVertexArrays(1, &m_id);
    bind();
}

VertexArray::~VertexArray()
{
    unbind();
    glDeleteVertexArrays(1, &m_id);
}

void VertexArray::bind()
{
    glBindVertexArray(m_id);
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}

unsigned int VertexArray::get_id()
{
    return m_id;
}


// ====================================================================================================================
// ====================================================================================================================

Shader::Shader(const std::string& vertex_, const std::string& fragment_)
{
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertex_shader_source = vertex_.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);

    int success;
    char message[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, message);
        out(message);
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragment_shader_source = fragment_.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, message);
        out(message);
    }

    m_id = glCreateProgram();
    glAttachShader(m_id, vertex_shader);
    glAttachShader(m_id, fragment_shader);
    glLinkProgram(m_id);

    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_id, 512, nullptr, message);
        out(message);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    bind();
}

Shader::~Shader()
{
    unbind();
    glDeleteProgram(m_id);
}

void Shader::bind()
{
    glUseProgram(m_id);
}

void Shader::unbind()
{
    glUseProgram(0);
}

void Shader::set_uniform_vec4(const std::string& name_, float r_, float g_, float b_, float a_)
{
    glUniform4f(glGetUniformLocation(m_id, name_.c_str()), r_, g_, b_, a_);
}

unsigned int Shader::get_id()
{
    return m_id;
}

}
}