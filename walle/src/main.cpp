#include "walle.hpp"

int main(int argc, char **argv)
{
	Walle walle("ttyHS0", 10);
	cv::VideoCapture cap(0);
	if (!cap.isOpened()) {
		std::cout << "fail to open camera" << std::endl;
		return -1;
	}
	cv::Mat frame;
	while (1) {
		cap >> frame;
		walle.guide(frame);
	}
	return 0;
}
