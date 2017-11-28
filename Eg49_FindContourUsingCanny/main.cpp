#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

Mat gray;
int thresh{ 100 };
const string kImageWindow{ "Coutours" };
RNG rng;

void onTrackbar(int pos, void* userdata) {
	Mat canny;
	Canny(gray, canny, thresh, thresh * 2, 3);
	
	// find contours and hierachy
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	findContours(canny, contours, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	// draw contours
	Mat drawing(canny.size(), CV_8UC3, Scalar(0));
	for (size_t i = 0; i < contours.size(); ++i) {
		Scalar color(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 1, LINE_AA, hierachy, 0);
	}
	imshow(kImageWindow, drawing);
}

int main() {
	// read source image and template
	Mat src = imread("../../data/ubuntu.png", IMREAD_UNCHANGED);
	if (src.empty()) {
		cout << "couldn't read source image." << endl;
		return -1;
	}
	cvtColor(src, gray, COLOR_BGR2GRAY);
	blur(gray, gray, Size(3, 3));
	
	// create window and trackbar
	namedWindow(kImageWindow, WINDOW_AUTOSIZE);
	createTrackbar("Threshold", kImageWindow, &thresh, 255, onTrackbar);
	onTrackbar(0, nullptr);

	waitKey();
	return 0;
}