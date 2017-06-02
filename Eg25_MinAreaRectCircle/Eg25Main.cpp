#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
	Mat image(600, 600, CV_8UC3);

	while (true)
	{
		int count = theRNG().uniform(3, 103);
		vector<Point> points;	//点值

		//随机生成点坐标
		for (int i = 0; i < count; ++i)
		{
			Point point;
			point.x = theRNG().uniform(image.cols / 4, image.cols * 3 / 4);
			point.y = theRNG().uniform(image.rows / 4, image.cols * 3 / 4);
			points.push_back(point);
		}
		//绘制出随机颜色的点
		image = Scalar::all(0);
		for (int i = 0; i < count; ++i)
		{
			circle(image, points[i], 3, Scalar(theRNG().uniform(0, 255), theRNG().uniform(0, 255), theRNG().uniform(0, 255)),
				FILLED, LINE_AA);
		}

		if (0)		//最小面积的矩形包围
		{
			//对给定的2D点集,寻找最小面积的包围矩形
			RotatedRect box = minAreaRect(Mat(points));
			Point2f vertex[4];
			box.points(vertex);

			//绘制出最小面积的包围矩形
			for (int i = 0; i < 4; ++i)
				line(image, vertex[i], vertex[(i + 1) % 4], Scalar(100, 200, 211), 2, LINE_AA);
		}
		else    //最小面积的圆包围
		{
			Point2f center;
			float radius = 0;
			minEnclosingCircle(Mat(points), center, radius);
			//绘制出最小面积的包围圆
			circle(image, center, radius, Scalar(theRNG().uniform(0, 255), theRNG().uniform(0, 255), theRNG().uniform(0, 255)),
				2, LINE_AA);

		}

		imshow("Image", image);

		char key = (char)waitKey();
		if (key == 27)
			break;
	}

	return 0;
}