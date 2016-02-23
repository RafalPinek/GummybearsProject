#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

extern bool areRectangularMarkers(vector<vector<Point>> resultContours);
extern bool compareContoursBySize(vector<Point> contourOne, vector<Point> contourTwo);
extern void rejectPixelsCloseToEdge(Mat& outputFrame);
extern vector<vector<Point>> getFourHighestAreaAndRejectTooBig(vector<vector<Point>>& contours);
extern vector<vector<Point>> rejectTooOblong(vector<vector<Point>> contours);
extern void rejectOtherShape(vector<vector<Point>>& resultContours, vector<RotatedRect> rectangles, vector<vector<Point>> oldContours);
extern vector<vector<Point>> findMarkers(Mat imageColor, Mat imageGray);