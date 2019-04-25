#include <iostream>
#include <Windows.h>
#include <vector>
#include <opencv2/opencv.hpp>

#define CVUI_IMPLEMENTATION
#include "cvui.h"

using namespace std;
using namespace cv;


#define WINDOW_NAME "Show"

//Determine if it is clockwise
bool clockwise(vector<Point> contour);
//expand function
void expand_polygon(vector<Point> &contour, vector<Point> &contour_exp, float range);

void on_Trackbar(int, void*)
{

}


int main()
{
	Mat show(800, 800, CV_8UC3, Scalar(255,255,255));
	namedWindow(WINDOW_NAME);
	cvui::init(WINDOW_NAME);
	int range=0;
	
	//Polygon setting
	vector<Point> contours;
	contours.push_back(Point(400, 650));
	contours.push_back(Point(450, 600));
	contours.push_back(Point(450, 450));
	contours.push_back(Point(500, 450));
	contours.push_back(Point(500, 400));
	contours.push_back(Point(400, 400));
	
	while (true)
	{
		show= Mat(800, 800, CV_8UC3, Scalar(255, 255, 255));
		//cvui setting
		cvui::window(show, 200, 20, 400, 70, "Setting");
		cvui::trackbar(show, 250, 40, 300, &range, -100, 100);
		cvui::update();
		vector<Point> points_expand;

		//caculate new polygon
		expand_polygon(contours, points_expand, range);

		//show original polygon
		Point vertex;
		for (int i = 0; i < contours.size(); i++) {
			vertex.x = contours[i].x;
			vertex.y = contours[i].y;
			circle(show, vertex, 2, Scalar(0, 255, 0), 1);
			ostringstream indexText;// or std::to_string()
			indexText << i;
			putText(show, indexText.str(), vertex, cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
			int next = (i == (contours.size() - 1) ? 0 : (i + 1));
			line(show, contours[i], contours[next], Scalar(0, 0, 255), 1);
		}

		//show new polygon
		for (int i = 0; i < points_expand.size(); i++) {
			vertex.x = points_expand[i].x;
			vertex.y = points_expand[i].y;
			circle(show, vertex, 2, Scalar(0, 255, 0), 1);
			ostringstream indexText;// or std::to_string()
			indexText << i;
			putText(show, indexText.str(), vertex, cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
			int next = (i == (points_expand.size() - 1) ? 0 : (i + 1));
			line(show, points_expand[i], points_expand[next], Scalar(0, 255, 0), 1);
		}

		imshow(WINDOW_NAME, show);
		waitKey(1);
	}
	
	return 0;
}

bool clockwise(vector<Point> contour)
{
	int x_max = contour[0].x;
	int index = 0;
	for (int i = 1; i < contour.size(); i++) {
		if (contour[i].x > x_max) {
			x_max = contour[i].x;
			index = i;
		}
	}
	Point a, b, c;
	if (index == 0) {
		a = contour.back();
		b = contour[0];
		c = contour[1];
	}
	else if (index == contour.size() - 1)
	{
		a = contour[contour.size() - 2];
		b = contour.back();
		c = contour[0];
	}
	else {
		a = contour[index - 1];
		b = contour[index];
		c = contour[index + 1];
	}

	return ((b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x)) > 0 ? TRUE : FALSE;
}

void expand_polygon(vector<Point> &contour, vector<Point> &contour_exp, float range) {

	// 1. Determine if it is clockwise
	if (clockwise(contour)) {
		range = -range;
	}

	// 2. edge set and normalize it
	vector<Point2f> dpList, ndpList;
	int count = contour.size();
	for (int i = 0; i < count; i++) {
		int next = (i == (count - 1) ? 0 : (i + 1));
		dpList.push_back(contour.at(next) - contour.at(i));
		float unitLen = 1.0f / sqrt(dpList.at(i).dot(dpList.at(i)));
		ndpList.push_back(dpList.at(i) * unitLen);
		cout << "i=" << i << ",pList:" << contour.at(next) << "," << contour.at(i) << ",dpList:" << dpList.at(i) << ",ndpList:" << ndpList.at(i) << endl;
	}

	// 3. compute Line
	for (int i = 0; i < count; i++) {
		int startIndex = (i == 0 ? (count - 1) : (i - 1));
		int endIndex = i;
		float sinTheta = ndpList.at(startIndex).cross(ndpList.at(endIndex));
		Point2f orientVector = ndpList.at(endIndex) - ndpList.at(startIndex);//i.e. PV2-V1P=PV2+PV1
		Point2f temp_out;
		temp_out.x = contour.at(i).x + range / sinTheta * orientVector.x;
		temp_out.y = contour.at(i).y + range / sinTheta * orientVector.y;
		contour_exp.push_back(temp_out);
	}
}
