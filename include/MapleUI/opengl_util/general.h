#pragma once
#include "define.h"

namespace maple
{
namespace gl
{

// ====================================================================================================================
// 
// ====================================================================================================================

class VertexBuffer
{
private:
    VertexBuffer();
    virtual ~VertexBuffer();
public:
    static std::shared_ptr<VertexBuffer> create();

public:
    void bind();
    void unbind();
    unsigned int get_id();

private:
    unsigned int m_id;
};

class VertexArray
{
private:
    VertexArray();
    virtual ~VertexArray();
public:
    static std::shared_ptr<VertexArray> create();

public:
    void bind();
    void unbind();
    unsigned int get_id();

private:
    unsigned int m_id;
};


// ====================================================================================================================
// 
// ====================================================================================================================

class Shader
{
private:
    Shader(const std::string& vertex_, const std::string& fragment_);
    virtual ~Shader();
public:
    static std::shared_ptr<Shader> create(const std::string& vertex_, const std::string& fragment_);

public:
    void bind();
    void unbind();
    unsigned int get_id();

    void set_uniform_vec4(const std::string& name_, float r_, float g_, float b_, float a_);

private:
    unsigned int m_id;
};


}
}