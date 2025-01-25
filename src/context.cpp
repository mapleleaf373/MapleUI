#include "context.h"

#include "opengl_util/general.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>



namespace
{



// ====================================================================================================================
//      INTERNAL CLASS: LibGLFWInitializer
// ====================================================================================================================

//
// Initialize the GLFW library and load OpenGL functions using Glad.
// Once initialized, it will be valid until the end of program.
//
class LibGLFWInitializer
{
public:
    LibGLFWInitializer();
    ~LibGLFWInitializer();

    void initialize();

    bool is_initialized() const;

private:
    static void error_callback(int error_code_, const char* description_);

    bool m_is_initialized;
};

// --------------------------------------------------------------------------------------------------------------------

LibGLFWInitializer lib_glfw_initializer;

// --------------------------------------------------------------------------------------------------------------------

LibGLFWInitializer::LibGLFWInitializer()
    : m_is_initialized{ false }
{
}

LibGLFWInitializer::~LibGLFWInitializer()
{
    if (!is_initialized())
        return;

    glfwTerminate();
    m_is_initialized = false;
}

// --------------------------------------------------------------------------------------------------------------------

void LibGLFWInitializer::initialize()
{
    if (is_initialized())
        return;

    if (!glfwInit())
    throw std::runtime_error("void LibGLFWInitializer::initialize(): "
                             "Failed to initialize GLFW. glfwInit()");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,
                   maple::configuration::opengl_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,
                   maple::configuration::opengl_version_major);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* dummy = glfwCreateWindow(100, 100,
                                         "", nullptr, nullptr);
    if (!dummy)
        throw std::runtime_error("void LibGLFWInitializer::initialize(): "
                                 "Failed to create window. glfwCreateWindow()");
    glfwMakeContextCurrent(dummy);
    if (!gladLoadGL(glfwGetProcAddress))
        throw std::runtime_error("void LibGLFWInitializer::initialize(): "
                                 "Failed to create opengl context. gladLoadGL()");
    glfwDestroyWindow(dummy);

    m_is_initialized = true;
}

bool LibGLFWInitializer::is_initialized() const
{
    return m_is_initialized;
}

// --------------------------------------------------------------------------------------------------------------------

void LibGLFWInitializer::error_callback(int error_code_, const char* description_)
{
    std::cerr << "GLFW error callback: [ " << error_code_ << " ] "
              << description_ << "\n";
}

// --------------------------------------------------------------------------------------------------------------------



// ====================================================================================================================
//      shader source code used by InternalRenderer
// ====================================================================================================================

namespace shader
{

std::string quad_vertex = R"(
    #version 330 core
    layout (location = 0) in vec3 position;

    void main()
    {
        gl_Position = vec4(position.x/2, position.y/2, position.z/2, 1.0);
    }
)";

std::string quad_fragment = R"(
    #version 330 core
    layout (location = 0) out vec4 frag_color;

    uniform vec4 u_color;

    void main()
    {
        frag_color = u_color;
    }
)";

}



// ====================================================================================================================
//      INTERNAL CLASS: InternalRenderer
// ====================================================================================================================

//
// Only handles rendering.
// Will generate objects and set states for OpenGL context,
// but the context sharing part should be managed by Context and Window class
// by passing in the correct GLFWwindow* during generate_shared_objects
// and generate_window_states.
//
class InternalRenderer
{
public:
    struct SharedObjects;
    struct WindowStates;

    InternalRenderer();
    ~InternalRenderer();

    SharedObjects generate_shared_objects(GLFWwindow* shared_gl_context_);
    WindowStates generate_window_states(const SharedObjects& objs_, GLFWwindow* window_);

    void draw_quad(const SharedObjects& objs_, const WindowStates& states_);
};

