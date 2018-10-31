/* FAST corner detector algorithm implementation.
 * @file
 * @date 2018-10-16
 * @author Anonymous
 */

#include "cvlib.hpp"
void nothing()
{
};
namespace cvlib
{
// static
cv::Ptr<corner_detector_fast> corner_detector_fast::create()
{
    return cv::makePtr<corner_detector_fast>();
}

void corner_detector_fast::setThreshold(int threshold)
{
	_threshold = threshold;
}

bool corner_detector_fast::check(cv::Point point, int num_compare)
{
	int counter = 0;
	_out = false;
	int ref_counter = num_compare == 6 ? 3 : 12;
	std::vector<cv::Point> pos;
	pos = num_compare == 6 ? pos4 : pos16;
	for (int i = 0; i < num_compare; ++i)
		if ((_im.at<uchar>(point + pos[i % pos.size()]) - _im.at<uchar>(point)) > _threshold)
		{
			counter++;
			_out = counter > ref_counter ? true : false;
			if (_out)
				break;
		}
		else
		{
			counter = 0;
		}
	return _out;
}

bool corner_detector_fast::check1(cv::Point point, int num_compare)
{
	int counter = 0;
	_out = false;
	int ref_counter = num_compare == 6 ? 3 : 12;
	std::vector<cv::Point> pos;
	pos = num_compare == 6 ? pos4 : pos16;
	for (int i = 0; i < num_compare; ++i)
		if ((_im.at<uchar>(point) - _im.at<uchar>(point + pos[i % pos.size()])) > _threshold)
		{
			counter++;
			_out = counter > ref_counter ? true : false;
			if (_out)
				break;
		}
		else
		{
			counter = 0;
		}
	return _out;
}

void corner_detector_fast::detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray /*mask = cv::noArray()*/)
{
    keypoints.clear();
	image.type() == CV_8UC1 ? image.copyTo(_im) : cv::cvtColor(image, _im, cv::COLOR_BGR2GRAY);
	for (int i = 3; i < _im.cols - 3; ++i)
		for (int j = 3; j < _im.rows - 3; ++j)
		{
			check(cv::Point(i, j), 6) ? (check(cv::Point(i, j), 27) ? keypoints.push_back(cv::KeyPoint((float)i, (float)j, 1.0f)) : nothing()) : nothing();
			if (!_out)
				check1(cv::Point(i, j), 6) ? (check1(cv::Point(i, j), 27) ? keypoints.push_back(cv::KeyPoint((float)i, (float)j, 1.0f)) : nothing()) : nothing();
		}
}
} // namespace cvlib
