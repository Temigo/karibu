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

Mat detect_biggest_blob(Mat draw, Hand* hand) {
    vector< vector<Point> > outputs;
    vector<Vec4i> hierarchy;
    Mat dst = Mat::zeros(draw.size(), CV_8UC3);

    findContours(draw, outputs, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE );
    int largestComp = -1;
    double maxArea = 200;

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
            if (ENABLE_ALT_TAB) hand->setSum();
        }
    }
    return dst;
}

Point find_higher_point(vector<Point> contour) {
    Point max_point;
    Rect box = boundingRect(contour);
    for (unsigned int i = 0; i < contour.size(); i++) {
        Point p = contour[i];
        if (p.y >= box.tl().y) {
            max_point = p;
        }
    }
    return max_point;
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
