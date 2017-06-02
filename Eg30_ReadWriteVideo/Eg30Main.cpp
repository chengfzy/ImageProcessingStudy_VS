#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	//string inFile = "G:\\CalibrationData\\15030010054\\temp.mov";
	string inFile = "G:\\CalibrationData\\pano.mov";
	string outFile = "G:\\CalibrationData\\pano.mov";
	vector<Mat*> apFrame;

	VideoCapture capture(inFile);
	if (!capture.isOpened())
		return -1;

	//get frame count
	int frameCount = capture.get(CAP_PROP_FRAME_COUNT);
	//get frame rate
	double frameRate = capture.get(CAP_PROP_FPS);
	//get frame size
	Size frameSize(capture.get(CAP_PROP_FRAME_WIDTH), capture.get(CAP_PROP_FRAME_HEIGHT));

	//read video frames
	int pos{ 0 };
	Mat frame, previewFrame;
	while (capture.read(frame))
	{
		apFrame.push_back(new Mat(frame.clone()));
		pos++;
	}

	//write video
	//Video Writer
	VideoWriter* pVideoWriter = new VideoWriter();
	pVideoWriter->open(outFile, CV_FOURCC('M', 'J', 'P', 'G'), frameRate, frameSize);
	if (!pVideoWriter->isOpened())
	{
		cout << "Open Video File Error" << endl;
		return -1;
	}
	//write to video
	for (auto image : apFrame)
	{
		pVideoWriter->write(*image);
	}
	delete pVideoWriter;


	//release memory
	for (auto image : apFrame)
	{
		delete image;
	}

	return 0;
}