#include <MapleUI/context.h>

int main()
{
    /*
    maple::WindowProperties prop{
        .width = 800, .height = 600,
        .title = "Test 1 Window"
    };

    auto window = maple::Window::create(prop);

    maple::mainloop();
    */

    using namespace maple;

    auto root = Context::create();
    auto window = Window::create(root);
    root->mainloop();    

    return 0;
}