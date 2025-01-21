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
    private:
        Window(const WindowProperties& properties_);
        ~Window();
    public:
        static std::shared_ptr<Window> create(const WindowProperties& properties_);

    public:
        friend void mainloop(const std::vector<std::shared_ptr<Window>>& windows_);

    public:
        void set_title(const std::string& title_);
        std::string get_title() const;

    private:
        void show();
        void close();
        bool should_close();
        void render();

    private:
        WindowProperties m_prop;
        struct InternalGLFWData;
        std::unique_ptr<InternalGLFWData> m_handle;
    };

    void mainloop(const std::vector<std::shared_ptr<Window>>& windows_);

}
