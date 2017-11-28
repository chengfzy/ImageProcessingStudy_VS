#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

Mat gray, binary;
int thresh{ 100 };
const string kImageWindow{ "Coutours" };

void onTrackbar(int pos, void* userdata) {
	threshold(gray, binary, thresh, 255, THRESH_BINARY);

	vector<vector<Point>> contours;
	findContours(binary, contours, noArray(), RETR_LIST, CHAIN_APPROX_SIMPLE);
	binary.setTo(Scalar(0));

	drawContours(binary, contours, -1, Scalar(255));
	imshow(kImageWindow, binary);
}

int main() {
	// read source image and template
	gray = imread("../../data/ubuntu.png", IMREAD_GRAYSCALE);
	if (gray.empty()) {
		cout << "couldn't read source image." << endl;
		return -1;
	}
	
	// create window and trackbar
	namedWindow(kImageWindow, WINDOW_AUTOSIZE);
	createTrackbar("Threshold", kImageWindow, &thresh, 255, onTrackbar);
	onTrackbar(0, nullptr);

	waitKey();
	return 0;
}