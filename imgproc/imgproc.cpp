// # 1337Coding # Martin Angelov 06.14.2020 # ----> Leet Image Processing imgproc.cpp

#include <imgproc/imgproc.h>
#include <algorithm>
#include <iostream>

// ------------------------------------------------------------------------------------------------
//
//

ImageProcessing::ImageProcessing() { }

// ------------------------------------------------------------------------------------------------

void ImageProcessing::warpPerspective(Point3f pos, Point3f angles, ColorBuffer inputBuffer, ColorBuffer& outputBuffer)
{
    int scale = std::max(inputBuffer.getWidth(), inputBuffer.getHeight());

    outputBuffer = ColorBuffer(scale, scale);

    float scaleX = inputBuffer.getWidth() == scale ? offset : (float)inputBuffer.getWidth() / inputBuffer.getHeight();
    float scaleY = inputBuffer.getHeight() == scale ? offset : (float)inputBuffer.getHeight() / inputBuffer.getWidth();

    if (pos.getZ() < -halfOffset)
        pos.setZ(-halfOffset);

    Point3f size(outputBuffer.getWidth(), outputBuffer.getHeight(), offset);
    Point3f center = size / halfValue;

    pos += Point3f(offset, offset, halfValue);

    Point3f a = Point3f(-halfOffset * scaleX, halfOffset * scaleY, 0) + pos,
        b = Point3f(halfOffset * scaleX, halfOffset * scaleY, 0) + pos,
        c = Point3f(-halfOffset * scaleX, -halfOffset * scaleY, 0) + pos,
        d = Point3f(halfOffset * scaleX, -halfOffset * scaleY, 0) + pos;

    Point3f surface(0, 0, offset);

    rotatePoint(a, angles, pos);
    rotatePoint(b, angles, pos);
    rotatePoint(c, angles, pos);
    rotatePoint(d, angles, pos);

    drawQuadrangle(calcProjection(a, surface, center), calcProjection(b, surface, center), 
        calcProjection(c, surface, center), calcProjection(d, surface, center), inputBuffer, outputBuffer);
}

// ------------------------------------------------------------------------------------------------

void ImageProcessing::rotatePoint(Point3f& point, Point3f angles, Point3f center)
{
    float pitch = angles.getY() * Deg2Rad,
        roll = angles.getX() * Deg2Rad,
        yaw = -angles.getZ() * Deg2Rad;

    point -= center;

    float cosa = cos(yaw);
    float sina = sin(yaw);

    float cosb = cos(pitch);
    float sinb = sin(pitch);

    float cosc = cos(roll);
    float sinc = sin(roll);

    float Axx = cosa * cosb;
    float Axy = cosa * sinb * sinc - sina * cosc;
    float Axz = cosa * sinb * cosc + sina * sinc;

    float Ayx = sina * cosb;
    float Ayy = sina * sinb * sinc + cosa * cosc;
    float Ayz = sina * sinb * cosc - cosa * sinc;

    float Azx = -sinb;
    float Azy = cosb * sinc;
    float Azz = cosb * cosc;

    float px = point.getX();
    float py = point.getY();
    float pz = point.getZ();

    point.setX(Axx * px + Axy * py + Axz * pz);
    point.setY(Ayx * px + Ayy * py + Ayz * pz);
    point.setZ(Azx * px + Azy * py + Azz * pz);

    point += center;
}

// ------------------------------------------------------------------------------------------------

Point2i ImageProcessing::calcProjection(Point3f point, Point3f surface, Point3f center)
{
    point *= center;

    Point3f p = point - center;

    float sx = 0;
    float cx = offset;

    float sy = 0;
    float cy = offset;

    float sz = 0;
    float cz = offset;

    float dx = cy * (sz * p.getY() + cz * p.getX()) - sy * p.getZ();
    float dy = sx * (cy * p.getZ() + sy * (sz * p.getY() + cz * p.getX())) + cx * (cz * p.getY() - sz * p.getX());
    float dz = cx * (cy * p.getZ() + sy * (sz * p.getY() + cz * p.getX())) - sx * (cz * p.getY() - sz * p.getX());

    float bx = surface.getZ() / dz * dx + surface.getX() + center.getX();
    float by = surface.getZ() / dz * dy + surface.getY() + center.getY();

    return Point2i(round(bx), round(by));
}

// ------------------------------------------------------------------------------------------------

void ImageProcessing::drawQuadrangle(Point2i a, Point2i b, Point2i c, Point2i d, ColorBuffer inputBuffer, ColorBuffer& outputBuffer)
{
    std::vector<Point2i> top = line(a, b, nullptr);
    std::vector<Point2i> bottom = line(c, d, nullptr);

    bool reversed = true;

    if (bottom.size() > top.size())
    {
        std::vector<Point2i> temp = bottom;
        bottom = top;
        top = temp;
        reversed = false;
    }

    DrawingContext* context = &DrawingContext(&inputBuffer, &outputBuffer, reversed);

    for (int i = 0; i < top.size(); i++)
    {
        float value = (float)i / top.size();
        int j = roundf(value * (bottom.size() - offset));

        context->setValue(value);

        line(top[i], bottom[j], context);
    }
}

// ------------------------------------------------------------------------------------------------

