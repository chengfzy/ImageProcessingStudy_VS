#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

// Brute-Force Match
void testBF(const Mat& image1, const Mat& image2) {
	// convert to gray image
	Mat gray1, gray2;
	switch (image1.channels()) {
	case 1:
		gray1 = image1;
		gray2 = image2;
		break;
	case 3:
		cvtColor(image1, gray1, COLOR_BGR2GRAY);
		cvtColor(image2, gray2, COLOR_BGR2GRAY);
		break;
	case 4:
		cvtColor(image1, gray1, COLOR_BGRA2GRAY);
		cvtColor(image2, gray2, COLOR_BGRA2GRAY);
		break;
	default:
		break;
	}

	// key points
	Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create(100);
	vector<KeyPoint> keypoints1, keypoints2;
	detector->detect(gray1, keypoints1);
	detector->detect(gray2, keypoints2);

	// descriptors
	Ptr<SiftDescriptorExtractor> extractor = SiftDescriptorExtractor::create(100);
	Mat descriptors1, descriptors2;
	extractor->compute(gray1, keypoints1, descriptors1);
	extractor->compute(gray2, keypoints2, descriptors2);

	Mat dst;
	drawKeypoints(gray1, keypoints1, dst, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("SIFT KeyPoint", dst);

	// match
	Ptr<BFMatcher> bfMatcher = BFMatcher::create(NORM_L2, true);
	vector<DMatch> matchers;
	bfMatcher->match(descriptors1, descriptors2, matchers);

	// show
	Mat output;
	drawMatches(image1, keypoints1, image2, keypoints2, matchers, output, Scalar::all(-1), Scalar::all(-1), vector<char>(),
		DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("Brute Force Match", output);
	waitKey();
}


int main() {
	Mat image1 = imread("../../../../Data/stillA.jpg", IMREAD_UNCHANGED);
	Mat image2 = imread("../../../../Data/stillC.jpg", IMREAD_UNCHANGED);
	if (image1.empty() || image2.empty()) {
		cout << "cannot read the image..." << endl;
		return -1;
	}

	// resize the image if too large
	if (image1.rows > 1000 || image1.cols > 1000) {
		resize(image1, image1, Size(1000, image1.rows * 1000 / image1.cols));
		resize(image2, image2, Size(1000, image2.rows * 1000 / image2.cols));
	}

	testBF(image1, image2);

	return 0;
}