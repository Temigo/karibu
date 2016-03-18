#include <stdio.h>
#include <iostream>
#include <list>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdexcept>
#include <vector>

// OpenCV 2.4.9
using namespace std;
using namespace cv;

extern bool ENABLE_ALT_TAB;

class Hand {
public:
    vector<Point> contour;
    double contourLength;
    double estimatedLength;
    double sum;
    double mean;
    double threshold;
    int history;
    bool change;

    Hand() {
        change = true;
        threshold = 100;
        history = 0;
        mean = 0;
        sum = 0;
    }
    void reset();
    void setSum();
};
