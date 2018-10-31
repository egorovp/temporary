/* Computer Vision Functions.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#ifndef __CVLIB_HPP__
#define __CVLIB_HPP__

#include <opencv2/opencv.hpp>

namespace cvlib
{
/// \brief Split and merge algorithm for image segmentation
/// \param image, in - input image
/// \param stddev, in - threshold to treat regions as homogeneous
/// \return segmented image
cv::Mat split_and_merge(const cv::Mat& image, double stddev);
cv::Mat only_split(const cv::Mat& image, double stddev);

/// \brief Segment texuture on passed image according to sample in ROI
/// \param image, in - input image
/// \param roi, in - region with sample texture on passed image
/// \param eps, in - threshold parameter for texture's descriptor distance
/// \return binary mask with selected texture
cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps);

/// \brief Motion Segmentation algorithm
class motion_segmentation : public cv::BackgroundSubtractor
{
public:
	/// \brief ctor
	motion_segmentation();

	/// \see cv::BackgroundSubtractor::apply
	void apply(cv::InputArray image, cv::OutputArray fgmask, double learningRate) override;

	/// \see cv::BackgroundSubtractor::BackgroundSubtractor
	void getBackgroundImage(cv::OutputArray backgroundImage) const override
	{
		backgroundImage.assign(bg_model_);
	}

	void setVarThreshold(double threshold);

private:
	bool init;
	int Threshold;
	double count;
	double accum_size;
	cv::Mat bg_model_f;
	cv::Mat bg_model_;
	cv::Mat accum;
};

/// \brief FAST corner detection algorithm
class corner_detector_fast : public cv::Feature2D
{
public:
	/// \brief Fabrique method for creating FAST detector
	static cv::Ptr<corner_detector_fast> create();

	/// \see Feature2d::detect
	virtual void detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray mask = cv::noArray()) override;
	void setThreshold(int threshold);
	bool check(cv::Point point, int num_compare);
	bool check1(cv::Point point, int num_compare);
	cv::Mat _im;
	int _threshold;
	bool _out;
	std::vector<cv::Point> pos4 = { { 0, -3 },{ 3, 0 },{ 0, 3 },{ -3, 0 }};
	std::vector<cv::Point> pos16 = { { 0, -3 },{ 1, -3 },{ 2, -2 },{ 3, -1 },{ 3, 0 },{ 3, 1 },{ 2, 2 },{ 1, 3 },{ 0, 3 },{ -1, 3 },{ -2, 2 },{ -3, 1 },{ -3, 0 },{ -3, -1 },{ -2, -2 },{ -1, -3 } };
};
} // namespace cvlib
#endif // __CVLIB_HPP__
