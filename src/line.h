#include "point.h"
#include <math.h>
#include <TinyGPS++.h>


class Line {
	public:
	Line() {boatSet = pinSet = false;}

	const Point getPin() {return pin;}
	const Point getBoat() {return boat;}
  double distToLine(Point us);

	void setPin(double lat, double lon) {pin = Point("pin",lat,lon);}
	void setBoat(double lat, double lon) {boat = Point("boat",lat,lon);}


	private:
	Point boat,pin;
	bool boatSet,pinSet;

};
