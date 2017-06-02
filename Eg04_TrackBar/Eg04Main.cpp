#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

const string windowName = "线性混合示例";	//窗口名

const int g_nMaxAlphaValue = 100;	//Alpha的最大值
int g_nAlphaValueSlider;			//滑动条对应的变量
double g_dAlphaValue;
double g_dBetaValue;


//全局变量
Mat g_srcImage1;
Mat g_srcImage2;
Mat g_dstImage;

void OnTrackbar(int, void*)
{
	//求出当前alpha值对应于最大值的比例
	g_dAlphaValue = (double)g_nAlphaValueSlider / g_nMaxAlphaValue;
	//beta值
	g_dBetaValue = 1.0 - g_dAlphaValue;
	//根据alpha和beta值进行线性混合
	addWeighted(g_srcImage1, g_dAlphaValue, g_srcImage2, g_dBetaValue, 0, g_dstImage);
	//显示效果
	imshow(windowName, g_dstImage);
}


int main()
{
	//加载图像
	g_srcImage1 = imread("..\\pic\\spring01.jpg");
	if (!g_srcImage1.data)
		cout << "图片1不存在!" << endl;
	g_srcImage2 = imread("..\\pic\\spring02.jpg");
	if (!g_srcImage2.data)
		cout << "图片2不存在!" << endl;
		 
	namedWindow(windowName);

	char trackbarName[50];
	sprintf(trackbarName, "透明值%d", g_nMaxAlphaValue);
	createTrackbar(trackbarName, windowName, &g_nAlphaValueSlider, g_nMaxAlphaValue, OnTrackbar);
	
	//结果在回调函数中显示
	OnTrackbar(g_nAlphaValueSlider, 0);

	waitKey(0);
	return 0;
}
