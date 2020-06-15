// # 1337Coding # Martin Angelov 06.14.2020 # ----> Leet Image Processing buffer.h

#pragma once
#include <vector>

// ------------------------------------------------------------------------------------------------
//
//

class Color
{
public:
	Color() { red = green = blue = 0; }
	Color(unsigned char red, unsigned char green, unsigned char blue);

public:

	unsigned char red,
		green,
		blue;
};

//
//
// ------------------------------------------------------------------------------------------------
//
//

class ColorBuffer
{
public:
	ColorBuffer() { width = height = 0; };
	ColorBuffer(int width, int height);
	ColorBuffer(std::vector<Color> data, int width, int height);
	~ColorBuffer();

	int getWidth() { return width; }
	int getHeight() { return height; }

	void setWidth(int width) { this->width = width; }
	void setHeight(int height) { this->height = height; }

	std::vector<Color> GetData() { return data; }

	void setDataValue(Color value, int index) { data[index] = value; }
	Color getDataValue(int index) { return data[index]; }

private:

	std::vector<Color> data;

	int width,
		height;
};

//
//
// ------------------------------------------------------------------------------------------------
//
//

class DrawingContext
{
public:
	DrawingContext(ColorBuffer* inputTexture, ColorBuffer* outputTexture, bool reversed);

	ColorBuffer* getInputTexture() { return inputTexture; }
	ColorBuffer* getOutputTexture() { return outputTexture; }
	float getValue() { return value; }
	bool isReversed() { return reversed; }
	void setValue(float value) { this->value = value; }

private:

	ColorBuffer *inputTexture,
		*outputTexture;

	float value;
	bool reversed;
};