/*
 * NmeaParser.h
 *
 *  Created on: Feb 3, 2016
 *      Author: steve
 */

#ifndef SRC_NMEAPARSER_H_
#define SRC_NMEAPARSER_H_

#include <vector>
#include <string>
#include <boost/date_time.hpp>
#include "NmeaEnums.h"
#include <bitset>

typedef std::bitset<16> NmeaParserResult;

class NmeaParser {
public:
	NmeaParser();
	virtual ~NmeaParser();

	//  -------------------- 01  --ZDA --------------------
	//	--ZDA message fields
	//	Field 	Meaning
	//	0 	Message ID $--ZDA
	//	1 	UTC
	//	2 	Day, ranging between 01 and 31
	//	3 	Month, ranging between 01 and 12
	//	4 	Year
	//	5 	Local time zone offset from GMT, ranging from 00 through ±13 hours
	//	6 	Local time zone offset from GMT, ranging from 00 through 59 minutes
	//	7 	The checksum data, always begins with *
	static NmeaParserResult parseZDA(const std::string& nmea,
			boost::posix_time::time_duration& mtime, int& day, int& month,
			int& year, int& localZoneHours, int& localZoneMinutes);

	//  -------------------- 02  --GLL --------------------
	//	--GLL message fields
	//	Field 	Meaning
	//	0 	Message ID $--GLL
	//	1 	Latitude in dd mm,mmmm format (0-7 decimal places)
	//	2 	Direction of latitude N: North S: South
	//	3 	Longitude in ddd mm,mmmm format (0-7 decimal places)
	//	4 	Direction of longitude E: East W: West
	//	5 	UTC of position in hhmmss.ss format
	//	6 	Fixed text "A" shows that data is valid
	//  7   Fixed text "A" shows that data is valid
	//	8 	The checksum data, always begins with *
	static NmeaParserResult parseGLL(const std::string& nmea, double& latitude,
			double& longitude, boost::posix_time::time_duration& mtime,
			char& status, char& modeIndicator);

	//  -------------------- 03  --GGA --------------------
	//	--GGA message fields
	//	Field 	Meaning
	//	0 	Message ID $--GGA
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
	static NmeaParserResult parseGGA(const std::string& nmea,
			boost::posix_time::time_duration& mtime, double& latitude,
			double& longitude, Nmea_GPSQualityIndicator& quality, int& numSV,
			double& hdop, double& orthometricheight, double& geoidseparation,
			double& agediffgps, std::string& refid);

	//  -------------------- 04  --VTG --------------------
	//	--VTG message fields
	//	Field 	Meaning
	//	0 	Message ID $--VTG
	//	1 	Course over ground (degrees true)
	//	2 	T: relative to true north
	//	3 	Course over ground (degrees magnetic)
	//	4 	M: relative to magnetic north
	//	5 	Speed, in knots
	//	6 	N: speed is measured in knots
	//	7 	Speed over ground in kilometers/hour (kph)
	//	8 	K: speed over ground is measured in kph
	//	9 	The checksum data, always begins with *
	static NmeaParserResult parseVTG(const std::string& nmea,
			double& coursetrue, double& coursemagnetic, double& speedknots,
			double& speedkph);

	//  -------------------- 05  --RMC --------------------
	//	--RMC message fields
	//	Field 	Meaning
	//	0 	Message ID $--RMC
	//	1 	UTC of position fix
	//	2 	Status A=active or V=void
	//	3 	Latitude
	//	4 	Longitude
	//	5 	Speed over the ground in knots
	//	6 	Track angle in degrees (True)
	//	7 	Date
	//	8 	Magnetic variation in degrees
	//	9 	The checksum data, always begins with *
	static NmeaParserResult parseRMC(const std::string& nmea,
			boost::posix_time::time_duration& mtime, double& latitude,
			double& longitude, double& speedknots, double& coursetrue,
			boost::gregorian::date& mdate, double& magneticvar);

