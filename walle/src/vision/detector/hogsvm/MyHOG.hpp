#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>

//��ⴰ��(128,64),��ߴ�(16,16),�鲽��(8,8),cell�ߴ�(8,8),ֱ��ͼbin����9
#define MY_HOG_WIN_SIZE        cv::Size(128, 64) //��ⴰ�ڴ�С
#define MY_HOG_WIN_STRIDE      cv::Size(8, 8)    //���ڲ���
#define MY_HOG_BLOCK_SIZE      cv::Size(16, 16)  //���С
#define MY_HOG_BLOCK_STRIDE    cv::Size(8, 8)    //�鲽��
#define MY_HOG_CELL_SIZE       cv::Size(16, 16)  //ϸ����С
#define MY_HOG_N_BINS              9             //�ݶȷ�����
#define MY_HOG_PADDING         cv::Size(0, 0)    //���
#define MY_HOG_SCALE_F             1.05          //�߶�������

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
