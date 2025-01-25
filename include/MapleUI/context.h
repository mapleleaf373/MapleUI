#pragma once
#include "define.h"



namespace maple
{



// ====================================================================================================================
//      CLASS: Context
// ====================================================================================================================

//
// 
//
class Window;
class Context
{
public:
    static std::shared_ptr<Context> create();

    bool mainloop();

private:
    struct InternalData;

    Context();
    virtual ~Context();

    std::shared_ptr<InternalData> m_internal;

    friend class Window;
};



// ====================================================================================================================
//      CLASS: Window
// ====================================================================================================================

struct WindowProperties
{
    Size size;
    Point position;
    std::string title;
};

class Window
{
public:
    static std::shared_ptr<Window> create(std::shared_ptr<Context>& context_,
                                          const WindowProperties& props_);
    static std::shared_ptr<Window> create(std::shared_ptr<Context>& context_,
                                          const Size& size_, const Point& position_,
                                          const std::string& title_);
    static std::shared_ptr<Window> create(std::shared_ptr<Context>& context_);

private:
    struct InternalData;

    Window(std::shared_ptr<Context>& context_, const WindowProperties& props_);
    virtual ~Window();

    void p_show();
    void p_draw();
    void p_close();
    bool p_is_close_approved();

    WindowProperties m_prop;
    std::shared_ptr<InternalData> m_internal;

    friend class Context;
};

// --------------------------------------------------------------------------------------------------------------------

}