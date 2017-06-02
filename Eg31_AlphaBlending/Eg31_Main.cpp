#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;


//REF: https://stackoverflow.com/questions/22315904/blending-does-not-remove-seams-in-opencv


cv::Mat border(cv::Mat mask)
{
	cv::Mat gx;
	cv::Mat gy;

	cv::Sobel(mask, gx, CV_32F, 1, 0, 3);
	cv::Sobel(mask, gy, CV_32F, 0, 1, 3);

	cv::Mat border;
	cv::magnitude(gx, gy, border);

	return border > 100;
}

cv::Mat computeAlphaBlending(cv::Mat image1, cv::Mat mask1, cv::Mat image2, cv::Mat mask2)
{
	// edited: find regions where no mask is set
	// compute the region where no mask is set at all, to use those color values unblended
	cv::Mat bothMasks = mask1 | mask2;
	cv::imshow("maskOR", bothMasks);
	cv::Mat noMask = 255 - bothMasks;

	// create an image with equal alpha values:
	cv::Mat rawAlpha = cv::Mat(noMask.rows, noMask.cols, CV_32FC1);
	rawAlpha = 1.0f;

	// invert the border, so that border values are 0 ... this is needed for the distance transform
	cv::Mat border1 = 255 - border(mask1);
	cv::Mat border2 = 255 - border(mask2);

	// show the immediate results for debugging and verification, should be an image where the border of the face is black, rest is white
	cv::imshow("b1", border1);
	cv::imshow("b2", border2);

	// compute the distance to the object center
	cv::Mat dist1;
	cv::distanceTransform(border1, dist1, CV_DIST_L2, 3);

	// scale distances to values between 0 and 1
	double min, max; cv::Point minLoc, maxLoc;

	// find min/max vals
	cv::minMaxLoc(dist1, &min, &max, &minLoc, &maxLoc, mask1&(dist1>0));  // edited: find min values > 0
	dist1 = dist1* 1.0 / max; // values between 0 and 1 since min val should alwaysbe 0

	// same for the 2nd image
	cv::Mat dist2;
	cv::distanceTransform(border2, dist2, CV_DIST_L2, 3);
	cv::minMaxLoc(dist2, &min, &max, &minLoc, &maxLoc, mask2&(dist2>0));  // edited: find min values > 0
	dist2 = dist2*1.0 / max;  // values between 0 and 1


	//TODO: now, the exact border has value 0 too... to fix that, enter very small values wherever border pixel is set...
	// mask the distance values to reduce information to masked regions
	cv::Mat dist1Masked;
	rawAlpha.copyTo(dist1Masked, noMask);    // edited: where no mask is set, blend with equal values
	dist1.copyTo(dist1Masked, mask1);
	rawAlpha.copyTo(dist1Masked, mask1&(255 - mask2)); //edited

	cv::Mat dist2Masked;
	rawAlpha.copyTo(dist2Masked, noMask);    // edited: where no mask is set, blend with equal values
	dist2.copyTo(dist2Masked, mask2);
	rawAlpha.copyTo(dist2Masked, mask2&(255 - mask1)); //edited

	cv::imshow("d1", dist1Masked);
	cv::imshow("d2", dist2Masked);

	// dist1Masked and dist2Masked now hold the "quality" of the pixel of the image, so the higher the value, the more of that pixels information should be kept after blending
	// problem: these quality weights don't build a linear combination yet

	// you want a linear combination of both image's pixel values, so at the end you have to divide by the sum of both weights
	cv::Mat blendMaskSum = dist1Masked + dist2Masked;
	//cv::imshow("blendmask==0",(blendMaskSum==0));

	// you have to convert the images to float to multiply with the weight
	cv::Mat im1Float;
	image1.convertTo(im1Float, dist1Masked.type());
	cv::imshow("im1Float", im1Float / 255.0);

	// TODO: you could replace those splitting and merging if you just duplicate the channel of dist1Masked and dist2Masked
	// the splitting is just used here to use .mul later... which needs same number of channels
	std::vector<cv::Mat> channels1;
	cv::split(im1Float, channels1);
	// multiply pixel value with the quality weights for image 1
	cv::Mat im1AlphaB = dist1Masked.mul(channels1[0]);
	cv::Mat im1AlphaG = dist1Masked.mul(channels1[1]);
	cv::Mat im1AlphaR = dist1Masked.mul(channels1[2]);

	std::vector<cv::Mat> alpha1;
	alpha1.push_back(im1AlphaB);
	alpha1.push_back(im1AlphaG);
	alpha1.push_back(im1AlphaR);
	cv::Mat im1Alpha;
	cv::merge(alpha1, im1Alpha);
	cv::imshow("alpha1", im1Alpha / 255.0);

	cv::Mat im2Float;
	image2.convertTo(im2Float, dist2Masked.type());
	cv::imshow("im2Float", im2Float / 255.0);

	std::vector<cv::Mat> channels2;
	cv::split(im2Float, channels2);
	// multiply pixel value with the quality weights for image 2
	cv::Mat im2AlphaB = dist2Masked.mul(channels2[0]);
	cv::Mat im2AlphaG = dist2Masked.mul(channels2[1]);
	cv::Mat im2AlphaR = dist2Masked.mul(channels2[2]);

	std::vector<cv::Mat> alpha2;
	alpha2.push_back(im2AlphaB);
	alpha2.push_back(im2AlphaG);
	alpha2.push_back(im2AlphaR);
	cv::Mat im2Alpha;
	cv::merge(alpha2, im2Alpha);
	cv::imshow("alpha2", im2Alpha / 255.0);

	// now sum both weighted images and divide by the sum of the weights (linear combination)
	cv::Mat imBlendedB = (im1AlphaB + im2AlphaB) / blendMaskSum;
	cv::Mat imBlendedG = (im1AlphaG + im2AlphaG) / blendMaskSum;
	cv::Mat imBlendedR = (im1AlphaR + im2AlphaR) / blendMaskSum;
	std::vector<cv::Mat> channelsBlended;
	channelsBlended.push_back(imBlendedB);
	channelsBlended.push_back(imBlendedG);
	channelsBlended.push_back(imBlendedR);

	// merge back to 3 channel image
	cv::Mat merged;
	cv::merge(channelsBlended, merged);

	// convert to 8UC3
	cv::Mat merged8U;
	merged.convertTo(merged8U, CV_8UC3);

	return merged8U;
}

int main()
{

	cv::Mat i1 = cv::imread("../pic/face1.png");
	cv::Mat i2 = cv::imread("../pic/face2.png");

	cv::Mat m1 = cv::imread("../pic/face1.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat m2 = cv::imread("../pic/face2.png", CV_LOAD_IMAGE_GRAYSCALE);

	// works too, for background near white
	//  m1 = m1 < 220;
	//  m2 = m2 < 220;

	// edited:  using OTSU thresholding. If not working you have to create your own masks with a better technique
	cv::threshold(m1, m1, 255, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
	cv::threshold(m2, m2, 255, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

	cv::Mat out = computeAlphaBlending(i1, m1, i2, m2);

	cv::waitKey(-1);
	return 0;
}