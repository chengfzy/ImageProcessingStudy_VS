#include <iostream>
#include <iomanip>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "NovelView.h"
#include "OpticalFlowVisualization.h"
#include "CvUtil.h"

using namespace std;
using namespace cv;
using namespace surround360;
using namespace surround360::util;
using namespace surround360::optical_flow;

string FLAGS_flow_alg = "pixflow_low";			// optical flow algorithm to us
const int FLAGS_repetitions = 1;				// number of times to repeat the flow calculation
const int FLAGS_num_intermediate_views = 11;	// number of views to make

// reads a pair of images specified by --left_img and --right_img. applies an optical flow
// algorithm specified by --flow_alg. generates a visualization of the flow field, and a
// sequence of novel views morphing between the two images. results are saved to
// subdirectories of --test_dir: /disparity and /novel_view
void testDisparity(const string imagePathL, const string imagePathR)
{
	Mat colorImageL = imread(imagePathL, IMREAD_UNCHANGED);
	Mat colorImageR = imread(imagePathR, IMREAD_UNCHANGED);

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



int TestOpticalFlow(int argc, char** argv)
{
	string imagePathL = "D:/Code/data/300/modelseq0_idZCAM00.png";
	string imagePathR = "D:/Code/data/300/modelseq1_idZCAM07.png";

	testDisparity(imagePathL, imagePathR);

	return 0;
}
