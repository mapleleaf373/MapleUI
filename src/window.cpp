#include "window.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// ====================================================================================================================
// GLOBAL STATES
// ====================================================================================================================

namespace
{

    // for managing opened windows
    // used by mainloop to determine when to end
    // Window::create will add to open_windows
    std::vector<std::shared_ptr<maple::Window>> open_windows;
    bool is_mainloop_running = false;
    int window_just_closed_index = -1;

}

// ====================================================================================================================
// INITIALIZE GLFW LIBRARY
// ====================================================================================================================

namespace
{

    // ----------------------------------------------------------------------------------------------------------------

    bool is_glfw_library_initialized = false;

    struct GLFWLibraryInitializer
    {
        // ------------------------------------------------------------------------------------------------------------

        GLFWwindow* context_provider;

        // ------------------------------------------------------------------------------------------------------------

        static void error_callback(int error_code_, const char* description_)
        {
            std::cerr << "GLFW error callback: [ " << error_code_ << " ] "
                      << description_ << "\n";
        }

        // ------------------------------------------------------------------------------------------------------------

        GLFWLibraryInitializer()
            : context_provider{ nullptr }
        {
            glfwSetErrorCallback(error_callback);
        }

        ~GLFWLibraryInitializer()
        {
            glfwDestroyWindow(context_provider);
            glfwTerminate();
        }

        // ------------------------------------------------------------------------------------------------------------
        
        void initialize()
        {
            if (!glfwInit())
                throw std::runtime_error("void glfw_library_initializer_t::initialize(): "
                                         "Failed to initialize GLFW. glfwInit()");

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,
                           maple::configuration::opengl_version_major);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,
                           maple::configuration::opengl_version_major);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

            context_provider = glfwCreateWindow(100, 100,
                                                "", nullptr, nullptr);
            if (!context_provider)
                throw std::runtime_error("void glfw_library_initializer_t::initialize(): "
                                         "Failed to create window. glfwCreateWindow()");
            glfwMakeContextCurrent(context_provider);
            if (!gladLoadGL(glfwGetProcAddress))
            throw std::runtime_error("void glfw_library_initializer_t::initialize(): "
                                     "Failed to create opengl context. gladLoadGL()");

            out(glGetString(GL_RENDERER));

            is_glfw_library_initialized = true;
        }

    };

    GLFWLibraryInitializer glfw_library_initializer;

    // ----------------------------------------------------------------------------------------------------------------

    

    // ----------------------------------------------------------------------------------------------------------------

}

// ====================================================================================================================
// Window class IMPLEMENTATION DETAILS
// ====================================================================================================================

namespace maple
{
    
    struct Window::WindowCallbackFunctions
    {
        event_type::WindowCloseAttempt close_attempt = []() { return true; };;
        event_type::WindowClose close = []() {};
    };

    struct Window::InternalGLFWData
    {
        GLFWwindow* window;
    };

    std::shared_ptr<Window> Window::create(const WindowProperties& properties_)
    {
        struct MakeSharedEnabler : public Window {
            MakeSharedEnabler(const WindowProperties& properties_) : Window(properties_) {}
        };
        auto window = std::make_shared<MakeSharedEnabler>(properties_);
        if (is_mainloop_running)
            glfwShowWindow(window->m_handle->window);
        open_windows.push_back(window);
        return window;
    }

}

// ====================================================================================================================
// Window class MEMBER FUNCTION DEFINITIONS
// ====================================================================================================================

namespace maple
{

    // ----------------------------------------------------------------------------------------------------------------

    Window::Window(const WindowProperties& properties_)
        : m_prop{ properties_ },
          m_callback{ std::make_unique<WindowCallbackFunctions>() },
          m_handle{ std::make_unique<InternalGLFWData>() }
    {
        if (!is_glfw_library_initialized)
            glfw_library_initializer.initialize();

        libglfw_create_window();
        libglfw_set_callbacks();
    }

    Window::~Window()
    {
    }

    // ----------------------------------------------------------------------------------------------------------------

    void Window::set_title(const std::string& title_)
    {
        m_prop.title = title_;
        glfwSetWindowTitle(m_handle->window, title_.c_str());
    }

    std::string Window::get_title() const
    {
        return m_prop.title;
    }

    void Window::close()
    {
        glfwSetWindowShouldClose(m_handle->window, GLFW_TRUE);
    }

    // ----------------------------------------------------------------------------------------------------------------

    void Window::on_close_attempt(event_type::WindowCloseAttempt func_)
    {
        m_callback->close_attempt = func_;
    }

    void Window::on_close(event_type::WindowClose func_)
    {
        m_callback->close = func_;
    }

    // ----------------------------------------------------------------------------------------------------------------

    void Window::libglfw_create_window()
    {
        m_handle->window = glfwCreateWindow(m_prop.width, m_prop.height,
                                            m_prop.title.c_str(),
                                            nullptr, glfw_library_initializer.context_provider);
        if (!m_handle->window)
            throw std::runtime_error("Window::Window(const WindowProperties& properties_): "
                                     "Failed to create window. glfwCreateWindow()");

        glfwSetWindowUserPointer(m_handle->window, static_cast<void*>(m_callback.get()));

        glfwMakeContextCurrent(m_handle->window);
        glfwSwapInterval(1);
    }

    void Window::libglfw_set_callbacks()
    {
        glfwSetWindowCloseCallback(m_handle->window, [](GLFWwindow* handle_)
            {
                auto callback = static_cast<WindowCallbackFunctions*>(glfwGetWindowUserPointer(handle_));
                bool should_close = callback->close_attempt();
                glfwSetWindowShouldClose(handle_, should_close);
            });

        glfwSetFramebufferSizeCallback(m_handle->window, [](GLFWwindow* handle_, int width_, int height_)
            {
                glViewport(0, 0, width_, height_);
            });
    }

    void Window::libglfw_show()
    {
        glfwShowWindow(m_handle->window);
    }

    void Window::libglfw_close()
    {
        glfwDestroyWindow(m_handle->window);
    }

    bool Window::libglfw_should_close()
    {
        return glfwWindowShouldClose(m_handle->window);
    }

    void Window::libglfw_render()
    {
        glfwMakeContextCurrent(m_handle->window);

        glClearColor(1.0f, 1.0f, m_prop.width / 1000.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(m_handle->window);
    }

    // ----------------------------------------------------------------------------------------------------------------

}

// ====================================================================================================================
// mainloop FUNCTION DEFINITION
// ====================================================================================================================

namespace maple
{

    void mainloop()
    {
        is_mainloop_running = true;

        for (auto window : open_windows)
            window->libglfw_show();

        while (open_windows.size() > 0)
        {
            for (int i = 0; auto window : open_windows)
            {

                if (window->libglfw_should_close())
                {
                    window->libglfw_close();
                    window->m_callback->close();
                    window_just_closed_index = i;
                    break;
                }

                window->libglfw_render();
                i++;
            }

            if (window_just_closed_index != -1)
            {
                open_windows.erase(open_windows.begin() + window_just_closed_index);
                window_just_closed_index = -1;
            }

            glfwWaitEvents();
        }

        is_mainloop_running = false;
    }

}

// ====================================================================================================================
// ====================================================================================================================