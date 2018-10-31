/* Split and merge segmentation algorithm implementation.
* @file
* @date 2018-09-18
* @author Anonymous
*/

#include "cvlib.hpp"

#include <iostream>

namespace cvlib
{
	motion_segmentation::motion_segmentation()
	{
		init = false;
		count = 0;
	}

	void motion_segmentation::apply(cv::InputArray _image, cv::OutputArray _fgmask, double _learningRate)
	{
		accum_size = _learningRate;// round(1 / _learningRate);
		
		
		cv::Mat frame = _image.getMat();

		cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

		cv::Mat frame_f;
		frame.convertTo(frame_f, CV_32FC1);
		float alpha = (float)(1 / accum_size);

		cv::Mat diff;
		cv::Mat diff_u;
		cv::Mat bg_temp;
		//cv::Mat bg_model_f;
		
		if (init == false)
		{
			if (count == 0)
			{
				bg_model_f = cv::Mat::zeros(frame.size(), CV_32FC1);
				bg_model_ = cv::Mat::zeros(frame.size(), CV_8UC1);
			}
			
			count++;
			
			cv::addWeighted(bg_model_f, 1, frame_f, alpha, 0, bg_model_f);
			cv::multiply(bg_model_f, accum_size / count, bg_temp);
			cv::absdiff(frame_f, bg_temp, diff);

			diff.convertTo(diff_u, CV_8UC1);
			cv::threshold(diff_u, diff_u, Threshold, 255, cv::THRESH_BINARY);

			_fgmask.assign(diff_u);

			if (count >= accum_size)
				init = true;

			return;
		}

		cv::addWeighted(frame_f, alpha, bg_model_f, (1 - alpha), 0, bg_model_f);
		bg_model_f.convertTo(bg_model_, CV_8UC1);
		cv::absdiff(frame_f, bg_model_f, diff);

		diff.convertTo(diff_u, CV_8UC1);
		cv::threshold(diff_u, diff_u, Threshold, 255, cv::THRESH_BINARY);

		//cv::Mat structuringElement3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		//cv::erode(diff_u, diff_u, structuringElement3);
		//cv::dilate(diff_u, diff_u, structuringElement3);

		_fgmask.assign(diff_u);
	}

	void motion_segmentation::setVarThreshold(double threshold)
	{
		Threshold = threshold;
	}
} // namespace cvlib