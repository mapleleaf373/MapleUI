#include "window.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace
{
    bool is_glfw_library_initialized = false;

    struct glfw_library_initializer_t
    {
        static void error_callback(int error_code_, const char* description_)
        {
            std::cerr << "GLFW error callback: [ " << error_code_ << " ] "
                      << description_ << "\n";
        }

        glfw_library_initializer_t()
        {
            glfwSetErrorCallback(error_callback);
        }

        ~glfw_library_initializer_t()
        {
            glfwTerminate();
        }
        
        void initialize()
        {
            if (!glfwInit())
                throw std::runtime_error("void glfw_library_initializer_t::initialize(): "
                                         "Failed to initialize GLFW. glfwInit()");
            is_glfw_library_initialized = true;
        }
    };

    glfw_library_initializer_t glfw_library_initializer;
}

namespace maple
{

    struct Window::InternalGLFWData
    {
        GLFWwindow* window;
    };

    std::shared_ptr<Window> Window::create(const WindowProperties& properties_)
    {
        struct MakeSharedEnabler : public Window {
            MakeSharedEnabler(const WindowProperties& properties_) : Window(properties_) {}
        };
        return std::make_shared<MakeSharedEnabler>(properties_);
    }

}

namespace maple
{

    Window::Window(const WindowProperties& properties_)
        : m_prop{ properties_ },
          m_handle{ std::make_unique<InternalGLFWData>() }
    {
        if (!is_glfw_library_initialized)
            glfw_library_initializer.initialize();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        m_handle->window = glfwCreateWindow(m_prop.width, m_prop.height,
                                            m_prop.title.c_str(),
                                            nullptr, nullptr); 
        if (!m_handle->window)
            throw std::runtime_error("Window::Window(const WindowProperties& properties_): "
                                     "Failed to create window. glfwCreateWindow()");

        glfwSetWindowUserPointer(m_handle->window, static_cast<void*>(this));

        glfwSetWindowCloseCallback(m_handle->window, [](GLFWwindow* handle_)
            {
                Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle_));
                out(window->get_title() << " window closing......");
                glfwSetWindowShouldClose(handle_, GLFW_TRUE);
            });

        glfwMakeContextCurrent(m_handle->window);
        if (!gladLoadGL(glfwGetProcAddress))
            throw std::runtime_error("Window::Window(const WindowProperties& properties_): "
                                     "Failed to create opengl context. gladLoadGL()");

        out(glGetString(GL_RENDERER));

        //glfwSwapInterval(1);
    }

    Window::~Window()
    {
    }

    void Window::set_title(const std::string& title_)
    {
    }

    std::string Window::get_title() const
    {
        return m_prop.title;
    }

    void Window::show()
    {
        glfwShowWindow(m_handle->window);
    }

    void Window::close()
    {
        glfwDestroyWindow(m_handle->window);
    }

    bool Window::should_close()
    {
        return glfwWindowShouldClose(m_handle->window);
    }

    void Window::render()
    {
        glfwMakeContextCurrent(m_handle->window);

        glClearColor(0.3f, 1.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(m_handle->window);
    }

    void mainloop(const std::vector<std::shared_ptr<Window>>& windows_)
    {
        enum class WindowState { Open, Close };
        std::vector<WindowState> window_states{ windows_.size(), WindowState::Open };
        size_t open_window_count{ windows_.size() };

        for (auto window : windows_)
            window->show();

        while (open_window_count > 0)
        {

            for (int i = 0; auto window : windows_)
            {
                auto& state = window_states[i++];

                if (state == WindowState::Close)
                    continue;

                if (window->should_close())
                {
                    state = WindowState::Close;
                    open_window_count--;
                    window->close();
                    continue;
                }

                window->render();
            }

            glfwPollEvents();

        }
    }

}
