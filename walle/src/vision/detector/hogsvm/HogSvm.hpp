#pragma once

#include "../detector.hpp"
#include "MyHOG.hpp"
#include "MySVM.hpp"

class  HogSvm : public Detector {
public:
	 HogSvm();
	 virtual ~HogSvm();

	 virtual std::vector<cv::Rect> detect(const cv::Mat &frame);

	 void pre_train();
	 void train();
	 std::vector<std::string> findHardSamples();
	 void post_train();
	 
protected:
	MyHOG hog;
	MySVM svm;
};
