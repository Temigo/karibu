#include <stdio.h>
#include <iostream>
#include <list>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdexcept>
#include <vector>
#include "hand.h"

// OpenCV 2.4.9
using namespace std;
using namespace cv;


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
    if (abs(sum/history - mean) > threshold && history > 10) {
        cout << "Change " << history << endl;
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