//
// OpenGL objects used for context sharing.
// These objects will be created using a shared OpenGL context that is not bound to any user created Window,
// instead they will be using the invisible OpenGL context created by a Context class.
//
struct InternalRenderer::SharedObjects
{
    std::shared_ptr<maple::gl::VertexBuffer> quad_vb{ nullptr };
    std::shared_ptr<maple::gl::Shader> quad_shader{ nullptr };
};

//
// Store individual state of OpenGL context for each user created Window.
// While rendering, the Window will be using OpenGL objects created by their parent Context.
//
struct InternalRenderer::WindowStates
{
    std::shared_ptr<maple::gl::VertexArray> quad_va{ nullptr };
};

// --------------------------------------------------------------------------------------------------------------------

InternalRenderer internal_renderer;

// --------------------------------------------------------------------------------------------------------------------

InternalRenderer::InternalRenderer()
{

}

InternalRenderer::~InternalRenderer()
{

}

// --------------------------------------------------------------------------------------------------------------------

//
// Used by Context class to created shared OpenGL objects.
//
InternalRenderer::SharedObjects InternalRenderer::generate_shared_objects(GLFWwindow* shared_gl_context_)
{
    glfwMakeContextCurrent(shared_gl_context_);

    using namespace maple::gl;

    SharedObjects objs;
    objs.quad_vb = VertexBuffer::create();
    float vertices[] = {
            -1.0f,  1.0f,  0.0f,
             1.0f,  1.0f,  0.0f,
             1.0f, -1.0f,  0.0f,
             1.0f, -1.0f,  0.0f,
            -1.0f, -1.0f,  0.0f,
            -1.0f,  1.0f,  0.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    objs.quad_shader = Shader::create(shader::quad_vertex, shader::quad_fragment);

    return objs;
}

//
// Used by individual Window class to set OpenGL context state.
//
InternalRenderer::WindowStates InternalRenderer::generate_window_states(const SharedObjects& objs_, GLFWwindow* window_)
{
    glfwMakeContextCurrent(window_);

    using namespace maple::gl;

    WindowStates states;
    states.quad_va = VertexArray::create();
    objs_.quad_vb->bind();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);

    return states;
}

