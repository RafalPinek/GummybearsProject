#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

extern int countColorfulContours(vector<vector<Point>> colorfulContours, Mat image);
extern void countGummybears(Mat transformedImageColor, Mat transformedImageGray, int* result);