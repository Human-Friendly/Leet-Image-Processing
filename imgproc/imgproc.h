// # 1337Coding # Martin Angelov 06.14.2020 # ----> Leet Image Processing imgproc.h

#pragma once
#include <imgproc/buffer.h>

// ------------------------------------------------------------------------------------------------
//
//

class Point2i
{
public:
	Point2i() { setX(0), setY(0); }
	Point2i(int x, int y);

	void setX(int x) { this->x = x; }
	void setY(int y) { this->y = y; }

	int getX() { return x; }
	int getY() { return y; }

private:

	int x,
		y;
};

//
// ------------------------------------------------------------------------------------------------
//

class Point3f
{
public:

	Point3f() { x = y = z = 0; }
	Point3f(float x, float y, float z);

	void setX(float x) { this->x = x; }
	void setY(float y) { this->y = y; }
	void setZ(float z) { this->z = z; }

	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }

	void operator += (const Point3f t);
	void operator -= (const Point3f t);
	void operator *= (const Point3f t);
	Point3f operator / (const float t);
	Point3f operator + (const Point3f t);
	Point3f operator - (const Point3f t);

private:

	float x,
		y,
		z;
};

//
// ------------------------------------------------------------------------------------------------
//

class ImageProcessing
{
public:
	ImageProcessing();

	void warpPerspective(Point3f pos, Point3f angles, ColorBuffer inputBuffer, ColorBuffer& outputBuffer);

private:

	void rotatePoint(Point3f& point, Point3f angles, Point3f center);
	Point2i calcProjection(Point3f point, Point3f surface, Point3f center);
	void drawQuadrangle(Point2i a, Point2i b, Point2i c, Point2i d, ColorBuffer inputBuffer, ColorBuffer& outputBuffer);
	std::vector<Point2i> line(Point2i a, Point2i b, DrawingContext *context);
	std::vector<Point2i> calculateDelta(Point2i a, Point2i b);
	void putPixel(int x, int y, Color color, ColorBuffer& outputTexture);
	Color getColorAt(float a, float b, ColorBuffer *inputTexture);

private:

	const int INDEX_DELTA_A = 0,
           INDEX_DELTA_B = 1,
           INDEX_DELTA_SIZE = 2;

	const float offset = 1,
		halfOffset = 0.5f,
		halfValue = 2,
		Deg2Rad = 0.0174532925f;

};