	//  -------------------- 06  --WPL --------------------
	//	--WPL message fields
	//	Field 	Meaning
	//	0 	Message ID $--WPL
	//	1 	Latitude
	//	2 	N or S (North or South)
	//	3 	Longitude
	//	4 	E or W (East or West)
	//	5 	Waypoint Name
	//	6	Checksum
	static NmeaParserResult parseWPL(const std::string& nmea, double& latitude,
			double& longitude, std::string& waypointName);

	//  -------------------- 07  --RTE --------------------
	//	--RTE message fields
	//	Field 	Meaning
	//	0 	Message ID $--RTE
	//	1 	Number of sentences in sequence
	//	2 	Sentence number
	//	3 	'c' = Current active route, 'w' = waypoint list starts with destination waypoint
	//	4 	Name or number of the active route
	//	5 	onwards, Names of waypoints in Route
	static NmeaParserResult parseRTE(const std::string& nmea, int& totalLines,
			int& lineCount, char& messageMode, std::string& routeIdentifier,
			std::vector<std::string>& waypointNames);

	//  -------------------- 08  VDVHW --------------------
	//	VDVHW message fields
	//	Field 	Meaning
	//	0 	Message ID $VDVHW
	//	1 	Degress True
	//	2 	T = True
	//	3 	Degrees Magnetic
	//	4 	M = Magnetic
	//	5 	Knots (speed of vessel relative to the water)
	//	6	N = Knots
	//	7	Kilometers (speed of vessel relative to the water)
	//	8	K = Kilometres
	//	9	Checksum
	static NmeaParserResult parseVHW(const std::string& nmea,
			double& speedInKnots, char& knots, double& speedInKmH,
			char& kilometers);

	//  -------------------- 09  VDMTW --------------------
	//	VDMTW message fields
	//	Field 	Meaning
	//	0 	Message ID $VDMTW
	//	1 	Degrees
	//	2 	Unit of Measurement, Celcius
	//	3 	Checksum
	static NmeaParserResult parseMTW(const std::string& nmea, double& degrees,
			char& celcius);

	//  -------------------- 10  VDVBW --------------------
	//	VDVBW message fields
	//	Field 	Meaning
	//	0 	Message ID $VDVBW
	//	1 	Longitudinal water speed, "-" means astern
	//	2 	Transverse water speed, "-" means port
	//	3 	Water Data Status, A = data valid
	//	4 	Longitudinal ground speed, "-" means astern
	//	5 	Transverse ground speed, "-" means port
	//	6	Ground Data Status, A = data valid
	//	7	Checksum
	static NmeaParserResult parseVBW(const std::string& nmea,
			double& longitudinalWaterSpeed, double& transverseWaterSpeed,
			char& waterDataStatus, double& longitudinalGroundSpeed,
			double& transverseGroundSpeed, char& groundDataStatus);

	//  -------------------- 11  VDVLW --------------------
	//	VDVLW message fields
	//	Field 	Meaning
	//	0 	Message ID $VDVLW
	//	1 	Total cumulative distance
	//	2 	N = Nautical Miles
	//	3 	Distance since Reset
	//	4 	N = Nautical Miles
	//	5 	Checksum
	static NmeaParserResult parseVLW(const std::string& nmea,
			double& totalCumulativeDistance, char& nauticalMiles1,
			double& distanceSinceReset, char& nauticalMiles2);

	//  -------------------- 12  SDDPT --------------------
	//	SDDPT message fields
	//	Field 	Meaning
	//	0 	Message ID $SDDPT
	//	1 	Depth, meters
	//	2 	Offset from transducer;
	//		positive means distance from transducer to water line,
	//		negative means distance from transducer to keel
	//	3 	Checksum
	static NmeaParserResult parseDPT(const std::string& nmea,
			double& waterDepthRelativeToTheTransducer,
			double& offsetFromTransducer, double& maximumRangeScaleInUse);

