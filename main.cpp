// Created by Kushal Kanodia on Oct 1, 2018

#include <iostream>
#include <cmath>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <list>
using namespace std;
using namespace std::chrono;
#define S 700

class Point {
    private:
        int x;
        int y;
        double angle = 0;
        static bool sortFunc(Point i, Point j) {
            return i.angle < j.angle;
        }
    public:
        Point() { x = 0; y = 0; }
        Point(int myX, int myY) { x = myX; y = myY; }
        int getX() { return x; }
        int getY() { return y; }
        void setX(int newX) { x = newX; }
        void setY(int newY) { y = newY; }
        double getAngle() { return angle; }
        void setAngle(double newAngle) { angle = newAngle; }
        static list<Point> getSorted(list<Point> pointList) {
            pointList.sort(sortFunc);
            return pointList;
        }
};


void setPixel(int image[S][S], int x, int y) {
    if (x >= 0 && x < S && y >= 0 && y < S)
        image[x][y] = 0;
}

void setPoint(int image[S][S], int x, int y) {
    for (int j = x-1; j <= x+1; j++) {
        for (int k = y-1; k <= y+1; k++) {
            setPixel(image, j, k);
        }
    }
}

void drawLine(int image[S][S], int x1, int y1, int x2, int y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double xt = x1;
    double yt = y1;

    setPixel(image, x1, y1);
    if (std::abs(dx) >= std::abs(dy)) {
        dy /= std::abs(dx);
        dx /= std::abs(dx);
    }
    else {
        dx /= std::abs(dy);
        dy /= std::abs(dy);
    }
    while (std::abs(round(xt) - x2) > 1.1 || std::abs(round(yt) - y2) > 1.1) {
        xt += dx;
        yt += dy;
        setPixel(image, round(xt), round(yt));
    }
}

int orientation(Point a, Point b, Point c) {
    int orient = (b.getY()-a.getY()) * (c.getX()-b.getX()) - (b.getX()-a.getX()) * (c.getY()-b.getY());
    if (orient > 0)
        return -1;
    else if (orient < 0)
        return 1;
    return 0;
}

Point underTop(list<Point>* stack) {
    Point top = stack->front();
    stack->pop_front();
    Point newTop = stack->front();
    stack->push_front(top);
    return newTop;
}

double grahamScan(int n) {
    int img[S][S];

    for (auto & x : img) {
        for (int & y : x) {
            y = 1;
        }
    }

    auto start = high_resolution_clock::now();

    srand(time(nullptr));
    auto* ptArr = new list<Point>;
    Point pt = Point(rand() % (S), rand() % (S));
    ptArr->push_front(pt);
    Point p = Point(pt.getX(), pt.getY());
    setPoint(img, pt.getX(), pt.getY());
    int startPt = -1;

    for (int i = 1; i < n; i++) {
        Point np = Point(rand() % (S), rand() % (S));
        if (np.getY() < p.getY() || (np.getY() == p.getY() && np.getX() < p.getX())) {
            p.setX(np.getX());
            p.setY(np.getY());
            startPt = -1*(i+1);
            np.setAngle(startPt);
        }
        ptArr->push_front(np);
        setPoint(img, np.getX(), np.getY());
    }

    for (auto& i : *ptArr) {
        if (i.getAngle() != startPt)
            i.setAngle(atan2(i.getY() - p.getY(), i.getX() - p.getX()));
    }

    auto* ptStack = new list<Point>;
    int count = 0;

    for (auto& i : Point::getSorted(*ptArr)) {
        if (count < 3)
            ptStack->push_front(i);
        else {
            while (orientation(underTop(ptStack), ptStack->front(), i) != 1)
                ptStack->pop_front();
            ptStack->push_front(i);
        }
        count++;
    }

    auto stop = high_resolution_clock::now();

    ptStack->push_front(p);
    int px = 0;
    int py = 0;

    while (!ptStack->empty()) {
        px = ptStack->front().getX();
        py = ptStack->front().getY();
        ptStack->pop_front();
        if (!ptStack->empty())
            drawLine(img, px, py, ptStack->front().getX(), ptStack->front().getY());
    }

	ofstream outputFile;
	outputFile.open("gsHull.ppm");
	outputFile << "P3  " << S << "  " << S << "  1\n";
	for (int y = 0; y < S; y++) {
		for (auto & x : img) {
			outputFile << x[y] << " " << x[y] << " " << x[y] << " ";
		}
		outputFile << "\n";
	}
	outputFile.close();
    auto duration = duration_cast<milliseconds>(stop - start);
    return duration.count() / 1000.0;
}

int main() {
    int pts;
    cout << "Enter the number of points to be generated: ";
    cin >> pts;
    cout << "Generated " << pts << " points and found convex hull in ~" << grahamScan(pts) << " seconds.";
    return 0;
}

// Potential Future Additions:
//  - User can select width and height of image
//  - Increase efficiency of Graham Scan
//  - Output to other file formats (png, jpg)
//  - Add color to convex hull to increase its visibility
//  - Increase randomness of points, resolve warnings
