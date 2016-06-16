#pragma once

#include <opencv2/opencv.hpp>
#include <string>

class Tracker
{
public:
	Tracker() {};
	 virtual ~Tracker() {};

    virtual void init(const cv::Mat& image, const cv::Rect& roi) = 0;
	virtual cv::Rect update(const cv::Mat& image) = 0;
};

