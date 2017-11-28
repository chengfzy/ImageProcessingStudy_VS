#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

bool useMask;
Mat src, templ, result;
const string kImageWindow{ "Source Image" };
const string kResultWindow{ "Result" };
int matchMethod{ 0 };
int maxTrackbar{ 5 };

void matching(int pos, void* userdata) {
	Mat imgDisplay;
	src.copyTo(imgDisplay);

	int resultCols = src.cols - templ.cols + 1;
	int resultRows = src.rows - templ.rows + 1;
	result.create(resultRows, resultCols, CV_32FC1);

	// template matching
	matchTemplate(src, templ, result, matchMethod);
	normalize(result, result, 0, 1, NORM_MINMAX);

	// find position and show
	double minVal, maxVal;
	Point minLoc, maxLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
	Point matchLoc;
	if (matchMethod == TM_SQDIFF || matchMethod == TM_SQDIFF_NORMED) {
		matchLoc = minLoc;
	} else
		matchLoc = maxLoc;	
	rectangle(imgDisplay, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar(0, 0, 255), 2, LINE_8);
	rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar(0), 2, LINE_8);
	imshow(kImageWindow, imgDisplay);
	imshow(kResultWindow, result);
}

int main() {
	// read source image and template
	src = imread("../../data/lena.png", IMREAD_UNCHANGED);
	if (src.empty()) {
		cout << "couldn't read source image." << endl;
		return -1;
	}
	templ = src(Rect(200, 200, 30, 30));

	// create window and trackbar
	namedWindow(kImageWindow, WINDOW_AUTOSIZE);
	namedWindow(kResultWindow, WINDOW_AUTOSIZE);
	const string kTrackbarLabel{ "Method:\n[0] SQDIFF\n[1] SQDIFF NORMED\n[2] CCORR\n[3] CCORR NORMED\n[4] CCOEFF\n[5] CCOEFF NORMED" };
	createTrackbar(kTrackbarLabel, kImageWindow, &matchMethod, 5, matching);
	cout << kTrackbarLabel << endl;
	matching(0, nullptr);

	waitKey();
	return 0;
}