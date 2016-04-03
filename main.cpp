#include <stdio.h>
#include <iostream>
#include <list>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdexcept>
#include <vector>
#include "actions.h"
#include "utils.h"

using namespace std;
using namespace cv;

// Enable/disable controls
bool ENABLE_SCROLL = false;
bool ENABLE_MOUSE = false;

void detect_rapid_finger_move(Mat frame, vector<Point> contour, Size size, ScreenSize* screen, KalmanFilter* KF, Mat measurement) {
    // Move mouse according to hand move
    Point max_point = find_higher_point(contour);

    // Kalman Filter
    Mat prediction = KF->predict();
    measurement.at<float>(2) = max_point.x - measurement.at<float>(0);
    measurement.at<float>(3) = max_point.y - measurement.at<float>(1);
    measurement.at<float>(0) = max_point.x;
    measurement.at<float>(1) = max_point.y;

    Mat estimated = KF->correct(measurement);
    //cout << "estimate " << estimated.at<float>(0) << " " << estimated.at<float>(1) << endl;
    //cout << "measure " << measurement.at<float>(0) << " " << measurement.at<float>(1) << endl;
    //cout << "measure " << measurement.at<float>(2) << " " << measurement.at<float>(3) << endl;
    //cout << "estimate " << estimated.at<float>(2) << " " << estimated.at<float>(3) << endl;
    do_rapid_mousemove((float) max_point.x / (float) size.width,
                (float) max_point.y/(float) size.height,
                estimated.at<float>(2), estimated.at<float>(3),
                screen);
    /*do_rapid_mousemove(max((float) 0., (float) estimated.at<float>(0) / (float) size.width),
                max((float) 0., (float) estimated.at<float>(1) / (float) size.height),
                estimated.at<float>(2), estimated.at<float>(3),
                screen);*/
    circle(frame, max_point, 10, Scalar(255, 0, 255));
    circle(frame, Point(measurement.at<float>(0), measurement.at<float>(1)), 10, Scalar(0, 255, 255));
    circle(frame, Point(estimated.at<float>(0), estimated.at<float>(1)), 10, Scalar(255, 255, 0));
    //do_mousemove(estimated.at<float>(0), estimated.at<float>(1), screen);
}

void detect_finger_move(Mat frame, vector<Point> contour, Size size, ScreenSize* screen, KalmanFilter* KF, Mat measurement) {
    // Move mouse according to hand move
    Point max_point = find_higher_point(contour);

    // Kalman Filter
    Mat prediction = KF->predict();
    measurement.at<float>(0) = max_point.x;
    measurement.at<float>(1) = max_point.y;
    Mat estimated = KF->correct(measurement);
    //cout << "estimate " << estimated.at<float>(0) << " " << estimated.at<float>(1) << endl;
    //cout << "measure " << measurement.at<float>(0) << " " << measurement.at<float>(1) << endl;
    /*do_mousemove((float) max_point.x / (float) size.width,
                (float) max_point.y/(float) size.height,
                screen);*/
    do_mousemove(max((float) 0., (float) estimated.at<float>(0) / (float) size.width),
                max((float) 0., (float) estimated.at<float>(1) / (float) size.height),
                screen);

    circle(frame, Point(estimated.at<float>(0), estimated.at<float>(1)), 10, Scalar(255, 255, 0));
}

int main( int argc, const char* argv[] ) {
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        throw std::runtime_error("Erreur dans l'ouverture du fichier.");
    }

    namedWindow("Karibu", CV_WINDOW_AUTOSIZE);

    // Setup
    Ptr<BackgroundSubtractor> pMOG2;
    pMOG2 = createBackgroundSubtractorMOG2(500, 16, false);
    Hand hand;
    ScreenSize screen;
