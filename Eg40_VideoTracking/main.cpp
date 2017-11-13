#include <iostream>
#include <iomanip>
#include "opencv2/video.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

Mat image;
bool backprojMode{ false };
bool selectObject{ false };
int trackObject{ 0 };
bool showHist{ true };
Point origin;
Rect selection;
int vmin{ 10 }, vmax{ 256 }, smin{ 30 };

// user draws box around object to track, this triggers CAMShift to start tracking
void onMouse(int event, int x, int y, int, void*) {
	if (selectObject) {
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);
		selection.height = std::abs(y - origin.y);

		selection &= Rect(0, 0, image.cols, image.rows);
	}

	switch (event) {
	case EVENT_LBUTTONDOWN:
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);
		selectObject = true;
		break;
	case EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
			trackObject = -1;   // Set up CAMShift properties in main() loop
		break;
	}
}

int main() {
	//VideoCapture cap;
	//cap.open("D:/testdata/videos/1.avi");
	//if (!cap.isOpened()) {
	//	cout << "could not open video." << endl;
	//	return -1;
	//}

	Rect trackWindow;
	int hsize{ 16 };
	float hranges[] = { 0, 180 };
	const float* phranges = hranges;


	namedWindow("Histogram", WINDOW_KEEPRATIO);
	namedWindow("CAMShift Demo", WINDOW_KEEPRATIO);
	setMouseCallback("CAMShift Demo", onMouse);
	createTrackbar("VMin", "CAMShift Demo", &vmin, 256);
	createTrackbar("VMax", "CAMShift Demo", &vmax, 256);
	createTrackbar("SMin", "CAMShift Demo", &smin, 256);

	Mat frame, hsv, hue, mask, hist, histImg = Mat::zeros(200, 320, CV_8UC3), backproj;
	bool paused{ false };

	for (int n = 0; n < 6290; ++n) {
		if (!paused) {
			//cap >> frame;
			//if (frame.empty())
			//	break;
			
			stringstream ss;
			cout << setw(4) << setfill('0') << 4 << endl;
			ss << "D:/Code/data/BlurOwl/BlurOwl/img/" << setw(4) << setfill('0') << n + 1 << ".jpg";
			//ss << "D:/Download/TrackingDataset_TV77/TrackingDataset/BoBot/Vid_B_cup/img" << setw(4) << setfill('0') << n / 10 + 2 << ".jpg";
			frame = imread(ss.str(), IMREAD_UNCHANGED);
		}

		frame.copyTo(image);

		if (!paused) {
			cvtColor(image, hsv, COLOR_BGR2HSV);

			if (trackObject) {
				int _vmin = vmin, _vmax = vmax;
				inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)), Scalar(180, 256, MAX(_vmin, _vmax)), mask);
				int ch[] = { 0,0 };
				hue.create(hsv.size(), hsv.depth());
				mixChannels(&hsv, 1, &hue, 1, ch, 1);

				if (trackObject < 0) {
					// object has been selected by user, set up CAMShift search properties once
					Mat roi(hue, selection), maskroi(mask, selection);
					calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
					normalize(hist, hist, 0, 255, NORM_MINMAX);

					trackWindow = selection;
					trackObject = 1;		// don't set up again, unless user selects new ROI

					histImg = Scalar::all(0);
					int binW = histImg.cols / hsize;
					Mat buf(1, hsize, CV_8UC3);
					for (size_t i = 0; i < hsize; i++) {
						buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i * 180.0 / hsize), 255, 255);						
					}
					cvtColor(buf, buf, COLOR_HSV2BGR);

					for (size_t i = 0; i < hsize; i++) {
						int val = saturate_cast<int>(hist.at<float>(i) * histImg.rows / 255);
						rectangle(histImg, Point(i * binW, histImg.rows), Point((i + 1) * binW, histImg.rows - val), Scalar(buf.at<Vec3b>(i)), -1);
					}
				}

				// perform CAMShift
				calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
				backproj &= mask;
				RotatedRect trackBox = CamShift(backproj, trackWindow, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

				if (trackWindow.area() <= 1) {
					int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
					trackWindow = Rect(trackWindow.x - r, trackWindow.y - r, trackWindow.x + r, trackWindow.y + r) & Rect(0, 0, cols, rows);					
				}

				if (backprojMode)
					cvtColor(backproj, image, COLOR_GRAY2BGR);
				ellipse(image, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
			} else if (trackObject < 0) {
				paused = false;
			}

			if (selectObject && selection.width > 0 && selection.height > 0) {
				Mat roi(image, selection);
				bitwise_not(roi, roi);
			}

			imshow("CAMShift Demo", image);
			imshow("Histogram", histImg);

			char c = (char)waitKey(10);
			if (c == 27)
				break;
			switch (c) {
			case 'b':
				backprojMode = !backprojMode;
				break;
			case 'c':
				trackObject = 0;
				histImg = Scalar::all(0);
				break;
			case 'h':
				showHist = !showHist;
				if (!showHist)
					destroyWindow("Histogram");
				else
					namedWindow("Histogram", 1);
				break;
			case 'p':
				paused = !paused;
				break;
			default:
				;
			}

		}
	}

	return 0;
}