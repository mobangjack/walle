#include "walle.hpp"

Walle::Walle(const char* port, double fireOnAccuracy)
{
	this->robot.setPort(port);
	this->fireOnAccuracy = fireOnAccuracy;
}

cv::Rect Walle::guide(cv::Mat& frame)
{
	vision.capture(frame);
	if (vision.spot()) {
		//if aimed, fire, else keep spotting
		if (vision.ds < fireOnAccuracy) {
			robot.cmd(vision.dx, vision.dy, vision.dz, 0xff);
		} else {
			robot.cmd(vision.dx, vision.dy, vision.dz, 0x00);
		}
	}
	// initiatively searching
	else {
		robot.cmd(vision.dx, vision.dy, vision.dz, 0x00);
	}
	return vision.target;
}