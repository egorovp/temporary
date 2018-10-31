/* Demo application for Computer Vision Library.
* @file
* @date 2018-09-05
* @author Anonymous
*/

#include "cvlib.hpp"
#include <opencv2/opencv.hpp>

int demo_motion_segmentation(int argc, char* argv[])
{
	cv::VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;

	auto mseg = new cvlib::motion_segmentation;//cv::createBackgroundSubtractorMOG2(); // \todo use cvlib::motion_segmentation
	const auto main_wnd = "frame";
	const auto demo_wnd = "demo";

	int threshold = 15;
	double learningR;
	int learn = 30;
	cv::namedWindow(main_wnd);
	cv::namedWindow(demo_wnd);
	cv::createTrackbar("thresh", demo_wnd, &threshold, 255);
	cv::createTrackbar("AccumSize", demo_wnd, &learn, 100);

	clock_t start = 0, stop = 10;
	cv::Mat frame;
	cv::Mat frame_gray;
	cv::Mat frame_mseg;
	cv::Mat bg;
	while (cv::waitKey(30) != 27) // ESC
	{
		//if (double((stop - start) / 100.0) > 2)
		{
			//start = clock();
			cap >> frame;
			cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
			cv::imshow(main_wnd, frame_gray);

			mseg->setVarThreshold(threshold); // \todo use TackbarCallback
			learningR = (double)learn;
			mseg->apply(frame, frame_mseg, (learningR));
			mseg->getBackgroundImage(bg);
			if (!frame_mseg.empty())
				cv::imshow(demo_wnd, frame_mseg);
			if (!bg.empty())
				cv::imshow("BG", bg);
			//stop = clock();
		}
		//else
		{
			//stop = clock();
		}
		
	}

	cv::destroyWindow(main_wnd);
	cv::destroyWindow(demo_wnd);
	cv::destroyWindow("BG");

	delete mseg;

	return 0;
}