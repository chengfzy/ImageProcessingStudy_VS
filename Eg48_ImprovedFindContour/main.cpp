#include <iostream>
#include <algorithm>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

struct AreaCmp {
	AreaCmp(const vector<float>& areas) :areas_(&areas) {}

	bool operator()(int a, int b) const {
		return (*areas_)[a] > (*areas_)[b];
	}

	const vector<float>* areas_;
};

int main() {	
	// read source image and template
	Mat src = imread("../../data/ubuntu.png", IMREAD_UNCHANGED);
	if (src.empty()) {
		cout << "couldn't read source image." << endl;
		return -1;
	}
	Mat gray;
	cvtColor(src, gray, COLOR_BGR2GRAY);

	Mat edge;
	threshold(gray, edge, 200, 255, THRESH_BINARY);
	imshow("Image after threshold", edge);
	
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(gray, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
	cout << "Total Contours Detected: " << contours.size() << endl;

	vector<int> sortIdx(contours.size());
	vector<float> areas(contours.size());
	for (int n = 0; n < contours.size(); ++n) {
		sortIdx[n] = n;
		areas[n] = contourArea(contours[n], false);
	}

	// sort contours so that the largest contours go first
	sort(sortIdx.begin(), sortIdx.end(), AreaCmp(areas));

	// draw contours
	RNG rng;
	for (int n = 0; n < sortIdx.size(); ++n) {
		int idx = sortIdx[n];
		Scalar color(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(src, contours, idx, color, 1, LINE_AA, hierarchy, 0);
		cout << "Contour #" << idx << ": area = " << areas[idx] << ", nvertics = " << contours[idx].size() << endl;
		imshow("Contours", src);

		if (waitKey() == 27)
			break;
	}
	
	waitKey();
	return 0;
}