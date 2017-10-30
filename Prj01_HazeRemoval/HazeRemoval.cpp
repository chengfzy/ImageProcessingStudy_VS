#include "HazeRemoval.h"

using namespace std;
using namespace cv;


HazeRemoval::HazeRemoval()
	: m_nKernelSize(7)
{
}


HazeRemoval::~HazeRemoval()
{
}

void HazeRemoval::filter(cv::Mat & image)
{
	CV_Assert(image.channels() >= 3);

	m_dark.create(image.size(), CV_8UC1);

	const int nChannels = image.channels();

	// get the dark channel
	for (int r = 0; r < image.rows; ++r)
	{
		for (int c = 0; c < image.cols; ++c)
		{
			int r0 = max(0, r - m_nKernelSize);
			int c0 = max(0, c - m_nKernelSize);
			int r1 = min(r + m_nKernelSize, image.rows);
			int c1 = min(c + m_nKernelSize, image.cols);
			// find min pixel, compare to R,G,B channel
			uchar darkPix{ 255 };
			for (int i = r0; i < r1; ++i)
			{
				const uchar* pSrc = image.ptr<uchar>(i);
				for (int j = c0; j < c1; ++j)
				{
					for (int k = 0; k < 3; ++k)
					{
						const uchar& pix = pSrc[j * nChannels + k];
						darkPix = pix < darkPix ? pix : darkPix;
					}
				}
			}
			m_dark.at<uchar>(r, c) = darkPix;
		}
	}



}

cv::Mat HazeRemoval::getDarkChannel() const
{
	return m_dark;
}
