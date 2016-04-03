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

using namespace std;
using namespace cv;

void detect_rapid_finger_move(Mat frame, vector<Point> contour, Size size, ScreenSize* screen, KalmanFilter* KF, Mat measurement);
void detect_finger_move(Mat frame, vector<Point> contour, Size size, ScreenSize* screen, KalmanFilter* KF, Mat measurement);
Mat detect_biggest_blob(Mat draw, Hand* hand);
Point find_higher_point(vector<Point> contour);
Mat smooth_frame(Mat frame);
Mat extract_background(Mat frame, Ptr<BackgroundSubtractor> pMOG2);
