#include <MapleUI/MapleUI.h>

int main()
{
    maple::WindowProperties prop{
        .width = 800, .height = 600
    };

    prop.title = "Window 1";
    auto win1 = maple::Window::create(prop);
    prop.title = "Window 2";
    auto win2 = maple::Window::create(prop);

    maple::mainloop({ win1, win2 });

    return 0;
}