/**
 * @file 
 * ColoredPoint3D.cpp
 *
 * @date: Dec 21, 2009
 * @author: sblume
 */

#include "ColoredPoint3D.h"

#include <stdexcept>

using std::runtime_error;

namespace BRICS_3D {


ColoredPoint3D::ColoredPoint3D() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
	red = 0;
	green = 0;
	blue = 0;
}

ColoredPoint3D::ColoredPoint3D(ColoredPoint3D* point) {
	x = point->x;
	y = point->y;
	z = point->z;
	red = point->red;
	green = point->green;
	blue = point->blue;
}

ColoredPoint3D::ColoredPoint3D(double x, double y, double z, unsigned char red, unsigned char green, unsigned char blue) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->red = red;
	this->green = green;
	this->blue = blue;
}


ColoredPoint3D::~ColoredPoint3D() {

}

istream& operator>>(istream &inStream, ColoredPoint3D &point) {
	unsigned int tmpColor;

	if (!inStream.good())
		throw runtime_error("ERROR: cannot read x coordinate of point.");
	inStream >> point.x;
	if (!inStream.good())
		throw runtime_error("ERROR: cannot read y coordinate of point.");
	inStream >> point.y;
	if (!inStream.good())
		throw runtime_error("ERROR: cannot read z coordinate of point.");
	inStream >> point.z;

	if (!inStream.good())
		throw runtime_error("ERROR: cannot read red color channel of point.");
	inStream >> tmpColor;
	point.red = static_cast<unsigned char>(tmpColor);

	if (!inStream.good())
		throw runtime_error("ERROR: cannot read green color channel of point.");
	inStream >> tmpColor;
	point.green = static_cast<unsigned char>(tmpColor);

	if (!inStream.good())
		throw runtime_error("ERROR: cannot read blue color channel of point.");
	inStream >> tmpColor;
	point.blue = static_cast<unsigned char>(tmpColor);

	return inStream;
}

ostream& operator<<(ostream &outStream, const ColoredPoint3D &point) {
	outStream << point.x << " " << point.y << " " << point.z;
	outStream << " " << static_cast<unsigned int>(point.red);
	outStream << " " << static_cast<unsigned int>(point.green);
	outStream << " " << static_cast<unsigned int>(point.blue);

	return outStream;
}



}

/* EOF */