	//  -------------------- 13  SDDBT --------------------
	//	SDDBT message fields
	//	Field 	Meaning
	//	0 	Message ID $SDDBT
	//	1 	Depth, feet
	//	2 	f = feet
	//	3 	Depth, meters
	//	4 	M = meters
	//	5 	Depth, Fathoms
	//	6	F = Fathoms
	//	7	Checksum
	static NmeaParserResult parseDBT(const std::string& nmea,
			double& waterDepthInFeet, char& feet, double& waterDepthInMeters,
			char& meters, double& waterDepthInFathoms, char& fathoms);

	//  -------------------- 14  SDDBK --------------------
	//	SDDBK message fields
	//	Field 	Meaning
	//	0 	Message ID $SDDBK
	//	1 	Depth, feet
	//	2 	f = feet
	//	3 	Depth, meters
	//	4 	M = meters
	//	5 	Depth, Fathoms
	//	6	F = Fathoms
	//	7	Checksum
	static NmeaParserResult parseDBK(const std::string& nmea,
			double& depthBelowKeelFeet, char& feet,
			double& depthBelowKeelMeters, char& meters,
			double& depthBelowKeelFathoms, char& fathoms);

	//  -------------------- 15  PSKPDPT --------------------
	//	PSKPDPT message fields
	//	Field 	Meaning
	//	0 	Message ID $GPPSKPDPT
	//	1 	Water depth relative to transducer, meters
	//	2 	Offset from transducer, meters
	//	3 	Maximum range scale in use, meters
	//	4 	Bottom echo strength (0,9)
	//	5 	Echo sounder channel number (0-99)**
	//	6 	Transducer location *
	//	7	Check sum, possible to turn on/off (see screen 8)
	static NmeaParserResult parsePSKPDPT(const std::string& nmea,
			double& waterDepthRelativeToTheTransducer,
			double& offsetFromTransducer, double& maximumRangeScaleInUse,
			int& bottomEchoStrength, int& echoSounderChannelNumber,
			std::string& transducerLocation);

	//  -------------------- 16  HCHDT --------------------
	//	HCHDT message fields
	//	Field 	Meaning
	//	0 	Message ID $HCHDT
	//	1 	Heading Degrees, true
	//	2 	T = True
	//	3 	Checksum
	static NmeaParserResult parseHDT(const std::string& nmea,
			double& headingDegreesTrue, char& t);

	//  -------------------- 17  HCHDG --------------------
	//	HCHDG message fields
	//	Field 	Meaning
	//	0 	Message ID $HCHDG
	//	1 	Magnetic Sensor heading in degrees
	//	2 	Magnetic Deviation, degrees
	//	3 	Magnetic Deviation direction, E = Easterly, W = Westerly
	//	4 	Magnetic Variation degrees
	//	5 	Magnetic Variation direction, E = Easterly, W = Westerly
	//	6	Checksum
	static NmeaParserResult parseHDG(const std::string& nmea,
			double& magneticSensorHeadingInDegrees,
			double& magneticDeviationDegrees, char& magneticDeviationDirection,
			double& magneticVariationDegrees, char& magneticVariationDirection);

	//  -------------------- 18  HCHDM --------------------
	//	HCHDM message fields
	//	Field 	Meaning
	//	0 	Message ID $HCHDM
	//	1 	Heading Degrees, magnetic
	//	2 	M = magnetic
	//	3 	Checksum
	static NmeaParserResult parseHDM(const std::string& nmea,
			double& headingDegreesMagnetic, char& magnetic);

	//  -------------------- 19  TIROT --------------------
	//	--ROT message fields
	//	Field 	Meaning
	//	0 	Message ID $--ROT
	//	1 	Rate Of Turn, degrees per minute, "-" means bow turns to port
	//	2 	Status, A means data is valid
	//	3 	Checksum
	static NmeaParserResult parseROT(const std::string& nmea,
			double& rateOfTurn, char& status);

	//  -------------------- 20  IIMWV --------------------
	//	IIMWV message fields
	//	Field 	Meaning
	//	0 	Message ID $IIMWV
	//	1 	Wind Angle, 0 to 360 degrees
	//	2 	Reference, R = Relative, T = True
	//	3 	Wind Speed
	//	4 	Wind Speed Units, K = km/hr, M = m/sec, N = kt
	//	5 	Sensor Status, A = Valid, V = Void
	//	6	Checksum
	static NmeaParserResult parseMWV(const std::string& nmea, double& windAngle,
			char& reference, double& windSpeed, char& windSpeedUnits,
			char& sensorStatus);

