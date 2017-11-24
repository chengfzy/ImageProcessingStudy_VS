#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"

using namespace std;
using namespace cv;

// find image features and good matches
void findFeatureMatch(const Mat& img1, const Mat& img2, vector<KeyPoint>& keyPoints1, vector<KeyPoint>& keyPoints2,
	vector<DMatch>& goodMatches, bool showResult = true) {
	// take SIFT features
	Ptr<xfeatures2d::SIFT> detector = xfeatures2d::SIFT::create();
	//Ptr<ORB> detector = ORB::create(500, 1.2f, 8, 21, 0, 2, ORB::HARRIS_SCORE, 21, 20);
	Mat descriptors1, descriptors2;
	detector->detectAndCompute(img1, noArray(), keyPoints1, descriptors1);
	detector->detectAndCompute(img2, noArray(), keyPoints2, descriptors2);

	// match
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE);
	vector<DMatch> matches;
	matcher->match(descriptors1, descriptors2, matches);

	// find the matches' min/max distance
	double minDist{ 10000 }, maxDist{ 0 };
	for (int i = 0; i < descriptors1.rows; ++i) {
		const double dist = matches[i].distance;
		if (dist < minDist) minDist = dist;
		if (dist > maxDist) maxDist = dist;
	}
	cout << "Min dist = " << minDist << ", Max dist = " << maxDist << endl;

	// select, only keep the distance < 2 * minDist
	for (int i = 0; i < descriptors1.rows; ++i) {
		if (matches[i].distance <= max(2 * minDist, 80.0))	// set the experienced min limit
			goodMatches.emplace_back(matches[i]);
	}
	cout << "All Matches = " << matches.size() << ", good matches = " << goodMatches.size() << endl;

	// show result
	if (showResult) {
		Mat imgMatch, imgGoodMatch;
		drawMatches(img1, keyPoints1, img2, keyPoints2, matches, imgMatch/*, Scalar::all(-1), Scalar::all(-1),
																		 vector<char>(), DrawMatchesFlags::DRAW_RICH_KEYPOINTS*/);
		drawMatches(img1, keyPoints1, img2, keyPoints2, goodMatches, imgGoodMatch);
		imshow("All Match", imgMatch);
		imshow("Good Match", imgGoodMatch);
	}
}

int main(int argc, char** argv) {
	Mat img1 = imread("../../data/SFM/1.png", IMREAD_UNCHANGED);
	Mat img2 = imread("../../data/SFM/2.png", IMREAD_UNCHANGED);
	
	// find feature and good matches
	vector<KeyPoint> keyPoints1, keyPoints2;
	vector<DMatch> goodMatches;
	findFeatureMatch(img1, img2, keyPoints1, keyPoints2, goodMatches);
	cout << "find matches size = " << goodMatches.size() << endl;



	waitKey();
	return 0;
}