/*
kalman->process_noise_cov is the 'process noise covariance matrix' and it is often referred in the Kalman literature as Q. The result will be smoother with lower values.

kalman->measurement_noise_cov is the 'measurement noise covariance matrix' and it is often referred in the Kalman literature as R. The result will be smoother with higher values.

The relation between those two matrices defines the amount and shape of filtering you are performing.

If the value of Q is high, it will mean that the signal you are measuring is varies quickly and you need the filter to be adaptable. If it is small, then big variations will be attributed to noise in the measure.

If the value of R is high (compared to Q), it will indicate that the measuring is noisy so it will be filtered more.
*/
    // Kalman filter : position and velocity
    KalmanFilter KF(4, 2, 0);
    //Mat state(4, 2, CV_32F);
    //Mat processNoise(4, 2, CV_32F);
    Mat_<float> measurement(2, 1);
    measurement.setTo(Scalar(0));
    KF.transitionMatrix = (Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);

    KF.statePre.at<float>(0) = 0; // First x
    KF.statePre.at<float>(1) = 0; // First y
    KF.statePre.at<float>(2) = 0;
    KF.statePre.at<float>(3) = 0;

    setIdentity(KF.measurementMatrix);
    setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
    setIdentity(KF.measurementNoiseCov, Scalar::all(1e-3));
    setIdentity(KF.errorCovPost, Scalar::all(1));

    // Kalman filter : position and velocity and acceleration
    KalmanFilter KF2(6, 4, 0);
    //Mat state(6, 4, CV_32F);
    //Mat processNoise(6, 4, CV_32F);
    Mat_<float> measurement2(4, 1);
    measurement2.setTo(Scalar(0));
    /* Motion equations
    x_k = x_{k-1} + dT * vx_{k-1} + 0.5 * dT * dT * ax_{k-1}
    vx_k = vx_{k-1} + dT * ax_{k-1}
    */
    float dT = 1e-1; // TODO Time step : use cv::getTickFrequency
    KF2.transitionMatrix = (Mat_<float>(6, 6) << 1,0,dT,0,0.5*dT*dT,0,   0,1,0,dT,0,0.5*dT*dT,  0,0,1,0,dT,0,  0,0,0,1,0,dT, 0,0,0,0,1,0, 0,0,0,0,0,1);

    KF2.statePre.at<float>(0) = 0; // First x
    KF2.statePre.at<float>(1) = 0; // First y
    KF2.statePre.at<float>(2) = 0;
    KF2.statePre.at<float>(3) = 0;
    KF2.statePre.at<float>(4) = 0;
    KF2.statePre.at<float>(5) = 0;

    setIdentity(KF2.measurementMatrix);
    setIdentity(KF2.processNoiseCov, Scalar::all(1e-1));
    setIdentity(KF2.measurementNoiseCov, Scalar::all(1e-1));
    setIdentity(KF2.errorCovPost, Scalar::all(1));

    if (ENABLE_SCROLL) do_alt_tab_press(true);
    for (;;) {
        Mat frame;
        cap >> frame;
        if (frame.empty())
            break;

        // Smoothing frame
        Mat blurred_frame = smooth_frame(frame);

        // Proceed on frame - hand contour is stored in hand->contour
        Mat dst = extract_background(blurred_frame, pMOG2);
        Mat dst2 = detect_biggest_blob(dst, &hand);

        if (ENABLE_MOUSE) detect_finger_move(dst2, hand.contour, frame.size(), &screen, &KF, measurement);
        if (ENABLE_SCROLL) detect_rapid_finger_move(dst2, hand.contour, frame.size(), &screen, &KF2, measurement2);

        //imshow("Original", frame);
        //imshow("Blur", blurred_frame);
        //moveWindow("Blur", 700, 0);
        imshow("Karibu", dst2);
        switch(waitKey(30)) {
            case 27: // Esc key to quit
                if (ENABLE_SCROLL) do_alt_tab_press(false);
                cout << "Exiting..." << endl;
                return 0;
                break;
            case 9: // tabulation key
                ENABLE_ALT_TAB = !ENABLE_ALT_TAB;
                cout << "   ENABLE_ALT_TAB = " << ENABLE_ALT_TAB << endl;
                break;
            case 32: // Space key
                ENABLE_SCROLL = !ENABLE_SCROLL;
                if (ENABLE_SCROLL) do_alt_tab_press(true);
                cout << "   ENABLE_SCROLL = " << ENABLE_SCROLL << endl;
                break;
            case 13: // Enter key
                ENABLE_MOUSE = !ENABLE_MOUSE;
                cout << "   ENABLE_MOUSE = " << ENABLE_MOUSE << endl;
        }
    }

    waitKey(0);
    return 0;
}
