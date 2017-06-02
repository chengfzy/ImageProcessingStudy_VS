#include <opencv2\opencv.hpp>
using namespace cv;

int main()
{
	//1. 点Point
	Point point;
	point.x = 10;
	point.y = 8;

	//2. 颜色Scalar
	Scalar(5, 50, 200);

	//3. 尺寸Size
	Size(5, 5);

	//4. 矩形Rect
	Rect rect1, rect2;
	
	//5.颜色空间转换cvtColor()

	//cvtColor(1,2,COLOR_RGB2BGR)
	system("pause");
}