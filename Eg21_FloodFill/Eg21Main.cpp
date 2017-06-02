#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

Mat g_srcImage, g_dstImage, g_grayImage, g_maskImage;		//原始图,目标图,灰度图,掩模图
int g_nFillMode = 1;				//漫水填充的模式
int g_nLowDifference = 20, g_nUpDifference = 20;			//负差最大值,正差最大值
int g_nConnectivity = 4;			//表示floodFill函数标识符低八位的连通值
int g_bIsColor = true;				//是否为彩色图的标识符
bool g_bUseMask = false;			//是否显示掩膜窗口的布尔值
int g_nNewMaskVal = 255;			//新的重新绘制的像素值

//鼠标消息回调函数
static void OnMouse(int event, int x, int y, int, void*)
{
	//若鼠标按下,便返回
	if (event != EVENT_LBUTTONDOWN)
		return;

	//调用floodFill函数之前的参数准备部分
	Point seed = Point(x, y);
	int LowDifference = g_nFillMode == 0 ? 0 : g_nLowDifference;		//空范围的漫水填充
	int UpDifference = g_nFillMode == 0 ? 0 : g_nUpDifference;			//空范围的漫水填充
	//标记符的0-7位为g_nConnectivity, 8-15位为g_nNewMaskVal左移8位的值,16-23位为CV_FLOODFILL_FIXED_RANGE或者0
	int flags = g_nConnectivity + (g_nNewMaskVal << 8) + (g_nFillMode == 1 ? FLOODFILL_FIXED_RANGE : 0);

	//随机生成bgr值
	int b = (unsigned)theRNG() & 255;
	int g = (unsigned)theRNG() & 255;
	int r = (unsigned)theRNG() & 255;
	Rect ccomp;			//定义重绘区域的最小边界矩形区域

	Scalar newVal = g_bIsColor ? Scalar(b, g, r) : Scalar(r* 0.299 + g * 0.587 + b * 0.114);	//在重绘区域像素的新值	
	Mat dst = g_bIsColor ? g_dstImage : g_grayImage;	//目标图的赋值
	int area;

	//2.正式调用floodFill函数
	if (g_bUseMask)
	{
		threshold(g_maskImage, g_maskImage, 1, 128, THRESH_BINARY);
		area = floodFill(dst, g_maskImage, seed, newVal, &ccomp, Scalar(LowDifference, LowDifference, LowDifference),
			Scalar(UpDifference, UpDifference, UpDifference), flags);
		imshow("mask", g_maskImage);
	}
	else
	{
		area = floodFill(dst, seed, newVal, &ccomp, Scalar(LowDifference, LowDifference, LowDifference),
			Scalar(UpDifference, UpDifference, UpDifference), flags);		
	}
	imshow("Result", dst);
}


int main()
{
	g_srcImage = imread("..\\pic\\cat.jpg");
	if (!g_srcImage.data)
		cout << "Read Image Error" << endl;

	g_srcImage.copyTo(g_dstImage);
	cvtColor(g_srcImage, g_grayImage, COLOR_BGR2GRAY);
	g_maskImage.create(g_srcImage.rows + 2, g_srcImage.cols + 2, CV_8UC1);		//利用srcImage尺寸来创建掩膜mask

	namedWindow("Result");
	createTrackbar("负差最大值", "Result", &g_nLowDifference, 255, 0);
	createTrackbar("正差最大值", "Result", &g_nUpDifference, 255, 0);
	setMouseCallback("Result", OnMouse, 0);


	//循环轮询按键
	while (1)
	{
		//先显示效果图
		imshow("Result", g_bIsColor ? g_dstImage : g_grayImage);

		//获取键盘按键
		int c = waitKey();
		//判断ESC是否按下,若按下便退出
		if ((c & 255) == 27)
		{
			cout << "exiting..." << endl;
			break;
		}

		//根据按键的不同,进行各种操作
		switch ((char)c)
		{
		case '1':		//1:效果图在彩色图、灰度图之间转换
			if (g_bIsColor)
			{
				cout << "键盘1被按下,切换彩色/灰度模式,当前操作为彩色==>灰度" << endl;
				cvtColor(g_srcImage, g_grayImage, COLOR_BGR2GRAY);
				g_maskImage = Scalar::all(0);
				g_bIsColor = false;
			}
			else
			{
				cout << "键盘1被按下,切换彩色/灰度模式,当前操作为灰度==>彩色" << endl;
				g_srcImage.copyTo(g_dstImage);
				g_maskImage = Scalar::all(0);
				g_bIsColor = true;
			}
			break;
		case '2':		//2.显示/隐藏掩膜窗口
			if (g_bUseMask)
			{
				destroyWindow("mask");
				g_bUseMask = false;
			}
			else
			{
				namedWindow("mask", 0);
				g_maskImage = Scalar::all(0);
				imshow("mask", g_maskImage);
				g_bUseMask = true;
			}
			break;
		case '3':		//3: 恢复原始图像
			cout << "3: 恢复原始图像" << endl;
			g_srcImage.copyTo(g_dstImage);
			cvtColor(g_dstImage, g_grayImage, COLOR_BGR2GRAY);
			g_maskImage = Scalar::all(0);
			break;
		case '4':		//4:使用空范围的漫水填充
			cout << "4:使用空范围的漫水填充" << endl;
			g_nFillMode = 0;
			break;
		case '5':		//5:使用渐变,固定范围的漫水填充
			cout << "5:使用渐变,固定范围的漫水填充" << endl;
			g_nFillMode = 1;
			break;
		case '6':		//6:使用渐变,浮动范围的漫水填充
			cout << "6:使用渐变,浮动范围的漫水填充" << endl;
			g_nFillMode = 2;
			break;
		case 7:			//7:操作标志符的低8位使用4位的连接模式
			cout << "7:操作标志符的低8位使用4位的连接模式" << endl;
			g_nConnectivity = 4;
			break;
		case '8':		//8:操作标志符的低8位使用8位的连接模式
			cout << "8:操作标志符的低8位使用8位的连接模式" << endl;
			g_nConnectivity = 8;
			break;
		}
	}

	return 0;
}