	//  -------------------- 21  --MWD --------------------
	//	--MWD message fields
	//	Field 	Meaning
	//	0 	Message ID --MWD
	//	1  	Wind direction, 0° to 359º
	//	2	True
	//	3	Wind direction, 0° to 359º
	//	4	Magnetic
	//	5	Wind speed
	//	6	Knots
	//	7	Wind speed
	//	8	meters/second
	static NmeaParserResult parseMWD(const std::string& nmea,
			double& trueWindDirection, char& t, double& magneticWindDirection,
			char& magnetic, double& windSpeedKnots, char& knots,
			double& windSpeedMeters, char& meters);

	//  -------------------- 22  --XDR --------------------
	//	--XDR message fields
	//	Field 	Meaning
	//	0 	Message ID --XDR
	//	1 	Transducer type
	//	2 	Measurement data
	//	3 	Units of measurement
	//	4 	Name of transducer
	//	X 	More of the same
	//	n	Checksum
	static NmeaParserResult parseXDR(const std::string& nmea,
			char& typeOfSensorTemperature, double& temperatureReading,
			char& centigrade, std::string& nameOfTransducer,
			char& typeOfSensorPressure, double& pressureReading, char& bars,
			std::string& nameOfPressureSensor, char& typeOfSensorHumidity,
			double& humidity, char& humidityUnitsOfMeasurePercent,
			std::string& nameOfRelativeHumiditySensor);

	//  --------------------   --TTM --------------------
	//	--TTM message fields
	//  Field   Meaning
	//  0   Message ID $--TTM
	//  1	Target Number (0-99)
	//	2   Target Distance from own ship
	// 	3   Bearing from own ship
	//  4   (T/R) true/relative
	//  5   Target Speed
	//  6   Target Course
	//  7 	(T/R) true relative
	//  8   Distance of closest-point-of-approach
	//  9   Time to CPA, min., "-" means increasing
	//  10   Speed units, (K/N/S)
	//  11  Target Name
	//  12  Target status
	//  13  Reference Target R, null otherwise
	//  14  Time of data UTC
	//  15  Type of acquisition A/M/R
	//  16  Checksum
	static NmeaParserResult parseTTM(const std::string& nmea, int& targetNumber,
			double& targetDistance, double& targetBearing,
			Nmea_AngleReference& targetBearingReference, double& targetSpeed,
			double& targetCourse, Nmea_AngleReference& targetCourseReference,
			Nmea_SpeedDistanceUnits& speedDistanceUnits,
			std::string& targetName, Nmea_TargetStatus& targetStatus,
			boost::posix_time::time_duration& timeOfData,
			Nmea_TypeOfAcquisition& typeOfAcquisition);

	//  -------------------- 23  --TTD --------------------
	//	--TTD message fields
	//	Field 	Meaning
	//  0   Message ID --TTD
	//  1   Total hex number of sentences needed to transfer the message, 1 to FF
	//  2   Hex sentence number, 1 to FF
	//  3   Sequential message identifier, 0 to 9
	//  4   Encapsulated tracked target data
	//  5   Number of fill-bits, 0 to 5
	static NmeaParserResult parseTTD(const std::string& nmea, int& totalLines,
			int& lineCount, int& sequenceIdentifier, std::string& trackData,
			int& fillBits);

	//  -------------------- 24  --TBL --------------------
	//	--TBL message fields
	//	Field 	Meaning
	//	0 	Message ID --TBL
	//	1 	Target number ´'n' reported by device
	//	2 	Label assigned to target 'n'
	//	3 	Additional label pairs
	//	n	Checksum
	static NmeaParserResult parseTLB(const std::string& nmea,
			std::vector<std::pair<int, std::string>>& trackNumbernLabel);

