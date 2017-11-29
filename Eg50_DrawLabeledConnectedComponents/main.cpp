#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {
	// read source image and template
	Mat src = imread("../../data/ubuntu.png", IMREAD_UNCHANGED);
	if (src.empty()) {
		cout << "couldn't read source image." << endl;
		return -1;
	}

	// convert to gray
	Mat gray;
	cvtColor(src, gray, COLOR_BGR2GRAY);

	// threshold
	Mat edge;
	threshold(gray, edge, 120, 255, THRESH_BINARY);
	//const int threshd{ 128 };
	//Canny(gray, edge, threshd, 2 * threshd, 3);
	imshow("Image after Threshold", edge);

	// connected component
	Mat labels, stats, centroids;
	int ccomps = connectedComponentsWithStats(edge, labels, stats, centroids, 8, CV_32S);
	cout << "Total Connected Components Detected: " << ccomps << endl;

	// set color for each connected components
	vector<Vec3b> colors(ccomps + 1);
	colors[0] = Vec3b(0, 0, 0);			// background pixels remain black
	for (int i = 1; i <= ccomps; ++i) {
		colors[i] = Vec3b(rand() % 256, rand() % 256, rand() % 256);
		if (stats.at<int>(i - 1, CC_STAT_AREA) < 10)
			colors[i] = Vec3b(0, 0, 0);			// small regions are painted with black too
	}

	// draw connected components
	Mat imageColor(src.size(), CV_8UC3, Scalar(0));
	for (int r = 0; r < imageColor.rows; ++r) {
		for (int c = 0; c < imageColor.cols; ++c) {
			int label = labels.at<int>(r, c);
			assert(0 <= label && label <= ccomps);
			imageColor.at<Vec3b>(r, c) = colors[label];
		}
	}
	imshow("Labled Map", imageColor);

	waitKey();
	return 0;
}