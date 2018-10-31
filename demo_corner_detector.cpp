/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <opencv2/opencv.hpp>

#include "utils.hpp"

int demo_corner_detector(int argc, char* argv[])
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

    const auto main_wnd = "frame";
    const auto demo_wnd = "demo";

	int threshold = 15;
    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);
	cv::createTrackbar("th", demo_wnd, &threshold, 255);

    cv::Mat frame;
	auto detector = cvlib::corner_detector_fast::create();// cv::GFTTDetector::create(); // \todo use cvlib::corner_detector_fast
    std::vector<cv::KeyPoint> corners;

    utils::fps_counter fps;
    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;
        cv::imshow(main_wnd, frame);
		detector->setThreshold(threshold);

        detector->detect(frame, corners);
        cv::drawKeypoints(frame, corners, frame, cv::Scalar(0, 0, 255));
        utils::put_fps_text(frame, fps);

		char buffer[25];
		sprintf_s(buffer, "%d", corners.size());
		cv::putText(frame, buffer, cv::Point(15, 15), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 0), 1, CV_AA);
        cv::imshow(demo_wnd, frame);
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
