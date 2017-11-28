#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "ceres/ceres.h"
#include "ceres/rotation.h"

using namespace std;
using namespace cv;

// find image features and good matches
void findFeatureMatch(const Mat& img1, const Mat& img2, vector<KeyPoint>& keyPoints1, vector<KeyPoint>& keyPoints2,
	vector<DMatch>& goodMatches, bool showResult = true) {
	// take SIFT features
	Ptr<xfeatures2d::SIFT> detector = xfeatures2d::SIFT::create();
	//Ptr<ORB> detector = ORB::create(500, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE, 31, 20);
	Mat descriptors1, descriptors2;
	detector->detectAndCompute(img1, noArray(), keyPoints1, descriptors1);
	detector->detectAndCompute(img2, noArray(), keyPoints2, descriptors2);

	// match
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE);
	//Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE_HAMMING);		// if using ORB
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
		waitKey();
	}
}

// reproject error between 2 images taken by the same camera, and its intrinsics matrix is K with parameters fx, fy, cx, cy.
// The function calculate the reproject error between the global position viewed by position 1 and global position viewed by postion 2
struct ReprojectionError {
	ReprojectionError(double pos1x, double pos1y, double pos2x, double pos2y)
		: pos1x_(pos1x), pos1y_(pos1y), pos2x_(pos2x), pos2y_(pos2y) {}
	
	// K is camera intrinsics: [fx, 0, cx; 0, fy, cy; 0, 0, 1]
	// Rt is rotation(angle axies) and translation: [thetaX, thetaY, thetaZ, tx, ty, tz]
	template<typename T>
	bool operator()(const T* const K, const T* const Rt, T* residuals) const {
		const T fx = K[0];
		const T fy = K[4];
		const T cx = K[2];
		const T cy = K[5];

		// calculate the position in normalized plane at position 1
		T x1 = (pos1x_ - cx) / fx;
		T y1 = (pos1y_ - cy) / fy;

		// calculate the position in normalized plane at position 2
		T x2 = (pos2x_ - cx) / fx;
		T y2 = (pos2y_ - cy) / fy;

		// project pos1 from normalized plane to gobal plane, P = R * P0 + t;
		const T pos1[3] = { x1, y1, T(1.0) };	// normalized position
		T pos2from1[3];
		ceres::AngleAxisRotatePoint(Rt, pos1, pos2from1);
		pos2from1[0] += Rt[3];
		pos2from1[1] += Rt[4];
		pos2from1[2] += Rt[5];

		// the error is the difference between position calculated from pos1 and viewed at pos2
		residuals[0] = x2 - pos2from1[0];
		residuals[1] = y2 - pos2from1[1];
		residuals[2] = T(1.0) - pos2from1[2];

		return true;
	}

	const double pos1x_;
	const double pos1y_;
	const double pos2x_;
	const double pos2y_;
};



int main(int argc, char** argv) {
	Mat img1 = imread("../../data/SFM/1.png", IMREAD_UNCHANGED);
	Mat img2 = imread("../../data/SFM/2.png", IMREAD_UNCHANGED);
	
	// find feature and good matches
	vector<KeyPoint> keyPoints1, keyPoints2;
	vector<DMatch> goodMatches;
	findFeatureMatch(img1, img2, keyPoints1, keyPoints2, goodMatches, false);

	// take point in good matches to vector
	vector<Point2f> points1, points2;
	for (auto m : goodMatches) {
		points1.emplace_back(keyPoints1[m.trainIdx].pt);
		points2.emplace_back(keyPoints1[m.queryIdx].pt);
	}

	//// find the fundamental matrix, and filter the bad points
	//Mat F = findFundamentalMat(points1, points2, CV_FM_RANSAC);
	//vector<Point2f> correctPoints1, correctPoints2;
	//correctMatches(F, points1, points2, correctPoints1, correctPoints2);
	//cout << "F = \n" << F << endl;
	
		
	// bundle adjustment
	double K[9] = { 520.9, 0, 325.1, 0, 521.0, 249.7, 0, 0, 1 };	// camera intrinsics [fx, 0, cx, 0, fy, cy, 0, 0, 1]
	double Rt[6] = { CV_PI / 6, CV_PI / 6, CV_PI / 6, -1, 0, 0.5 };			// pos matrix [R, t] = [AngleAxis, t]
	ceres::Problem problem;
	for (int i = 0; i < goodMatches.size(); ++i) {
		ceres::CostFunction* costFunction = new ceres::AutoDiffCostFunction<ReprojectionError, 3, 9, 6>(
			new ReprojectionError(points1[i].x, points1[i].y, points2[i].x, points2[i].y));
		problem.AddResidualBlock(costFunction, new ceres::CauchyLoss(0.5), K, Rt);
	}
	ceres::Solver::Options options;
	options.max_num_iterations = 500;
	options.linear_solver_type = ceres::DENSE_QR;
	options.minimizer_progress_to_stdout = true;
	ceres::Solver::Summary summary;
	ceres::Solve(options, &problem, &summary);
	cout << summary.BriefReport() << endl;

	// print the result
	cout << "K = [";
	for (int i = 0; i < 9; ++i) {
		cout << K[i];
		if (i != 8)
			cout << ", ";
	}
	cout << "]\nRt = [";
	for (int i = 0; i < 6; ++i) {
		cout << Rt[i];
		if (i != 6)
			cout << ", ";
	}
	cout << "]" << endl;

	waitKey();
	system("pause");
	return 0;
}