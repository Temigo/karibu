#include <stdio.h>
#include <iostream>
#include <list>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdexcept>
#include <vector>
#include "hand.h"
#include "actions.h"

// OpenCV 2.4.9
using namespace std;
using namespace cv;

bool ENABLE_ALT_TAB = false;

void Hand::reset() {
    history = 1;
    sum = contourLength;
    mean = sum;
}

void Hand::setSum() {
    // CUSUM ?
    sum = sum + contourLength;
    history++;
    //cout << contourLength << endl;
    //cout << history << " " << sum/history << " " << mean << endl;
    if (abs(sum/history - mean) > threshold && history > 15) {
        //cout << "Change " << history << endl;
        if (ENABLE_ALT_TAB) do_alt_tab();
        if (change) {
            cout << "Hand open" << endl;

        }
        else {
            cout << "Closed" << endl;
        }
        change = !change;
        reset();
        //cout << "   " << history << " " << sum/history << " " << mean << endl;
    }
    else {
        if (history < 50) {
            mean = sum / history;
        }
        else {
            reset();
        }
    }
}
