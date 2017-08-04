#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define UNKNOWN_FLOW_THRESH 1e9

// color encoding of flow vectors
void makeColorWheel(vector<Scalar>& colorWheel)
{
	int RY = 15;
	int YG = 6;
	int GC = 4;
	int CB = 11;
	int BM = 13;
	int MR = 6;

	for (int i = 0; i < RY; ++i)
		colorWheel.push_back(Scalar(255, 255 * i / RY, 0));
	for (int i = 0; i < YG; ++i)
		colorWheel.push_back(Scalar(255 - 255 * i / YG, 255, 0));
	for (int i = 0; i < GC; ++i)
		colorWheel.push_back(Scalar(0, 255, 255 * i / GC));
	for (int i = 0; i < CB; ++i)
		colorWheel.push_back(Scalar(0, 255 - 255 * i / CB, 255));
	for (int i = 0; i < BM; ++i)
		colorWheel.push_back(Scalar(255 * i / BM, 0, 255));
	for (int i = 0; i < MR; ++i)
		colorWheel.push_back(Scalar(255, 0, 255 - 255 * i / MR));
}

// convert motion to color
void motionToColor(Mat flow, Mat& color)
{
	if (color.empty())
		color.create(flow.rows, flow.cols, CV_8UC3);

	// RGB
	static vector<Scalar> colorwheel;
	if (colorwheel.empty())
		makeColorWheel(colorwheel);

	// determine motion range
	float maxrad = -1;

	// find max flow to normalize fx and fy
	for (int i = 0; i < flow.rows; ++i)
	{
		for (int j = 0; j < flow.cols; ++j)
		{
			Vec2f flowAtPoint = flow.at<Vec2f>(i, j);
			const float& fx = flowAtPoint[0];
			const float& fy = flowAtPoint[1];
			if ((fabs(fx) > UNKNOWN_FLOW_THRESH) || (fabs(fy) > UNKNOWN_FLOW_THRESH))
				continue;
			float rad = sqrt(fx * fx + fy * fy);
			maxrad = maxrad > rad ? maxrad : rad;
		}
	}

	// normalize
	for (int i = 0; i < flow.rows; ++i)
	{
		for (int j = 0; j < flow.cols; ++j)
		{
			uchar* data = color.data + color.step[0] * i + color.step[1] * j;
			Vec2f flowAtPoint = flow.at<Vec2f>(i, j);
			
			const float& fx = flowAtPoint[0] / maxrad;
			const float& fy = flowAtPoint[1] / maxrad;
			if ((fabs(fx) > UNKNOWN_FLOW_THRESH) || (fabs(fy) > UNKNOWN_FLOW_THRESH))
			{
				data[0] = data[1] = data[2] = 0;
				continue;
			}
			float rad = sqrt(fx * fx + fy * fy);

			float angle = atan2(-fy, -fx) / CV_PI;
			float fk = (angle + 1.0) / 2.0*(colorwheel.size() - 1);
			int k0 = static_cast<int>(fk);
			int k1 = (k0 + 1) % colorwheel.size();
			float f = fk - k0;

			for (int b = 0; b < 3; ++b)
			{
				float col0 = colorwheel[k0][b] / 255.0;
				float col1 = colorwheel[k1][b] / 255.0;
				float col = (1 - f) * col0 + f * col1;
				if (rad <= 1)
					col = 1 - rad * (1 - col);	// increase saturation with radius
				else
					col *= 0.75;	// out of range
				data[2 - b] = static_cast<int>(255.0*col);
			}
		}
	}
}


// optical flow using Lucas-Kanade, sparse optical flow
void testOpticalFlowLK()
{
	Mat preFrame = imread("../../../../Data/OpticalFlow/eval-data/Army/frame07.png", IMREAD_GRAYSCALE);
	Mat frame = imread("../../../../Data/OpticalFlow/eval-data/Army/frame08.png", IMREAD_GRAYSCALE);
	
	vector<Point2f> preFeatures, features;
	vector<uchar> status;
	vector<float> err;
	int maxCount = 300;
	double minDis = 10;
	double qLevel = 0.01;
	goodFeaturesToTrack(preFrame, preFeatures, maxCount, qLevel, minDis);
	// optical flow
	calcOpticalFlowPyrLK(preFrame, frame, preFeatures, features, status, err);
	// check moving key points
	int k = 0;
	for (int i = 0; i < features.size(); ++i)
	{
		if (status[i] && ((abs(preFeatures[i].x - features[i].x) + abs(preFeatures[i].y - features[i].y)) > 0))
		{
			features[k++] = features[i];
		}
	}
	features.resize(k);
	cout << "features count = " << k << endl;

	for (int i = 0; i < features.size(); ++i)
	{
		circle(frame, features[i], 3, Scalar(255, 0, 0), 1);
	}

	imshow("features", frame);
}

// optical flow using Lucas-Kanade, sparse optical flow
// Ref: Learn OpenCV3, p509, Example 16-1
void testOpticalFlowLK02()
{
	Mat imgA = imread("../../../../Data/OpticalFlow/eval-data/Backyard/frame07.png", IMREAD_GRAYSCALE);
	Mat imgB = imread("../../../../Data/OpticalFlow/eval-data/Backyard/frame08.png", IMREAD_GRAYSCALE);
	
	Mat imgC = imread("../../../../Data/OpticalFlow/eval-data/Backyard/frame08.png", IMREAD_UNCHANGED);
	Size imageSize = imgA.size();
	int winSize = 10;

	// get the features we want to track
	vector<Point2f> cornersA, cornersB;
	const int maxCorners = 500;
	goodFeaturesToTrack(imgA, cornersA, maxCorners, 0.01, 5, cv::noArray(), 3, false, 0.04);
	cornerSubPix(imgA, cornersA, Size(winSize, winSize), Size(-1, -1), TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 20, 0.03));

	// call the Lucus-Kanade algorithm
	vector<uchar> featuresFound;
	calcOpticalFlowPyrLK(imgA, imgB, cornersA, cornersB, featuresFound, noArray(), cv::Size(2 * winSize + 1, 2 * winSize + 1),
		5, TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 20, 0.30));

	// make some image of what we are looking at
	for (int i = 0; i < cornersA.size(); ++i)
	{
		if (!featuresFound[i])
			continue;
		arrowedLine(imgC, cornersA[i], cornersB[i], Scalar(0, 255, 0), 1, cv::LINE_AA);
	}

	imshow("LK Optical Flow Example", imgC);
}

// optical flow using Gunnar Farneback, dense optical flow
void testOpticalFlowFarneback()
{
	Mat preFrame = imread("../../../../Data/OpticalFlow/eval-data/Army/frame07.png", IMREAD_GRAYSCALE);
	Mat frame = imread("../../../../Data/OpticalFlow/eval-data/Army/frame08.png", IMREAD_GRAYSCALE);
	Mat flow;
	calcOpticalFlowFarneback(preFrame, frame, flow, 0.5, 3, 20, 3, 5, 1.2, 0);

	Mat motion2color;
	motionToColor(flow, motion2color);
	imshow("Optical Flow", motion2color);
}

int main()
{
	//testOpticalFlowLK();
	testOpticalFlowLK02();
	//testOpticalFlowFarneback();

	waitKey();
	return 0;
}