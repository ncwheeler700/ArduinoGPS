#include "line.h"

double Line::distToLine(Point us) {

	double lineLength = TinyGPSPlus::distanceBetween(pin.getLat(),pin.getLong(),
	boat.getLat(),boat.getLong());
	double distToPin = TinyGPSPlus::distanceBetween(pin.getLat(),pin.getLong(),
	us.getLat(),us.getLong());
	double distToBoat = TinyGPSPlus::distanceBetween(us.getLat(),us.getLong(),
	boat.getLat(),boat.getLong());

	double s = (lineLength + distToPin + distToBoat)/2;
	double area = sqrt(s*(s-lineLength)*(s-distToPin)*(s-distToBoat));
	return 2*area/lineLength;
}
