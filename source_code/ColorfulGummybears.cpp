#include "ColorfulGummybear.h"

void findDarkRed(Mat inputFrame, Mat outputFrame) {
	int blueMin = 5; int blueMax = 40;
	int greenMin = 5; int greenMax = 20;
	int redMin = 80; int redMax = 118;
	inRange(inputFrame, Scalar(blueMin, greenMin, redMin), Scalar(blueMax, greenMax, redMax), outputFrame);
}

void findBrightRed(Mat inputFrame, Mat outputFrame) {
	int blueMin = 15; int blueMax = 50;
	int greenMin = 15; int greenMax = 50;
	int redMin = 130; int redMax = 165;
	inRange(inputFrame, Scalar(blueMin, greenMin, redMin), Scalar(blueMax, greenMax, redMax), outputFrame);
}

void findGreen(Mat inputFrame, Mat outputFrame) {
	int blueMin = 20; int blueMax = 70;
	int greenMin = 60; int greenMax = 100;
	int redMin = 55; int redMax = 110;
	inRange(inputFrame, Scalar(blueMin, greenMin, redMin), Scalar(blueMax, greenMax, redMax), outputFrame);
}

void findOrange(Mat inputFrame, Mat outputFrame) {
	int blueMin = 10; int blueMax = 45;
	int greenMin = 55; int greenMax = 90;
	int redMin = 150; int redMax = 208;
	inRange(inputFrame, Scalar(blueMin, greenMin, redMin), Scalar(blueMax, greenMax, redMax), outputFrame);
}

void findWhite(Mat inputFrame, Mat outputFrame) {
	//Mat inputFrameClone;
	int blueMin = 100; int blueMax = 120;
	int greenMin = 125; int greenMax = 165;
	int redMin = 155; int redMax = 200;
	inRange(inputFrame, Scalar(blueMin, greenMin, redMin), Scalar(blueMax, greenMax, redMax), outputFrame);
}

void findYellow(Mat inputFrame, Mat outputFrame) {
	int blueMin = 16; int blueMax = 70;
	int greenMin = 90; int greenMax = 145;
	int redMin = 140; int redMax = 200;
	inRange(inputFrame, Scalar(blueMin, greenMin, redMin), Scalar(blueMax, greenMax, redMax), outputFrame);
}