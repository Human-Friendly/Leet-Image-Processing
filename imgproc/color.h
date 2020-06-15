#pragma once

class Color
{
public:
	Color(unsigned char red, unsigned char green, unsigned char blue);

public:

	unsigned char red,
		green,
		blue;
};

Color::Color(unsigned char red, unsigned char green, unsigned char blue)
{
	this->red = red;
	this->green = green;
	this->blue = blue;
}