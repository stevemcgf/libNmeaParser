/*
 * NmeaParser.h
 *
 *  Created on: Feb 3, 2016
 *      Author: steve
 */

#ifndef SRC_NMEAPARSER_H_
#define SRC_NMEAPARSER_H_

#include <string>
#include <boost/date_time.hpp>
#include "NmeaEnums.h"

class NmeaParser {
public:
	NmeaParser();
	virtual ~NmeaParser();

	//	GLL message fields
	//	Field 	Meaning
	//	0 	Message ID $GPGLL
	//	1 	Latitude in dd mm,mmmm format (0-7 decimal places)
	//	2 	Direction of latitude N: North S: South
	//	3 	Longitude in ddd mm,mmmm format (0-7 decimal places)
	//	4 	Direction of longitude E: East W: West
	//	5 	UTC of position in hhmmss.ss format
	//	6 	Fixed text "A" shows that data is valid
	//	7 	The checksum data, always begins with *
	static void parseGLL(const std::string& nmea, double& latitude,
			double& longitude, boost::posix_time::time_duration& mtime);

	//	GGA message fields
	//	Field 	Meaning
	//	0 	Message ID $GPGGA
	//	1 	UTC of position fix
	//	2 	Latitude
	//	3 	Direction of latitude: N: North S: South
	//	4 	Longitude
	//	5 	Direction of longitude: E: East W: West
	//	6 	GPS Quality indicator: 0: Fix not valid 1: GPS fix 2: Differential GPS fix, OmniSTAR VBS 4: Real-Time Kinematic, fixed integers 5: Real-Time Kinematic, float integers, OmniSTAR XP/HP or Location RTK
	//	7 	Number of SVs in use, range from 00 through to 24+
	//	8 	HDOP
	//	9 	Orthometric height (MSL reference)
	//	10 	M: unit of measure for orthometric height is meters
	//	11 	Geoid separation
	//	12 	M: geoid separation measured in meters
	//	13 	Age of differential GPS data record, Type 1 or Type 9. Null field when DGPS is not used.
	//	14 	Reference station ID, range 0000-4095. A null field when any reference station ID is selected and no corrections are received1.
	//	15 	The checksum data, always begins with *
	static void parseGGA(const std::string& nmea, int& hours, int& minutes,
			double& seconds, double& latitude, double& longitude,
			Nmea_GPSQualityIndicator& quality, int& numSV, double& hdop,
			float& orthometricheight, double& geoidseparation,
			double& agediffgps, std::string& refid);

	//	VTG message fields
	//	Field 	Meaning
	//	0 	Message ID $GPVTG
	//	1 	Course over ground (degrees true)
	//	2 	T: relative to true north
	//	3 	Course over ground (degrees magnetic)
	//	4 	M: relative to magnetic north
	//	5 	Speed, in knots
	//	6 	N: speed is measured in knots
	//	7 	Speed over ground in kilometers/hour (kph)
	//	8 	K: speed over ground is measured in kph
	//	9 	The checksum data, always begins with *
	static void parseVTG(const std::string& nmea, double& coursetrue,
			double& coursemagnetic, double& speedknots, double& speedkph);

	//	GPRMC message fields
	//	Field 	Meaning
	//	0 	Message ID $GPRMC
	//	1 	UTC of position fix
	//	2 	Status A=active or V=void
	//	3 	Latitude
	//	4 	Longitude
	//	5 	Speed over the ground in knots
	//	6 	Track angle in degrees (True)
	//	7 	Date
	//	8 	Magnetic variation in degrees
	//	9 	The checksum data, always begins with *
	static void parseRMC(const std::string& nmea, boost::posix_time::time_duration& mtime,
			double& latitude, double& longitude, double& speedknots,
			double& coursetrue, boost::gregorian::date& mdate, double& magneticvar);

private:
	class impl;
};

#endif /* SRC_NMEAPARSER_H_ */
