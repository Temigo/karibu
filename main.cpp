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



Mat detect_biggest_blob(Mat draw, Hand* hand) {
    vector< vector<Point> > outputs;
    vector<Vec4i> hierarchy;
    Mat dst = Mat::zeros(draw.size(), CV_8UC3);

    findContours(draw, outputs, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE );
    int largestComp = -1;
    double maxArea = 100;

    for(unsigned int idx = 0 ; idx < outputs.size() ; idx++) {
        double area = fabs(contourArea(outputs[idx], false));
        if( area > maxArea ) {
            maxArea = area;
            largestComp = idx;
        }
    }

    if (largestComp == -1) return dst;

    Scalar color( 0, 0, 255 );

    if (outputs[largestComp].size() >= 5) {
        vector<Point> contour = outputs[largestComp];
        RotatedRect box = fitEllipse(contour);

        if (box.size.width > 10) {
            vector<Point> hull;
            vector<int> hullI;
            convexHull(contour, hull);
            convexHull(contour, hullI);

            vector< vector<Point> > a;
            a.push_back(hull);
            //ellipse(dst, box, Scalar(0,255,0), 1, LINE_AA);
            drawContours(dst, outputs, largestComp, color, FILLED, LINE_8, hierarchy);
            //drawContours(dst, outputs, largestComp, color);
            drawContours(dst, a, 0, color);

            // Find and draw convexity defects
            if (hull.size() > 3 && contour.size() > 3) {
                vector<Vec4i> defects;
                convexityDefects(contour, hullI, defects);

                for (unsigned int i = 0; i < defects.size() ; i++) {
                    const Vec4i& v = defects[i];
                    float depth = v[3] / 256;
                    if (depth > 20) {
                        circle(dst, contour[v[2]], 10, Scalar(0, 255, 0));
                    }
                }
            }

            // Approx Poly
            /*vector<Point> approx;
            approxPolyDP(contour, approx, 30, true);
            a.push_back(approx);
            drawContours(dst, a, 1, Scalar(0,255,0));*/

            // Save in hand
            hand->contour = contour;
            hand->contourLength = arcLength(contour, true);
            hand->setSum();
        }
    }
    return dst;
}

void detect_finger_move(vector<Point> contour, Size size, ScreenSize* screen) {
    Rect box = boundingRect(contour);

    // Move mouse according to hand move
    Point max_point;
    max_point.y = box.tl().y;
    max_point.x = box.tl().x + box.width/2;
    do_mousemove((float) max_point.x / (float) size.width,
                (float) max_point.y/(float) size.height,
                screen);
    //cout << max_point.x << endl;
}

Mat smooth_frame(Mat frame) {
    Mat blurred_frame;
    //blur(frame, frame, Size(25, 25), Point(-1, -1));
    //GaussianBlur(frame, frame, Size(5, 5), 0, 0);
    //medianBlur(frame, frame, 5);
    bilateralFilter(frame, blurred_frame, 5, 5, 5);
    erode(blurred_frame, blurred_frame, NULL);
    dilate(blurred_frame, blurred_frame, NULL);
    return blurred_frame;
}

Mat extract_background(Mat frame, Ptr<BackgroundSubtractor> pMOG2) {
    //Mat blurred_frame;
    //GaussianBlur(frame, blurred_frame, Size(5, 5), 0, 0);
    //blur(frame, blurred_frame, Size(5, 5), Point(-1, -1));

    Mat fgMaskMOG2;
    pMOG2->apply(frame, fgMaskMOG2);
    return fgMaskMOG2;
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
        detect_finger_move(hand.contour, frame.size(), &screen);

        /*vector<Rect> objects;
        CascadeClassifier c = CascadeClassifier("include/palm.xml");
        c.detectMultiScale(frame, objects);

        for (int i = 0; i < objects.size(); i++) {
            rectangle(frame, objects[i], Scalar(0));
        }*/

        //imshow("Original", frame);
        //imshow("Blur", blurred_frame);
        //moveWindow("Blur", 700, 0);
        imshow("Karibu", dst2);

        waitKey(30);

    }

    waitKey(0);
    return 0;
}
