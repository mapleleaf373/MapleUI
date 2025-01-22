#pragma once
#include "define.h"

namespace maple
{

    struct WindowProperties
    {
    public:
        int width, height;
        std::string title;
    };

    class Window
    {
    // Window Creation
    private:
        Window(const WindowProperties& properties_);
        ~Window();
    public:
        static std::shared_ptr<Window> create(const WindowProperties& properties_);

    // General getters & setters
    public:
        void set_title(const std::string& title_);
        std::string get_title() const;
        void close();

    // Set custom callback function ( will override default ones )
    public:
        void on_close_attempt(event_type::WindowCloseAttempt func_);
        void on_close(event_type::WindowClose func_);

    public:
        void add_frame();

    // Helper functions to interface with glfw
    private:
        void libglfw_create_window();
        void libglfw_set_callbacks();
        void libglfw_show();
        void libglfw_close();
        bool libglfw_should_close();
        void libglfw_render();

    private:
        WindowProperties m_prop;

        struct WindowCallbackFunctions;
        std::unique_ptr<WindowCallbackFunctions> m_callback;
        struct InternalGLFWData;
        std::unique_ptr<InternalGLFWData> m_handle;

    public:
        friend void mainloop();
    };

    void mainloop();

}
