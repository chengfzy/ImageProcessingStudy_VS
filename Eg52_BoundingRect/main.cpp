#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

const string kSourceWindow = "Source";
Mat gray;
int thresh{ 100 };
const int kMaxThresh{ 255 };
RNG rng;
int type{ 1 };		// bound rect type: 0 for approxPolyDP, boundingRect and minEnclosingCircle; 1 for minAreaRect and fitEllipse

void threshCallback(int, void*) {
	Mat threshOutput;
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;

	// detect edge using threshold
	threshold(gray, threshOutput, thresh, 255, THRESH_BINARY);
	// find contours
	findContours(threshOutput, contours, hierachy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);


	Mat drawing(threshOutput.size(), CV_8UC3, Scalar(0));
	if (0 == type) {
		// approximate contours to polygons, and then get bounding rects and circles
		vector<vector<Point>> contoursPoly(contours.size());
		vector<Rect> boundRect(contours.size());
		vector<Point2f> center(contours.size());
		vector<float> radius(contours.size());
		for (size_t i = 0; i < contours.size(); ++i) {
			approxPolyDP(contours[i], contoursPoly[i], 3, true);
			boundRect[i] = boundingRect(contoursPoly[i]);
			minEnclosingCircle(contoursPoly[i], center[i], radius[i]);
		}

		// draw polygonal contour, bounding rect and circles
		for (size_t i = 0; i < contours.size(); ++i) {
			Scalar color(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contoursPoly, i, color, 1, LINE_AA, hierachy, 0);		// try to modify the parameters
			rectangle(drawing, boundRect[i], color, 2, LINE_AA);
			circle(drawing, center[i], radius[i], color, 2, LINE_AA);
		}
	} else if (1 == type) {
		// find the rotated rectangles and ellipses for each contour
		vector<RotatedRect> minRect(contours.size());
		vector<RotatedRect> minEllipse(contours.size());
		for (size_t i = 0; i < contours.size(); ++i) {
			minRect[i] = minAreaRect(contours[i]);
			if (contours[i].size() > 5) {
				minEllipse[i] = fitEllipse(contours[i]);
			}
		}

		// draw polygonal contour, bounding rect and circles
		for (size_t i = 0; i < contours.size(); ++i) {
			Scalar color(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 1, LINE_AA, hierachy, 0);		// try to modify the parameters
			// draw rotated rectangle
			Point2f rectPoints[4];
			minRect[i].points(rectPoints);
			for (int j = 0; j < 4; ++j)
				line(drawing, rectPoints[j], rectPoints[(j + 1) % 4], color, 1, LINE_AA);
			// draw ellipse
			ellipse(drawing, minEllipse[i], color, 2, LINE_AA);
		}
	}

	// show in window
	imshow("Contours", drawing);

}

int main() {
	// read source image and template
	Mat src = imread("../../data/opencv.png", IMREAD_UNCHANGED);
	if (src.empty()) {
		cout << "couldn't read source image." << endl;
		return -1;
	}

	// convert to gray and blur
	cvtColor(src, gray, COLOR_BGR2GRAY);
	blur(gray, gray, Size(3, 3));

	// create trackbar
	imshow(kSourceWindow, src);
	createTrackbar("Threshold", kSourceWindow, &thresh, 255, threshCallback);
	threshCallback(0, nullptr);

	waitKey();
	return 0;
}