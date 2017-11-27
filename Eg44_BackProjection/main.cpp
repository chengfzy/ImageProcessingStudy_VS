#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

Mat src, hsv, hue;
int bins{ 25 };

void histAndBackproj(int pos, void* userdata) {
	// calculate hue histogram
	Mat hist;
	calcHist(vector<Mat>{ hue }, vector<int>{ 0 }, noArray(), hist, vector<int>{ MAX(bins, 2) }, vector<float>{ 0, 180 }, false);
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

	// calculate back projection
	MatND backproj;
	calcBackProject(vector<Mat>{ hue }, vector<int>{ 0 }, hist, backproj, vector<float>{ 0, 180 }, 1);
	imshow("Back Projection", backproj);
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

	// use only the hue channels
	hue.create(hsv.size(), hsv.depth());
	mixChannels(vector<Mat>{ hsv }, vector<Mat>{ hue }, vector<int>{ 0, 0 });

	// create trackerbar to enter the number of bins
	namedWindow("Source Image", WINDOW_AUTOSIZE);
	createTrackbar("Hue Bins", "Source Image", &bins, 180, histAndBackproj);
	imshow("Source Image", src);
		
	waitKey();
	return 0;
}