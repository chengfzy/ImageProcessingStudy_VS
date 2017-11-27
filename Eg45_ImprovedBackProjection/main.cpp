#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

Mat src, hsv;
Mat mask;
int lowThresh{ 20 }, highThresh{ 20 };
const string kWindowTitle = "Source Image";

void histAndBackproj() {
	// calculate hue histogram
	Mat hist;
	vector<int> channels{ 0,1 };
	vector<float> ranges{ 0,179,0,255 };
	calcHist(vector<Mat>{ hsv }, channels, mask, hist, vector<int>{ 30, 32 }, ranges, false);
	normalize(hist, hist, 0, 255, NORM_MINMAX);

	// show histogram
	const int width{ 600 };
	const int height{ 400 };
	Mat histImg(height, width, CV_8UC3, Scalar(0));
	for (int i = 0; i < hist.rows; ++i) {
		rectangle(histImg, Point(i * width / hist.rows, height), Point((i + 1) * width / hist.rows, height - hist.at<float>(i) * height / 255.0),
			Scalar(100, 123, 255), FILLED);
	}
	imshow("Hue Histogram", histImg);

	// get back projection
	MatND backproj;
	calcBackProject(vector<Mat>{ hsv }, channels, hist, backproj, ranges, 1);
	imshow("Back Projection", backproj);
}

void pickPoint(int event, int x, int y, int, void* userdata) {
	if (event != EVENT_LBUTTONDOWN) {
		return;
	}

	// fill and get the mask
	Point seed(x, y);
	int newMaskVal{ 255 };
	Scalar newVal(120, 120, 120);
	int connectivity{ 8 };
	int flags = connectivity + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;

	Mat mask2(src.rows + 2, src.cols + 2, CV_8UC1, Scalar::all(0));
	floodFill(src, mask2, seed, newVal, nullptr, Scalar(lowThresh, lowThresh, lowThresh), Scalar(highThresh, highThresh, highThresh), flags);
	mask = mask2(Range(1, mask2.rows - 1), Range(1, mask2.cols - 1));

	imshow("Mask", mask);
	histAndBackproj();
}

int main() {
	// read image
	src = imread("../../data/HandIndoorColor.jpg", IMREAD_UNCHANGED);
	if (src.empty()) {
		cout << "couldn't read image." << endl;
		return 0;
	}

	// convert to HSV
	cvtColor(src, hsv, COLOR_BGR2HSV);

	// show the image
	namedWindow(kWindowTitle, WINDOW_AUTOSIZE);
	imshow(kWindowTitle, src);
	// create trackbars for floodfill thresholds
	createTrackbar("Low Thresh", kWindowTitle, &lowThresh, 255);
	createTrackbar("High Thresh", kWindowTitle, &lowThresh, 255);
	// set a mouse callback
	setMouseCallback(kWindowTitle, pickPoint);

	waitKey();
	return 0;
}