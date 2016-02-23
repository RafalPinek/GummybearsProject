#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

extern void findDarkRed(Mat inputFrame, Mat outputFrame);
extern void findBrightRed(Mat inputFrame, Mat outputFrame);
extern void findGreen(Mat inputFrame, Mat outputFrame);
extern void findOrange(Mat inputFrame, Mat outputFrame);
extern void findWhite(Mat inputFrame, Mat outputFrame);
extern void findYellow(Mat inputFrame, Mat outputFrame);