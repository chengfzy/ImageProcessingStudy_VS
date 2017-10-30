#include <iostream>
#include <iomanip>
#define _USE_MATH_DEFINES
#include <math.h>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "NovelView.h"
#include "OpticalFlowVisualization.h"
#include "CvUtil.h"
#include "MathUtil.h"

using namespace std;
using namespace cv;
using namespace surround360;
using namespace surround360::util;
using namespace surround360::math_util;
using namespace surround360::optical_flow;

string FLAGS_flow_alg = "pixflow_low";			// optical flow algorithm to us
const int FLAGS_repetitions = 1;				// number of times to repeat the flow calculation
const int FLAGS_num_intermediate_views = 11;	// number of views to make

												// reads a pair of images specified by --left_img and --right_img. applies an optical flow
												// algorithm specified by --flow_alg. generates a visualization of the flow field, and a
												// sequence of novel views morphing between the two images. results are saved to
												// subdirectories of --test_dir: /disparity and /novel_view
void stitch(const string imagePathL, const string imagePathR)
{
	// read image
	Mat colorImageL = imread(imagePathL, IMREAD_UNCHANGED);
	Mat colorImageR = imread(imagePathR, IMREAD_UNCHANGED);

	// parameters obtained by JSON
	float cameraRingRadius = 50;
	const double fovHorizontal = 2 * 60 * (180 / M_PI);

	vector<Mat> projectionImages;		// projection to spherical coordinates
	Mat sphericalImageL, sphericalImageR;
	double opticalFlowRuntime, novelViewRuntime;

	// generate novel views and stereo spherical panoramas
	const int numCams = projectionImages.size();

	// this is the amount of horizontal overlap the cameras would have if they
	// were all perfectly aligned (in fact due to misalignment they overlap by a
	// different amount for each pair, but we ignore that to make it simple)
	const float fovHorizontalRadians = toRadians(fovHorizontal);
	const float overlapAngleDegrees =
		(camFovHorizontalDegrees * float(numCams) - 360.0) / float(numCams);
	const int camImageWidth = projectionImages[0].cols;
	const int camImageHeight = projectionImages[0].rows;
	const int overlapImageWidth =
		float(camImageWidth) * (overlapAngleDegrees / camFovHorizontalDegrees);
	const int numNovelViews = camImageWidth - overlapImageWidth; // per image pair

																 // setup parallel optical flow
	double startOpticalFlowTime = getCurrTimeSec();
	vector<NovelViewGenerator*> novelViewGenerators(projectionImages.size());
	vector<std::thread> threads;
	for (int leftIdx = 0; leftIdx < projectionImages.size(); ++leftIdx) {
		const int rightIdx = (leftIdx + 1) % projectionImages.size();
		novelViewGenerators[leftIdx] =
			new NovelViewGeneratorAsymmetricFlow(FLAGS_side_flow_alg);
		threads.push_back(std::thread(
			prepareNovelViewGeneratorThread,
			overlapImageWidth,
			leftIdx,
			&projectionImages[leftIdx],
			&projectionImages[rightIdx],
			novelViewGenerators[leftIdx]
		));
	}
	for (std::thread& t : threads) { t.join(); }

	opticalFlowRuntime = getCurrTimeSec() - startOpticalFlowTime;

	// lightfield/parallax formulas
	const float v =
		atanf(FLAGS_zero_parallax_dist / (FLAGS_interpupilary_dist / 2.0f));
	const float psi =
		asinf(sinf(v) * (FLAGS_interpupilary_dist / 2.0f) / cameraRingRadius);
	const float vergeAtInfinitySlabDisplacement =
		psi * (float(camImageWidth) / fovHorizontalRadians);
	const float theta = -M_PI / 2.0f + v + psi;
	const float zeroParallaxNovelViewShiftPixels =
		float(FLAGS_eqr_width) * (theta / (2.0f * M_PI));

	double startNovelViewTime = getCurrTimeSec();
	// a "chunk" will be just the part of the panorama formed from one pair of
	// adjacent cameras. we will stack them horizontally to build the full
	// panorama. we do this so it can be parallelized.
	vector<Mat> panoChunksL(projectionImages.size(), Mat());
	vector<Mat> panoChunksR(projectionImages.size(), Mat());
	vector<std::thread> panoThreads;
	for (int leftIdx = 0; leftIdx < projectionImages.size(); ++leftIdx) {
		panoThreads.push_back(std::thread(
			renderStereoPanoramaChunksThread,
			leftIdx,
			numCams,
			camImageWidth,
			camImageHeight,
			numNovelViews,
			fovHorizontalRadians,
			vergeAtInfinitySlabDisplacement,
			novelViewGenerators[leftIdx],
			&panoChunksL[leftIdx],
			&panoChunksR[leftIdx]
		));
	}
	for (std::thread& t : panoThreads) { t.join(); }

	novelViewRuntime = getCurrTimeSec() - startNovelViewTime;

	for (int leftIdx = 0; leftIdx < projectionImages.size(); ++leftIdx) {
		delete novelViewGenerators[leftIdx];
	}

	panoImageL = stackHorizontal(panoChunksL);
	panoImageR = stackHorizontal(panoChunksR);

	panoImageL = offsetHorizontalWrap(panoImageL, zeroParallaxNovelViewShiftPixels);
	panoImageR = offsetHorizontalWrap(panoImageR, -zeroParallaxNovelViewShiftPixels);






















	// we need alpha channels for flow. if they are missing, convert
	if (colorImageL.type() == CV_8UC3)
	{
		cvtColor(colorImageL, colorImageL, CV_BGR2BGRA);
	}
	if (colorImageR.type() == CV_8UC3)
	{
		cvtColor(colorImageR, colorImageR, CV_BGR2BGRA);
	}

	for (int rep = 0; rep < FLAGS_repetitions; ++rep)
	{
		cout << "---- repetition " << rep << endl;

		cout << "constructing novel view generator" << endl;
		NovelViewGenerator* novelViewGen = new NovelViewGeneratorAsymmetricFlow(FLAGS_flow_alg);

		cout << "calling prepare" << endl;
		//double prepareStartTime = getCurrTimeSec();
		novelViewGen->prepare(colorImageL, colorImageR);
		//double prepareEndTime = getCurrTimeSec();
		//cout << "RUNTIME (sec) = " << (prepareEndTime - prepareStartTime) << endl;

		cout << "building visualizations" << endl;

		Mat flowVisLtoR = visualizeFlowAsGreyDisparity(novelViewGen->getFlowLtoR());
		Mat flowVisRtoL = visualizeFlowAsGreyDisparity(novelViewGen->getFlowRtoL());
		Mat flowVisLtoRColorWheel = visualizeFlowColorWheel(novelViewGen->getFlowLtoR());
		Mat flowVisRtoLColorWheel = visualizeFlowColorWheel(novelViewGen->getFlowRtoL());
		Mat flowVisLtoRColorWithLines = visualizeFlowAsVectorField(novelViewGen->getFlowLtoR(), colorImageL);
		Mat flowVisRtoLColorWithLines = visualizeFlowAsVectorField(novelViewGen->getFlowRtoL(), colorImageR);

		cvtColor(flowVisRtoL, flowVisRtoL, CV_GRAY2BGRA);
		cvtColor(flowVisLtoR, flowVisLtoR, CV_GRAY2BGRA);
		cvtColor(flowVisLtoRColorWheel, flowVisLtoRColorWheel, CV_BGR2BGRA);
		cvtColor(flowVisRtoLColorWheel, flowVisRtoLColorWheel, CV_BGR2BGRA);

		Mat horizontalVisLtoR = stackHorizontal(vector<Mat>({ flowVisLtoR, flowVisLtoRColorWheel, flowVisLtoRColorWithLines }));
		Mat horizontalVisRtoL = stackHorizontal(vector<Mat>({ flowVisRtoL, flowVisRtoLColorWheel, flowVisRtoLColorWithLines }));

		//imwriteExceptionOnFail(
		//	FLAGS_test_dir + "/disparity/LtoR_" + FLAGS_flow_alg + ".png",
		//	horizontalVisLtoR);

		//imwriteExceptionOnFail(
		//	FLAGS_test_dir + "/disparity/RtoL_" + FLAGS_flow_alg + ".png",
		//	horizontalVisRtoL);

		//system(string("rm " + FLAGS_test_dir + "/novel_view/*").c_str());

		for (int v = 0; v < FLAGS_num_intermediate_views; ++v) {
			const double shiftFromLeft =
				double(v) / double(FLAGS_num_intermediate_views - 1);

			Mat novelViewMerged = Mat(); // init here so we don't crash if nothing
			Mat novelViewFromL = Mat();  // is written
			Mat novelViewFromR = Mat();
			novelViewGen->generateNovelView(
				shiftFromLeft, novelViewMerged, novelViewFromL, novelViewFromR);

			stringstream ss;
			ss << std::setfill('0') << std::setw(6) << v;
			const string frameIdxPadded = ss.str();

			//imwriteExceptionOnFail(
			//	FLAGS_test_dir + "/novel_view/" + frameIdxPadded + ".png",
			//	novelViewMerged);

			//if (FLAGS_save_asymmetric_novel_views) {
			//	imwriteExceptionOnFail(
			//		FLAGS_test_dir + "/novel_view/novelFromL_" + frameIdxPadded + ".png",
			//		novelViewFromL);

			//	imwriteExceptionOnFail(
			//		FLAGS_test_dir + "/novel_view/novelFromR_" + frameIdxPadded + ".png",
			//		novelViewFromR);
			//}
		}

		delete novelViewGen;
	}
}



int main(int argc, char** argv)
{
	string imagePathL = "D:/Code/data/300/modelseq0_idZCAM00.png";
	string imagePathR = "D:/Code/data/300/modelseq1_idZCAM07.png";

	stitch(imagePathL, imagePathR);

	return 0;
}
