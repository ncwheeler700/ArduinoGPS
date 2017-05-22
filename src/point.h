#ifndef __POINT__
#define __POINT__
#include <stdlib.h>

class Point {

	public:
	Point() {}
	Point(char* lbl,double lat,double lon): _name(lbl),_lat(lat),_long(lon) {};

	const char* getName() {return _name;}
	const double getLat() {return _lat;}
	const double getLong() {return _long;}

	void rename(char* newName) {if (_name != newName) _name = newName;}
	void setLat(double newLat) {if (_lat != newLat) _lat = newLat;}
	void setLong(double newLong) {if (_long != newLong) _long = newLong;}

	private:
	char* _name;
	double _lat,_long;
};
#endif
