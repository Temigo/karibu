#include <stdlib.h>
#include <cmath> // For abs
// Using Linux
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "actions.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;


void do_alt_tab() {
    system("xdotool key alt+Tab");
}

void do_alt_tab_press(bool setOn) {
    // sleep 2; xdotool keyup alt
    if (setOn) {
        system("xdotool keydown alt+Tab");
    }
    else {
        system("xdotool keyup alt");
    }
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

bool GOING_RIGHT = false;
bool RETURN_PEAK = false;
int SPEED_THRESHOLD = 10;
// TODO : works only towards the right
void do_rapid_mousemove(float x, float y, float vx, float vy, ScreenSize* screen) {
    if (abs(vx) > SPEED_THRESHOLD) {
        RETURN_PEAK = ((GOING_RIGHT && (vx > 0)) || (!GOING_RIGHT && (vx < 0))) && !RETURN_PEAK;
    }
    GOING_RIGHT = (vx < 0);

    if (vx < -SPEED_THRESHOLD) {
        //cout << "[Right] ";
        if (RETURN_PEAK) {
            cout << "Peak right ";
            system("xdotool key Right");
        }
    }

    if (vx > SPEED_THRESHOLD) {
        //cout << "[Left] ";
        if (RETURN_PEAK) {
            cout << "Peak left ";
            system("xdotool key Left");
        }
    }

    if (abs(vx) < SPEED_THRESHOLD) { // Re-init
        RETURN_PEAK = false;
        GOING_RIGHT = false;
    }

    if (RETURN_PEAK && (abs(vx) > SPEED_THRESHOLD)) {
        cout << GOING_RIGHT << " " << RETURN_PEAK << " " << vx << " " << vy << endl;
    }
}
