#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

//全局变量
const string g_windowName = "程序窗口";	//窗口标题
Rect g_rectangle;
bool g_bDrawingBox = false;				//是否进行绘制
RNG g_rng(12345);

//函数声明
void OnMouseHandle(int event, int x, int y, int flags, void* param);
void DrawRectangle(Mat& img, Rect box);


//根据不同的鼠标事件进行不同的操作
void OnMouseHandle(int event, int x, int y, int flags, void* param)
{
	Mat& image = *(Mat*)param;
	switch (event)
	{
	case EVENT_MOUSEMOVE:	//鼠标移动
	{
		if (g_bDrawingBox)
		{
			g_rectangle.width = x - g_rectangle.x;
			g_rectangle.height = y - g_rectangle.y;
		}
	}
	break;
	case EVENT_LBUTTONDOWN:	//右键按下
	{
		g_bDrawingBox = true;
		g_rectangle = Rect(x, y, 0, 0);
	}
	break;
	case EVENT_LBUTTONUP:	//左键抬起
	{
		g_bDrawingBox = false;	//置标记符为false
		//对宽和高<0的处理
		if (g_rectangle.width < 0)
		{
			g_rectangle.x += g_rectangle.width;
			g_rectangle.width *= -1;
		}
		if (g_rectangle.height < 0)
		{
			g_rectangle.y += g_rectangle.height;
			g_rectangle.height *= -1;
		}
		//调用函数进行绘制
		DrawRectangle(image, g_rectangle);
	}
	break;
	default:
		break;
	}
}

//自定义的矩形绘制函数
void DrawRectangle(Mat& img, Rect box)
{
	//随机颜色
	rectangle(img, box.tl(), box.br(), Scalar(g_rng.uniform(0, 255), g_rng.uniform(0, 255), g_rng.uniform(0, 255)));
}

void ShowHelpText();

int main()
{
	//1. 准备参数
	g_rectangle = Rect(-1, -1, 0, 0);
	Mat srcImage(600, 800, CV_8UC3), tempImage;
	srcImage.copyTo(tempImage);
	srcImage = Scalar::all(0);

	//2. 设置鼠标操作回调函数
	namedWindow(g_windowName);
	setMouseCallback(g_windowName, OnMouseHandle, (void*)&srcImage);

	//3. 程序主循环，当进行绘制的标识符为真时，进行绘制
	while (1)
	{
		srcImage.copyTo(tempImage);		//复制源图到临时变量
		//当标识符为真时，则进行绘制
		if (g_bDrawingBox)
			DrawRectangle(tempImage, g_rectangle);
		imshow(g_windowName, tempImage);
		//按下ESC时程序退出
		if (waitKey(10) == 27)
			break;
	}

	return 0;
}