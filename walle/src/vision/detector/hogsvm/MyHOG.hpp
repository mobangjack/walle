#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>

//检测窗口(128,64),块尺寸(16,16),块步长(8,8),cell尺寸(8,8),直方图bin个数9
#define MY_HOG_WIN_SIZE        cv::Size(128, 64) //检测窗口大小
#define MY_HOG_WIN_STRIDE      cv::Size(8, 8)    //窗口步进
#define MY_HOG_BLOCK_SIZE      cv::Size(16, 16)  //块大小
#define MY_HOG_BLOCK_STRIDE    cv::Size(8, 8)    //块步进
#define MY_HOG_CELL_SIZE       cv::Size(16, 16)  //细胞大小
#define MY_HOG_N_BINS              9             //梯度方向数
#define MY_HOG_PADDING         cv::Size(0, 0)    //填充
#define MY_HOG_SCALE_F             1.05          //尺度缩放因

#define MY_HOG_DESCRIPTOR_VECTOR_SIZE()  \
(size_t)MY_HOG_N_BINS * \
(MY_HOG_BLOCK_SIZE.width / MY_HOG_CELL_SIZE.width) * \
(MY_HOG_BLOCK_SIZE.height / MY_HOG_CELL_SIZE.height) * \
((MY_HOG_WIN_SIZE.width - MY_HOG_BLOCK_SIZE.width) / MY_HOG_BLOCK_STRIDE.width + 1) * \
((MY_HOG_WIN_SIZE.height - MY_HOG_BLOCK_SIZE.height) / MY_HOG_BLOCK_STRIDE.height + 1)


class  MyHOG : public cv::HOGDescriptor {
public:
	 MyHOG();
	 virtual ~MyHOG();

	 void describe(const cv::Mat& img, std::vector<float>& descriptors);
	 std::vector<float> describe(const cv::Mat& img);
	 void describe(const std::vector<std::string>& imgset, const int& label, cv::Mat& featureMat, cv::Mat& labelMat);
	 void saveDescriptors(const std::vector<float>& descriptors, const std::string& path) const;
	 void loadDescriptors(const std::string& path);
	 void detect(const cv::Mat& img, std::vector<cv::Rect>& found);
	 void filter(const std::vector<cv::Rect>& found, std::vector<cv::Rect>& found_filtered);
};