	//  -------------------- 25  --OSD --------------------
	//	--OSD message fields
	//	Field 	Meaning
	//	0 	Message ID --OSD
	//	1 	Heading, degrees true
	//	2 	Status, A = Data Valid
	//	3 	Vessel Course, degrees True
	//	4 	Course Reference
	//	5 	Vessel Speed
	//	6	Speed Reference
	//	7	Vessel Set, degrees True
	//	8	Vessel drift (speed)
	//	9	Speed Units
	//	10	Checksum
	static NmeaParserResult parseOSD(const std::string& nmea, double& heading,
			char& status, double& vesselCourse, char& referenceCourse,
			double& vesselSpeed, char& referenceSpeed, double& vesselSet,
			double& vesselDrift, char& speedUnits);

	//  -------------------- 26  --RSD --------------------
	//	--RSD message fields
	//	Field 	Meaning
	//	0 	Message ID --RSD
	//	1 	Original range from own ship
	//	2 	Original bearing degrees from 0
	//	3 	Variable range marker 1 range
	//	4 	Bearing Line 1(EBL1) degrees from 0
	//	5 	origin2 range
	//	6	origin2 bearing
	//	7 	VRM2, range
	//	8 	EBL2 degrees
	//	9 	Cursor range, from own ship
	//	10 	Cursor bearing, degrees CW from 0
	//	11	range scale (maximum)
	//	12	range units, K/N/S
	//	13	display rotation
	//	14	Checksum
	static NmeaParserResult parseRSD(const std::string& nmea,
			double& origin1Range, double& origin1BearingDegrees,
			double& variableRangeMarker1, double& bearingLine1,
			double& origin2Range, double& origin2Bearing, double& vrm2,
			double& ebl2, double& cursorRange, double& cursorBearing,
			double& rangeScale, char& rangeUnits, char& displayRotation);

	//  -------------------- 27  --VDM --------------------
	//	--VDM message fields
	//	Field 	Meaning
	//  0   Message ID --VDM
	//  1   Total hex number of sentences needed to transfer the message, 1 to FF
	//  2   Hex sentence number, 1 to FF
	//  3   Sequential message identifier, 0 to 9
	//  4   AIS Channel
	//  5   Encapsulated data
	//  6   Number of fill-bits, 0 to 5
	static NmeaParserResult parseVDM(const std::string& nmea, int& totalLines,
			int& lineCount, int& sequenceIdentifier, char& aisChannel,
			std::string& encodedData, int& fillBits);

	//  -------------------- 28  --VDO --------------------
	//	--VDM message fields
	//	Field 	Meaning
	//  0   Message ID --VDO
	//  1   Total hex number of sentences needed to transfer the message, 1 to FF
	//  2   Hex sentence number, 1 to FF
	//  3   Sequential message identifier, 0 to 9
	//  4   AIS Channel
	//  5   Encapsulated data
	//  6   Number of fill-bits, 0 to 5
	static NmeaParserResult parseVDO(const std::string& nmea, int& totalLines,
			int& lineCount, int& sequenceIdentifier, char& aisChannel,
			std::string& encodedData, int& fillBits);

private:
	static bool tokenizeSentence(const std::string& nmea,
			std::vector<std::string>& tokens);
	static bool decodeTime(std::vector<std::string>::iterator &i,
			boost::posix_time::time_duration& out,
			const boost::posix_time::time_duration& def);
	static bool decodeLatLng(std::vector<std::string>::iterator &i, double& out,
			const double& def);
	static bool decodeDate(std::vector<std::string>::iterator &i,
			boost::gregorian::date& out, const boost::gregorian::date& def);
	static double toDecimalDegree(double degrees, double minutes,
			double seconds, char hemisphere);
	static bool decodeString(std::vector<std::string>::iterator &i,
			std::string& out, const std::string& def);
	static bool decodeHex(std::vector<std::string>::iterator &i, uint& out,
			const uint& def);
	template<typename Target>
	static bool decodeDefault(std::vector<std::string>::iterator &i,
			Target &out, const Target& def);
};

#endif /* SRC_NMEAPARSER_H_ */
