#include <stdlib.h>
// Using Linux
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "actions.h"

void do_alt_tab() {
    system("xdotool key alt+Tab");
}