//
// Uses the SharedObjects from parent Context class and WindowStates from individual Window class.
//
void InternalRenderer::draw_quad(const SharedObjects& objs_, const WindowStates& states_)
{
    objs_.quad_shader->bind();
    states_.quad_va->bind();
    objs_.quad_shader->set_uniform_vec4("u_color", 0.3f, 0.4f, 0.5f, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// --------------------------------------------------------------------------------------------------------------------

}



namespace maple
{



// ====================================================================================================================
//     CLASS: Context
// ====================================================================================================================

struct Context::InternalData
{
    GLFWwindow* shared_gl_context{ nullptr };
    InternalRenderer::SharedObjects renderer_shared_objects;

    bool is_mainloop_running{ false };
    std::vector<std::shared_ptr<Window>> windows;
};

// --------------------------------------------------------------------------------------------------------------------

std::shared_ptr<Context> Context::create()
{
    struct MakeSharedEnabler : public Context {};
    return std::make_shared<MakeSharedEnabler>();
}

bool Context::mainloop()
{
    m_internal->is_mainloop_running = true;

    for (auto& window : m_internal->windows)
        window->p_show();

    while (m_internal->windows.size() > 0)
    {
        int index_of_window_just_closed = -1;
        for (int i = 0; auto& window : m_internal->windows)
        {
            if (window->p_is_close_approved())
            {
                window->p_close();
                index_of_window_just_closed = i;
                break;
            }

            window->p_draw();

            i++;
        }

        if (index_of_window_just_closed != -1)
            m_internal->windows.erase(m_internal->windows.begin() + index_of_window_just_closed);

        glfwWaitEvents();
    }

    return true;
}

// --------------------------------------------------------------------------------------------------------------------

Context::Context()
    : m_internal{ std::make_shared<InternalData>() }
{
    if (!lib_glfw_initializer.is_initialized())
        lib_glfw_initializer.initialize();

    // setup up shared opengl context

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,
                   maple::configuration::opengl_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,
                   maple::configuration::opengl_version_major);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    m_internal->shared_gl_context = glfwCreateWindow(100, 100,
                                                     "", nullptr, nullptr);
    if (!m_internal->shared_gl_context)
        throw std::runtime_error("void Context::Context(): "
                                 "Failed to create window. glfwCreateWindow()");

    glfwMakeContextCurrent(m_internal->shared_gl_context);
    out(glGetString(GL_RENDERER));

    m_internal->renderer_shared_objects = internal_renderer.generate_shared_objects(m_internal->shared_gl_context);
}

Context::~Context()
{
}

// --------------------------------------------------------------------------------------------------------------------



// ====================================================================================================================
//     CLASS: Window
// ====================================================================================================================

struct Window::InternalData
{
    std::shared_ptr<Context>& context;
    GLFWwindow* handle{ nullptr };
    InternalRenderer::WindowStates renderer_window_states;
};

// --------------------------------------------------------------------------------------------------------------------

std::shared_ptr<Window> Window::create(std::shared_ptr<Context>& context_,
                                       const WindowProperties& props_)
{
    struct MakeSharedEnabler : public Window
    {
        MakeSharedEnabler(std::shared_ptr<Context>& context_, const WindowProperties& props_)
            : Window(context_, props_) {}
    };
    auto window = std::make_shared<MakeSharedEnabler>(context_, props_);
    context_->m_internal->windows.push_back(window);
    return window;
}

std::shared_ptr<Window> Window::create(std::shared_ptr<Context>& context_,
                                       const Size& size_, const Point& position_,
                                       const std::string& title_)
{
    return create(context_,
                  WindowProperties{
                      .size     = size_,
                      .position = position_,
                      .title    = title_
                  });
}

std::shared_ptr<Window> Window::create(std::shared_ptr<Context>& context_)
{
    return create(context_,
        WindowProperties{
            .size       = Size{ .width = 800, .height = 600 },
            .position   = Point{ .x = 50, .y = 50 },
            .title      = "Maple UI"
        });
}

// --------------------------------------------------------------------------------------------------------------------

Window::Window(std::shared_ptr<Context>& context_, const WindowProperties& props_)
    : m_prop{ props_ },
      m_internal{ std::make_shared<InternalData>(InternalData{ .context = context_, .handle = nullptr }) }
{
    // create window

    m_internal->handle = glfwCreateWindow(m_prop.size.width, m_prop.size.height,
                                          m_prop.title.c_str(),
                                          nullptr,
                                          context_->m_internal->shared_gl_context);
    if (!m_internal->handle)
        throw std::runtime_error("void Window::Window(): "
                                 "Failed to create window. glfwCreateWindow()");

    // set callbacks

    glfwSetFramebufferSizeCallback(m_internal->handle, [](GLFWwindow* handle_, int width_, int height_)
        {
            glViewport(0, 0, width_, height_);
        });

    // set opengl context states for rendering

    m_internal->renderer_window_states
        = internal_renderer.generate_window_states(context_->m_internal->renderer_shared_objects,
                                                   m_internal->handle);
}

Window::~Window()
{
}

// --------------------------------------------------------------------------------------------------------------------

void Window::p_show()
{
    glfwShowWindow(m_internal->handle);
}

void Window::p_draw()
{
    glfwMakeContextCurrent(m_internal->handle);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    internal_renderer.draw_quad(m_internal->context->m_internal->renderer_shared_objects,
                                m_internal->renderer_window_states);

    glfwSwapBuffers(m_internal->handle);
}

void Window::p_close()
{
    glfwDestroyWindow(m_internal->handle);
}

bool Window::p_is_close_approved()
{
    return static_cast<bool>(glfwWindowShouldClose(m_internal->handle));
}

// --------------------------------------------------------------------------------------------------------------------

}