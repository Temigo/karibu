#include <stdlib.h>
// Using Linux
#include <X11/Xlib.h>
#include <X11/Xutil.h>

struct ScreenSize {
    /* For Windows :
    #include <windows.h>
    Int x = GetSystemMetrics(SM_CXSCREEN);
    Int y = GetSystemMetrics(SM_CYSCREEN);
    */
    // For Unix
    int width;
    int height;
    ScreenSize() {
        Display* disp = XOpenDisplay(NULL);
        Screen*  screen = DefaultScreenOfDisplay(disp);
        width = screen->width;
        height = screen->height;
    }
};

void do_alt_tab();
void do_alt_tab_press(bool setOn);
void do_mousemove(float x, float y, ScreenSize* screen);
void do_rapid_mousemove(float x, float y, float vx, float vy, ScreenSize* screen);
