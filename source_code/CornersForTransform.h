#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

extern vector<Point> findCornersNonRectangularShape(vector<vector<Point>> contours);
extern vector<Point> findCornersRectangularShape(vector<vector<Point>> contours, Mat imageColor);

extern bool compareContoursByPositionX(vector<Point> contourOne, vector<Point> contourTwo);
extern bool compareContoursByPositionY(vector<Point> contourOne, vector<Point> contourTwo);
extern bool comparePointsByPositionY(Point pointOne, Point pointTwo);
extern bool comparePointsByPositionX(Point pointOne, Point pointTwo);

extern Point calculatePointByContour(vector<Point> contour);
extern void calculateLineCoefficients(float* a, float* b, vector<vector<Point>> contours);
extern void calculatePointsOfIntersection(vector<vector<Point>> contours, int* x, int* y, float* a, float* b, Mat image);
extern vector<int> calculateWidthAndHeight(vector<Point> points);