std::vector<Point2i> ImageProcessing::line(Point2i a, Point2i b, DrawingContext *context)
{
    std::vector<Point2i> delta = calculateDelta(a, b);

    Point2i deltaA = delta[INDEX_DELTA_A];
    Point2i deltaB = delta[INDEX_DELTA_B];

    int longest = delta[INDEX_DELTA_SIZE].getX();
    int shortest = delta[INDEX_DELTA_SIZE].getY();

    int numerator = longest >> (int)offset;

    std::vector<Point2i> line;

    for (int i = 0; i <= longest; i++)
    {
        if (context)
        {
            int index = context->isReversed() ? longest - i : i;
            float y = (float)index / longest;

            Color color = getColorAt(context->getValue(), y, context->getInputTexture());

            putPixel(a.getX(), a.getY(), color, *context->getOutputTexture());
            putPixel(a.getX(), a.getY() + offset, color, *context->getOutputTexture());
        }

        numerator += shortest;

        if (!(numerator < longest))
        {
            numerator -= longest;
            a.setX(a.getX() + deltaA.getX());
            a.setY(a.getY() + deltaA.getY());
        }
        else
        {
            a.setX(a.getX() + deltaB.getX());
            a.setY(a.getY() + deltaB.getY());
        }

        line.push_back(Point2i(a.getX(), a.getY()));
    }

    return line;
}

// ------------------------------------------------------------------------------------------------

std::vector<Point2i> ImageProcessing::calculateDelta(Point2i a, Point2i b)
{
    int width = b.getX() - a.getX();
    int height = b.getY() - a.getY();

    Point2i deltaA,
        deltaB,
        size;

    if (width < 0)
    {
        deltaA.setX(-offset); 
        deltaB.setX(-offset);
    }
    else if (width > 0)
    {
        deltaA.setX(offset);
        deltaB.setX(offset);
    }

    if (height < 0)
        deltaA.setY(-offset);
    else if (height > 0)
        deltaA.setY(offset);

    int longest = abs(width);
    int shortest = abs(height);

    if (!(longest > shortest))
    {
        longest = abs(height);
        shortest = abs(width);

        if (height < 0)
            deltaB.setY(-offset);
        else if (height > 0)
            deltaB.setY(offset);

        deltaB.setX(0);
    }

    size.setX(longest);
    size.setY(shortest);

    std::vector<Point2i> points;

    points.push_back(deltaA);
    points.push_back(deltaB);
    points.push_back(size);

    return points;
}

// ------------------------------------------------------------------------------------------------

void ImageProcessing::putPixel(int x, int y, Color color, ColorBuffer& outputTexture)
{
    int index = (y * outputTexture.getWidth()) + x;

    if (!(x < 0) && x < outputTexture.getWidth() && !(y < 0) && y < outputTexture.getHeight())
        outputTexture.setDataValue(color, index);
}

// ------------------------------------------------------------------------------------------------

Color ImageProcessing::getColorAt(float a, float b, ColorBuffer *inputTexture)
{
    int x = roundf(a * (inputTexture->getWidth() - offset));
    int y = roundf(b * (inputTexture->getHeight() - offset));

    int index = (y * inputTexture->getWidth()) + x;

    return inputTexture->getDataValue(index);
}

// ------------------------------------------------------------------------------------------------

Point2i::Point2i(int x, int y)
{
    setX(x);
    setY(y);
}

// ------------------------------------------------------------------------------------------------

Point3f::Point3f(float x, float y, float z)
{
    setX(x);
    setY(y);
    setZ(z);
}

// ------------------------------------------------------------------------------------------------

void Point3f::operator+=(const Point3f t)
{
    this->x += t.x;
    this->y += t.y;
    this->z += t.z;
}

// ------------------------------------------------------------------------------------------------

void Point3f::operator-=(const Point3f t)
{
    this->x -= t.x;
    this->y -= t.y;
    this->z -= t.z;
}

// ------------------------------------------------------------------------------------------------

void Point3f::operator*=(const Point3f t)
{
    this->x *= t.x;
    this->y *= t.y;
    this->z *= t.z;
}

// ------------------------------------------------------------------------------------------------

Point3f Point3f::operator/(const float t)
{
    this->x /= t;
    this->y /= t;
    this->z /= t;
    return *this;
}

// ------------------------------------------------------------------------------------------------

Point3f Point3f::operator+(const Point3f t)
{
    this->x += t.x;
    this->y += t.y;
    this->z += t.z;
    return *this;
}

// ------------------------------------------------------------------------------------------------

Point3f Point3f::operator-(const Point3f t)
{
    this->x -= t.x;
    this->y -= t.y;
    this->z -= t.z;
    return *this;
}

// ------------------------------------------------------------------------------------------------

Color::Color(unsigned char red, unsigned char green, unsigned char blue)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
}

// ------------------------------------------------------------------------------------------------

ColorBuffer::ColorBuffer(int width, int height)
{
    setWidth(width);
    setHeight(height);
    data.resize(width * height);
}

// ------------------------------------------------------------------------------------------------

ColorBuffer::ColorBuffer(std::vector<Color> data, int width, int height)
{
    this->data = data;
    setWidth(width);
    setHeight(height);
}

// ------------------------------------------------------------------------------------------------

ColorBuffer::~ColorBuffer()
{
    data.clear();
}

// ------------------------------------------------------------------------------------------------

DrawingContext::DrawingContext(ColorBuffer* inputTexture, ColorBuffer* outputTexture, bool reversed)
{
    this->inputTexture = inputTexture;
    this->outputTexture = outputTexture;
    value = 0;
    this->reversed = reversed;
}