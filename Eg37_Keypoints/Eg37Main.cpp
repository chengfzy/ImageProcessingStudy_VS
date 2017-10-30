#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"

using namespace std;
using namespace cv;

// convert to gray image
Mat convertToGray(const Mat& image) {
	Mat gray;
	switch (image.channels()) {
	case 1:
		gray = image;
		break;
	case 3:
		cvtColor(image, gray, COLOR_BGR2GRAY);
		break;
	case 4:
		cvtColor(image, gray, COLOR_BGRA2GRAY);
		break;
	default:
		break;
	}

	return gray;
}

// Harris-Shi-Tomasi detector
void testHarris(const Mat& image) {
	Mat grayImage = convertToGray(image);

	// detect
	vector<Point2f> harrisFeatures, shiFeatures;
	goodFeaturesToTrack(grayImage, harrisFeatures, 300, 0.01, 5, noArray(), 3, true, 0.04);
	printf("Harris: keypoints = %d, no descriptor implement\n", harrisFeatures.size());

	goodFeaturesToTrack(grayImage, shiFeatures, 300, 0.01, 5, noArray(), 3, false);
	printf("Shi-Tomasi: keypoints = %d, no descriptor implement\n", shiFeatures.size());	

	// draw Harris corners	
	Mat harrisDst = image.clone();
	for (auto p : harrisFeatures) {
		circle(harrisDst, p, 5, Scalar(0, 255, 0));
	}
	imshow("Harris", harrisDst);

	// draw Shi-Tomasi corners
	Mat shiDst = image.clone();
	for (auto p : shiFeatures) {
		circle(shiDst, p, 5, Scalar(0, 255, 0));
	}
	imshow("Shi-Tomasi", shiDst);
}

// Harris-Shi-Tomasi detector, another method
void testHarrisB(const Mat& image) {
	Mat grayImage = convertToGray(image);
	
	// Harris
	vector<KeyPoint> harrisKeypoints;
	Ptr<GFTTDetector> harrisDetector = GFTTDetector::create(300, 0.01, 5, 3, true, 0.04);
	harrisDetector->detect(grayImage, harrisKeypoints);
	printf("Harris: keypoints = %d, no descriptor implement\n", harrisKeypoints.size());

	// Shi-Tomasi
	vector<KeyPoint> shiKeypoints;
	Ptr<GFTTDetector> shiDetector = GFTTDetector::create(300, 0.01, 5, 3, false);
	shiDetector->detect(grayImage, shiKeypoints);
	printf("Shi-Tomasi: keypoints = %d, no descriptor implement\n", shiKeypoints.size());

	// draw Harris keypoints
	Mat harrisDst;
	drawKeypoints(image, harrisKeypoints, harrisDst, Scalar(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("Harris", harrisDst);

	// draw Shi-Tomasi corners
	Mat shiDst;
	drawKeypoints(image, shiKeypoints, shiDst, Scalar(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("Shi-Tomasi", shiDst);
}

// Simple Blob detector
void testSimpleBlob(const Mat& image) {
	Mat grayImage = convertToGray(image);

	// detect
	SimpleBlobDetector::Params params;

	params.minDistBetweenBlobs = 10;
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	vector<KeyPoint> keypoints;
	detector->detect(grayImage, keypoints);
	printf("Simple Blob: keypoints = %d, no descriptor implement\n", keypoints.size());

	// draw keypoints
	Mat dst;
	drawKeypoints(image, keypoints, dst, Scalar(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("Simple Blob", dst);	
}

// FAST(Features from Accelerated Segments Test) detector
void testFAST(const Mat& image) {
	Mat grayImage = convertToGray(image);

	// detect
	Ptr<FastFeatureDetector> detector = FastFeatureDetector::create(80, true, FastFeatureDetector::TYPE_9_16);
	vector<KeyPoint> keypoints;
	detector->detect(grayImage, keypoints);
	printf("FAST(Features from Accelerated Segments Test): keypoints = %d, no descriptor implement\n", keypoints.size());

	// draw keypoints
	Mat dst;
	drawKeypoints(image, keypoints, dst, Scalar(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("FAST", dst);
}

// SIFT(Scale Invariant Feature Transform) detector
void testSIFT(const Mat& image) {
	Mat grayImage = convertToGray(image);

	// detect
	Ptr<xfeatures2d::SIFT> detector = xfeatures2d::SIFT::create(0);
	vector<KeyPoint> keypoints;
	Mat descriptors;
	detector->detect(grayImage, keypoints);
	detector->compute(grayImage, keypoints, descriptors);
	printf("SIFT(Scale Invariant Feature Transform): keypoints = %d, descriptor = (w%d, h%d)\n",
		keypoints.size(), descriptors.cols, descriptors.rows);

	// draw keypoints
	Mat dst;
	drawKeypoints(image, keypoints, dst, Scalar(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("SIFT", dst);
}



int main() {
	Mat image = imread("../../../../Data/stillA.jpg", IMREAD_UNCHANGED);
	if (image.empty()) {
		cout << "cannot read the image..." << endl;
		return -1;
	}

	// resize the image if too large
	if (image.rows > 1000 || image.cols > 1000) {
		resize(image, image, Size(1000, image.rows * 1000 / image.cols));
	}

	//testHarris(image);
	//testHarrisB(image);			// another calling method
	//testSimpleBlob(image);
	testFAST(image);
	testSIFT(image);

	waitKey();
	return 0;
}