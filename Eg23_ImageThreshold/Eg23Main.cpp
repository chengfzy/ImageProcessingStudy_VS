#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

Mat g_grayImage, g_dstImage;
const string g_szWindowName = "Image Threshold";
int g_nThresholdValue = 100;
int g_nThresholdType = 3;

//阈值回调函数
void OnThreshold(int, void*)
{
	threshold(g_grayImage, g_dstImage, g_nThresholdValue, 255, g_nThresholdType);
	imshow(g_szWindowName, g_dstImage);
}

int main()
{
	Mat srcImage = imread("..\\pic\\1.jpg");
	if (!srcImage.data)
		cout << "Read Image Error" << endl;

	//保存一份原图的灰度图
	cvtColor(srcImage, g_grayImage, COLOR_RGB2GRAY);
	
	//创建窗口并显示原始图
	namedWindow(g_szWindowName, WINDOW_AUTOSIZE);

	//创建滑动条来控制阈值
	createTrackbar("Mode", g_szWindowName, &g_nThresholdType, 4, OnThreshold);
	createTrackbar("Parameters", g_szWindowName, &g_nThresholdValue, 255, OnThreshold);

	//初始化自定义的阈值回调函数
	OnThreshold(0, 0);

	waitKey();
	return 0;
}