/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"

std::vector<cv::Scalar> split_graph;
std::vector<cv::Scalar> temp_graph;
std::vector<std::vector<cv::Scalar>> merge_graph;

namespace
{
	bool are_near(std::vector<cv::Scalar> cur, cv::Scalar comp)
	{
		//printf("\ntemp_graph  %d", cur.size());
		for (int i = 0; i < cur.size(); ++i)
		{
			//bool status = false;
			if ((abs(cur[i][0] - comp[2]) < 4 || abs(cur[i][2] - comp[0]) < 4) && (((cur[i][1] >= comp[1]) && (cur[i][1] <= comp[3])) || ((cur[i][1] <= comp[1]) && (cur[i][3] >= comp[1]))))
				return true;
			if ((abs(cur[i][1] - comp[3]) < 4 || abs(cur[i][3] - comp[1]) < 4) && (((cur[i][0] >= comp[0]) && (cur[i][0] <= comp[2])) || ((cur[i][0] <= comp[0]) && (cur[i][2] >= comp[0]))))
				return true;


			if ((cur[i][0] == (comp[2] + 1) || cur[i][2] == (comp[0] + 1)) && (((cur[i][1] >= comp[1]) && (cur[i][1] <= comp[3])) || ((cur[i][1] <= comp[1]) && (cur[i][3] >= comp[1]))))
				return true;
			if ((cur[i][1] == (comp[3] + 1) || cur[i][3] == (comp[1] + 1)) && (((cur[i][0] >= comp[0]) && (cur[i][0] <= comp[2])) || ((cur[i][0] <= comp[0]) && (cur[i][2] >= comp[0]))))
				return true;
		}
		return false;
	}

	bool stddev_parts(cv::Mat cur, cv::Mat comp, double stddev)
	{
		cv::Scalar mean1;
		cv::Scalar mean2;
		cv::Mat dev1;
		cv::Mat dev2;
		bool status = false;
		mean1 = cv::mean(cur);
		mean2 = cv::mean(comp);
		//int mean_all = ((int)mean1[0] * cur.cols * cur.rows + (int)mean2[0] * comp.cols * comp.rows) / (cur.cols * cur.rows * comp.cols * comp.rows);
		//int diff1 = mean_all - (int)mean1[0];
		//int diff2 = mean_all - (int)mean2[0];
		//cv::add(cur, diff1, cur);
		//cv::add(comp, diff2, comp);
		//cv::meanStdDev(cur, mean1, dev1);
		//cv::meanStdDev(comp, mean2, dev2);
		//if (abs(dev1.at<double>(0) - dev2.at<double>(0)) < stddev)//
		if (abs(mean1[0] - mean2[0]) < 10)
			status = true;
		return status;
	}

	void set_to_mean(cv::Mat image)
	{
		int aver_mean = 0;
		int all_mean = 0;
		int aver_size = 0;
		int cur_mean = 0;
		int cur_size = 0;
		cv::Mat cur_im;
		for (int i = 0; i < merge_graph.size(); ++i)
		{
			for (int j = 0; j < merge_graph[i].size(); ++j)
			{
				cur_im = image(cv::Range(merge_graph[i][j][0], merge_graph[i][j][2]), cv::Range(merge_graph[i][j][1], merge_graph[i][j][3]));
				cur_size = (merge_graph[i][j][2] - merge_graph[i][j][0])*(merge_graph[i][j][3] - merge_graph[i][j][1]);
				cur_mean = cv::mean(cur_im)[0];
				aver_mean += cur_mean * std::max(cur_size, 1);
				aver_size += cur_size;
			}
			all_mean = aver_mean / std::max(aver_size, 1);
			aver_mean = 0;
			aver_size = 0;
			for (int j = 0; j < merge_graph[i].size(); ++j)
			{
				//if (merge_graph[i].size() == 1)
				{
					//cur_im.setTo(255);
				}
				//else
				{
					cur_im = image(cv::Range(merge_graph[i][j][0], merge_graph[i][j][2]), cv::Range(merge_graph[i][j][1], merge_graph[i][j][3]));
					cur_im.setTo(all_mean);
				}
			}
		}
	}

	void split_image(cv::Mat image, double stddev, int x0, int y0, int x1, int y1)
	{
	    cv::Scalar mean;
	    cv::Mat dev;
	    cv::meanStdDev(image, mean, dev);
	
		if (dev.at<double>(0) <= stddev)
		{
			split_graph.push_back(cv::Scalar(x0, y0, x1, y1));
			image.setTo(mean[0]);
			return;
		}
		if (image.cols < 4 || image.rows < 4)
		{
			split_graph.push_back(cv::Scalar(x0, y0, x1, y1));
			image.setTo(mean[0]);
			return;
		}

	    const auto width = image.cols;
	    const auto height = image.rows;

	    split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev, x0, y0, (x0 + height/2), (y0 + width/2));
	    split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev, x0, (y0 + width / 2), (x0 + height / 2), y1);
	    split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev, (x0 + height / 2), (y0 + width / 2), x1, y1);
	    split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev, (x0 + height / 2), y0, x1, (y0 + width / 2));
	}
	
	void merge_image(cv::Mat image, double stddev)
	{
		cv::Mat part1;
		cv::Mat part2;
		while (split_graph.size()>1)
		{
			temp_graph.push_back(split_graph[0]);
			part1 = image(cv::Range(split_graph[0][0], split_graph[0][2]), cv::Range(split_graph[0][1], split_graph[0][3]));
			for (int j = 1; j < split_graph.size(); ++j)
			{
				if (split_graph.size() > j)
				{
					if (are_near(temp_graph, split_graph[j]))
					{
						part2 = image(cv::Range(split_graph[j][0], split_graph[j][2]), cv::Range(split_graph[j][1], split_graph[j][3]));
						if (stddev_parts(part1, part2, stddev))
						{
							temp_graph.push_back(split_graph[j]);
							split_graph.erase(split_graph.begin() + j);
							j--;
						}
					}
				}
			}
			merge_graph.push_back(temp_graph);
			temp_graph.clear();
			split_graph.erase(split_graph.begin());
		}
		// для cur сделать рассчет размера и формирование картинки вне цикла + добавить усреднение по соединенным областям через среднее и размер
		//set_to_mean(image);
	}
}

namespace cvlib
{
	cv::Mat split_and_merge(const cv::Mat& image, double stddev)
	{

		temp_graph.clear();
		split_graph.clear();
		merge_graph.clear();
	    // split part
	    cv::Mat res = image;
	    split_image(res, stddev, 0, 0, res.rows, res.cols);
		printf("\nbefore  %d", split_graph.size());
	
		// merge part
		cv::Mat res2 = image;
		merge_image(res2, stddev);
		//printf("\nafter   %d", split_graph.size());
		printf("\n\nmerge   %d\n", merge_graph.size());
		set_to_mean(res);
	    return res;
	}
	cv::Mat only_split(const cv::Mat& image, double stddev)
	{
		// split part
		cv::Mat res = image;
		split_image(res, stddev, 0, 0, res.rows, res.cols);
		//cv::Mat cur_im;
		/*
		for (int i = 0; i < split_graph.size(); ++i)
		{
			cur_im = res(cv::Range(split_graph[i][0], split_graph[i][2]), cv::Range(split_graph[i][1], split_graph[i][3]));
			cur_im.setTo((i % 3) * 127);
		}*/
		return res;
	}
} // namespace cvlib
