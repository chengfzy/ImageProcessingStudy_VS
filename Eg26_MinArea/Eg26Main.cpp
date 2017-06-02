#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

const string g_szOriginWindowName = "Origin";
const string g_szResultWindowName = "Result";
Mat g_srcImage, g_grayImage, g_dstImage;
int g_nThresh = 50;				//阈值
int g_nMaxThresh = 255;			//阈值最大值


void OnContoursChange(int, void*)
{
	Mat thresholdImage;
	vector<vector<Point>> vContours;
	vector<Vec4i> vHierarchy;

	//使用threshold检测边缘
	threshold(g_grayImage, thresholdImage, g_nThresh, 255, THRESH_BINARY);

	//找出轮廓
	findContours(thresholdImage, vContours, vHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//多边形逼近轮廓+获取矩形和圆形边界框
	vector<vector<Point>> polyContours(vContours.size());
	vector<Rect> rectBound(vContours.size());
	vector<Point2f> center(vContours.size());
	vector<float> radius(vContours.size());

	//遍历所有部分
	for (unsigned int i = 0; i < vContours.size(); ++i)
	{
		approxPolyDP(Mat(vContours[i]), polyContours[i], 3, true);
		rectBound[i] = boundingRect(Mat(polyContours[i]));
		minEnclosingCircle(polyContours[i], center[i], radius[i]);
	}

	//绘制多边形轮廓+包围的矩形框+圆形框
	Mat drawing = Mat::zeros(thresholdImage.size(), CV_8UC3);
	for (unsigned int i = 0; i < vContours.size(); ++i)
	{
		Scalar color = Scalar(theRNG().uniform(0, 255), theRNG().uniform(0, 255), theRNG().uniform(0, 255));
		drawContours(drawing, polyContours, i, color, 1, LINE_AA, vector<Vec4i>(), 0, Point());
		rectangle(drawing, rectBound[i].tl(), rectBound[i].br(), color, 2, LINE_AA, 0);
	}

	imshow(g_szResultWindowName, drawing);
}

int main()
{
	g_srcImage = imread("..\\pic\\cat.jpg");
	if (!g_srcImage.data)
		cout << "Read Image Error" << endl;

	//得到原图的灰度图像并进行平滑
	cvtColor(g_srcImage, g_grayImage, COLOR_BGR2GRAY);
	blur(g_grayImage, g_grayImage, Size(3, 3));

	//创建原始图窗口并显示
	namedWindow(g_szOriginWindowName, WINDOW_AUTOSIZE);
	imshow(g_szOriginWindowName, g_srcImage);

	createTrackbar("Threshold", g_szOriginWindowName, &g_nThresh, g_nMaxThresh, OnContoursChange);
	OnContoursChange(0, 0);

	waitKey();
	return 0;
}