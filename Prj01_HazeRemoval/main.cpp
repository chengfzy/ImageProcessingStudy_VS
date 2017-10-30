#include <iostream>
#include "HazeRemoval.h"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("../../../data/haze/tiananmen1.bmp", IMREAD_UNCHANGED);
	//Mat src = imread("../../../data/haze/tiananmen1_res.png", IMREAD_UNCHANGED);	
	if (src.empty())
	{
		cout << "Cannot load images." << endl;
		return -1;
	}

	HazeRemoval hazeRemoval;
	hazeRemoval.filter(src);


	return 0;
}