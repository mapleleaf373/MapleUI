#include <MapleUI/MapleUI.h>

int main()
{
    maple::WindowProperties prop{
        .width = 800, .height = 600,
        .title = "Test 1 Window"
    };

    bool ok = false;
    bool* okay = &ok;
    auto window = maple::Window::create(prop);
    window->on_close_attempt([=]() {
        out("closing....");
        return true;
        });

    maple::mainloop();

    return 0;
}