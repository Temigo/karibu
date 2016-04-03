/*
Bindings to graphic actions (X11) with xdotools
*/
#include <stdlib.h>

// Using Linux
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <string>
#include <sstream>
#include <iostream>
#include <ctime>
#include <cmath> // For abs

#include "actions.h"

using namespace std;

void do_click() {
    system("xdotool click 1");
}

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

int old_x_coord = -1;
int old_y_coord = -1;
float MAX_MOVEMENT = 1000. ;
void do_mousemove(float x, float y, ScreenSize* screen) {
    int x_coord = int((1-x) * screen->width);
    int y_coord = int(y * screen->height);

    if (old_x_coord > 0 && sqrt(pow(x_coord - old_x_coord, 2) +  pow(y_coord - old_y_coord, 2)) < MAX_MOVEMENT) {
        string cmd = "xdotool mousemove --sync " + to_string(x_coord) + " " + to_string(y_coord);
        system(cmd.c_str());
    }
    old_x_coord = x_coord;
    old_y_coord = y_coord;
}

bool GOING_RIGHT = false;
bool RETURN_PEAK = false;
int SPEED_THRESHOLD = 10;
int MAX_SECONDS = 3; // Max duration after which we click
clock_t inactive_since = -1;

// TODO : works only towards the right
void do_rapid_mousemove(float x, float y, float vx, float vy, ScreenSize* screen) {
    if (abs(vx) > SPEED_THRESHOLD) {
        inactive_since = -1;
        RETURN_PEAK = ((GOING_RIGHT && (vx > 0)) || (!GOING_RIGHT && (vx < 0))) && !RETURN_PEAK;
    }
    GOING_RIGHT = (vx < 0);

    if (vx < -SPEED_THRESHOLD) {
        //cout << "[Right] ";
        if (RETURN_PEAK) {
            //cout << "Peak right ";
            system("xdotool key Right");
        }
    }

    if (vx > SPEED_THRESHOLD) {
        //cout << "[Left] ";
        if (RETURN_PEAK) {
            //cout << "Peak left ";
            system("xdotool key Left");
        }
    }

    if (abs(vx) < SPEED_THRESHOLD) { // Re-init
        if (inactive_since == -1) {
            inactive_since = clock();
        }
        else {
            double elapsed_secs = double(clock() - inactive_since) / CLOCKS_PER_SEC;
            if (elapsed_secs > MAX_SECONDS) do_alt_tab_press(false);
        }
        RETURN_PEAK = false;
        GOING_RIGHT = false;
    }

    /*if (RETURN_PEAK && (abs(vx) > SPEED_THRESHOLD)) {
        cout << GOING_RIGHT << " " << RETURN_PEAK << " " << vx << " " << vy << endl;
    }*/
}
