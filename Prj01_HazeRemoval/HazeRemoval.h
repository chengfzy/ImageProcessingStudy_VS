#pragma once
#include "opencv2/core.hpp"

class HazeRemoval
{
public:
	HazeRemoval();
	~HazeRemoval();

public:
	void filter(cv::Mat& image);

	// get dark channel
	cv::Mat getDarkChannel() const;

private:
	int m_nKernelSize;

	cv::Mat m_dark;		// dark channel
};