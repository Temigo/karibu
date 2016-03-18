#include <stdlib.h>
// Using Linux
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "actions.h"
#include <string>
#include <sstream>
#include <iostream>
//#include <cmath>

using namespace std;


void do_alt_tab() {
    system("xdotool key alt+Tab");
}

void do_alt_tab_press() {
    system("xdotool keydown alt+Tab ; sleep 2; xdotool keyup alt");
}

void do_mousemove(float x, float y, ScreenSize* screen) {
    int x_coord = int((1-x) * screen->width);
    int y_coord = int(y * screen->height);

    //cout << x_coord << " " << y_coord << endl;

    ostringstream oss;
    oss << "xdotool mousemove --sync " << x_coord << " " << y_coord;

    string s = oss.str();
    system(s.c_str());
}

void detect_finger_move(float x, float y, ScreenSize* screen) {
    int x_coord = int((1-x) * screen->width);
    int y_coord = int(y * screen->height);
      
}
