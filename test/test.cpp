/*
 * test.cpp
 *
 *  Created on: Mar 2, 2016
 *      Author: steve
 */

#define BOOST_TEST_MODULE libNmeaParser test
#include <boost/test/included/unit_test.hpp>
#include "../src/NmeaParser.h"

//int main() {

//  ----------------------------------------- 01  GPZDA -----------------------------------------
BOOST_AUTO_TEST_CASE( parseZDA ) {
	std::string nmeaZDA = "$GPZDA,160619.00,20,04,2016,8,3*6C";
	//std::string nmeaZDA = "$GPZDA,,,,,,*19";
	boost::posix_time::time_duration mtime;
	int day;
	int month;
	int year;
	int localZoneHours;
	int localZoneMinutes;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseZDA(nmeaZDA, mtime, day, month, year,
					localZoneHours, localZoneMinutes));
}

//  ----------------------------------------- 02  GPGLL -----------------------------------------
BOOST_AUTO_TEST_CASE( parseGLL ) {
	std::string nmeaGLL = "$GPGLL,1202.5313138,S,07708.5464255,W,155730.00,A,A*65";
	//std::string nmeaGLL = "$GPGLL,,,,,,,*19";
	double latitude;
	double longitude;
	boost::posix_time::time_duration mtime;
	char status;
	char modeIndicator;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseGLL(nmeaGLL, latitude, longitude, mtime, status,
					modeIndicator));
}

//  ----------------------------------------- 03  GPGGA -----------------------------------------
BOOST_AUTO_TEST_CASE( parseGGA ) {
	std::string nmeaGGA = "$GPGGA,172814.0,3723.46587704,N,12202.26957864,W,4,6,1.2,18.893,M,-25.669,M,2.5,0031*19";
	//std::string nmeaGGA = "$GPGGA,,,,,,,,,,,,,,*19";
	boost::posix_time::time_duration mtime;
	double latitude;
	double longitude;
	Nmea_GPSQualityIndicator quality;
	int numSV;
	double hdop;
	double orthometricheight;
	double geoidseparation;
	double agediffgps;
	std::string refid;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseGGA(nmeaGGA, mtime, latitude, longitude, quality,
					numSV, hdop, orthometricheight, geoidseparation, agediffgps,
					refid));
}

//  ----------------------------------------- 04  GPVTG -----------------------------------------
BOOST_AUTO_TEST_CASE( parseVTG ) {
	std::string nmeaVTG = "$GPVTG,8.86,T,10.29,M,0.02,N,0.04,K,A*19";
	//std::string nmeaVTG = "$GPVTG,,,,,,,,,*19";
	double coursetrue;
	double coursemagnetic;
	double speedknots;
	double speedkph;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseVTG(nmeaVTG, coursetrue, coursemagnetic,
					speedknots, speedkph));
}

//  ----------------------------------------- 05  GPRMC -----------------------------------------
BOOST_AUTO_TEST_CASE( parseRMC ) {
	std::string nmeaRMC = "$GPRMC,160618.00,A,1202.5313983,S,07708.5478298,W,0.10,166.87,200416,1.4,W,A,S*56";
	//std::string nmeaRMC = "$GPRMC,,,,,,,,,,,,,*56";

	boost::posix_time::time_duration mtime;
	double latitude;
	double longitude;
	double speedknots;
	double coursetrue;
	boost::gregorian::date mdate;
	double magneticvar;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseRMC(nmeaRMC, mtime, latitude, longitude,
					speedknots, coursetrue, mdate, magneticvar));
}

//  ----------------------------------------- 06  GPWPL -----------------------------------------
BOOST_AUTO_TEST_CASE( parseWPL ) {
	std::string nmeaWPL = "$GPWPL,4917.16,N,12310.64,W,003*19";
	//std::string nmeaWPL = "$GPWPL,,,,,*19";
	double latitude;
	double longitude;
	std::string waypointName;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseWPL(nmeaWPL, latitude, longitude, waypointName));
}

//  ----------------------------------------- 07  GPRTE -----------------------------------------
BOOST_AUTO_TEST_CASE( parseRTE ) {
	std::string nmeaRTE = "$GPRTE,3,3,c,WPT4,RTE-A,RTE-B,RTE-C,RTE-D,RTE-E,RTE-F*19";
	//std::string nmeaRTE = "$GPRTE,1,1,w,DAVID,JAVIER,VICTOR*19";
	//std::string nmeaRTE = "$GPRTE,,,,,,,,,,*19";
	double messagesTransmitted;
	double messageNumber;
	char messageMode;
	std::string routeIdentifier;
	std::vector<std::string> revenue1;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseRTE(nmeaRTE, messagesTransmitted, messageNumber,
					messageMode, routeIdentifier, revenue1));
}

//  ----------------------------------------- 08  VDVHW -----------------------------------------
BOOST_AUTO_TEST_CASE( parseVHW ) {
	std::string nmeaVHW = "$VDVHW,147.0,T,147.0,M,11.0,N,20.4,K*19";
	//std::string nmeaVHW = "$VDVHW,,,,,,,,*19";
	double speedInKnots;
	char knots;
	double speedInKmH;
	char kilometers;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseVHW(nmeaVHW, speedInKnots, knots, speedInKmH,
					kilometers));
}

//  ----------------------------------------- 09  VDMTW -----------------------------------------
BOOST_AUTO_TEST_CASE( parseMTW ) {
	std::string nmeaMTW = "$VDMTW,21.3,C*19";
	//std::string nmeaMTW = "$VDMTW,,*19";
	double degrees;
	char celcius;

	BOOST_REQUIRE_NO_THROW(NmeaParser::parseMTW(nmeaMTW, degrees, celcius));
}

//  ----------------------------------------- 10  VDVBW -----------------------------------------
BOOST_AUTO_TEST_CASE( parseVBW ) {
	//std::string nmeaVBW = "VDVBW,10.4,0.35,A,8.3,0.25,A";
	std::string nmeaVBW = "$VDVBW,15.0,02.4,A,15.0,04.6,A,00.0,A,06.8,A*19";
	//std::string nmeaVBW = "$VDVBW,,,,,,,,,,*19";
	double longitudinalWaterSpeed;
	double transverseWaterSpeed;
	char waterDataStatus;
	double longitudinalGroundSpeed;
	double transverseGroundSpeed;
	char groundDataStatus;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseVBW(nmeaVBW, longitudinalWaterSpeed,
					transverseWaterSpeed, waterDataStatus,
					longitudinalGroundSpeed, transverseGroundSpeed,
					groundDataStatus));
}

//  ----------------------------------------- 11  VDVLW -----------------------------------------
BOOST_AUTO_TEST_CASE( parseVLW ) {
	std::string nmeaVLW = "$VWVLW,0.225,N,0.238,A*19";
	//std::string nmeaVLW = "$VWVLW,,,,*19";
	double totalCumulativeDistance;
	char nauticalMiles1;
	double distanceSinceReset;
	char nauticalMiles2;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseVLW(nmeaVLW, totalCumulativeDistance,
					nauticalMiles1, distanceSinceReset, nauticalMiles2));
}

//  ----------------------------------------- 12  SDDPT -----------------------------------------
BOOST_AUTO_TEST_CASE( parseDPT ) {
	std::string nmeaDPT = "$SDDPT,1.8,2.6,1.3*19";
	//std::string nmeaDPT = "$SDDPT,,,*19";
	double waterDepthRelativeToTheTransducer;
	double offsetFromTransducer;
	double maximumRangeScaleInUse;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseDPT(nmeaDPT, waterDepthRelativeToTheTransducer,
					offsetFromTransducer, maximumRangeScaleInUse));
}

//  ----------------------------------------- 13  SDDBT -----------------------------------------
BOOST_AUTO_TEST_CASE( parseDBT ) {
	std::string nmeaDBT = "$SDDBT,05.9,f,01.8,M,01.0,F*19";
	//std::string nmeaDBT = "$SDDBT,,,,,,*19";
	double waterDepthInFeet;
	char feet;
	double waterDepthInMeters;
	char meters;
	double waterDepthInFathoms;
	char fathoms;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseDBT(nmeaDBT, waterDepthInFeet, feet,
					waterDepthInMeters, meters, waterDepthInFathoms, fathoms));
}

//  ----------------------------------------- 14  SDDBK -----------------------------------------
BOOST_AUTO_TEST_CASE( parseDBK ) {
	std::string nmeaDBK = "$SDDBK,05.9,f,01.8,M,01.0,F*19";
	//std::string nmeaDBK = "$SDDBK,,,,,,*19";
	double depthBelowKeelFeet;
	char feet;
	double depthBelowKeelMeters;
	char meters;
	double depthBelowKeelFathoms;
	char fathoms;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseDBK(nmeaDBK, depthBelowKeelFeet, feet,
					depthBelowKeelMeters, meters, depthBelowKeelFathoms,
					fathoms));
}

//  ----------------------------------------- 15  PSKPDPT -----------------------------------------
BOOST_AUTO_TEST_CASE( parsePSKPDPT ) {
	std::string nmeaPSKPDP = "$PSKPDPT,22.3,23.4,6.8,4,5,Fet*19";
	//std::string nmeaPSKPDP = "$PSKPDPT,,,,,,*19";
	double waterDepthRelativeToTheTransducer;
	double offsetFromTransducer;
	double maximumRangeScaleInUse;
	int bottomEchoStrength;
	int echoSounderChannelNumber;
	std::string transducerLocation;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parsePSKPDPT(nmeaPSKPDP,
					waterDepthRelativeToTheTransducer, offsetFromTransducer,
					maximumRangeScaleInUse, bottomEchoStrength,
					echoSounderChannelNumber, transducerLocation));
}

//  ----------------------------------------- 16  HCHDT -----------------------------------------
BOOST_AUTO_TEST_CASE( parseHDT ) {
	std::string nmeaHDT = "$HCHDT,077.5,T*19";
	//std::string nmeaHDT = "$HCHDT,,*19";
	double headingDegreesTrue;
	char t;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseHDT(nmeaHDT, headingDegreesTrue, t));
}

//  ----------------------------------------- 17  HCHDG -----------------------------------------
BOOST_AUTO_TEST_CASE( parseHDG ) {
	//std::string nmeaHDG = "HCHDG,19.4,0.7,A,2.1,E";
	std::string nmeaHDG = "$HCHDG,77.0,00.9,E,00.5,W*19";
	//std::string nmeaHDG = "$HCHDG,,,,,*19";
	double magneticSensorHeadingInDegrees;
	double magneticDeviationDegrees;
	char magneticDeviationDirection;
	double magneticVariationDegrees;
	char magneticVariationDirection;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseHDG(nmeaHDG, magneticSensorHeadingInDegrees,
					magneticDeviationDegrees, magneticDeviationDirection,
					magneticVariationDegrees, magneticVariationDirection));
}

//  ----------------------------------------- 18  HCHDM -----------------------------------------
BOOST_AUTO_TEST_CASE( parseHDM ) {
	std::string nmeaHDM = "$HCHDM,77.9,M*19";
	//std::string nmeaHDM = "$HCHDM,,*19";
	double headingDegreesMagnetic;
	char magnetic;

	BOOST_REQUIRE_NO_THROW(
			NmeaParser::parseHDM(nmeaHDM, headingDegreesMagnetic, magnetic));
}

//  ----------------------------------------- 19  TIROT -----------------------------------------
BOOST_AUTO_TEST_CASE( parseROT ) {
	std::string nmeaROT = "$TIROT,01.7,A*19";
	//std::string nmeaROT = "$TIROT,,*19";
	double rateOfTurn;
	char status;

	BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseROT(nmeaROT, rateOfTurn, status));
}

//  ----------------------------------------- 20  IIMWV -----------------------------------------
BOOST_AUTO_TEST_CASE( parseMWV ) {
	std::string nmeaMWV = "$IIMWV,004.2,T,007.2,N,A*19";
	//std::string nmeaMWV = "$IIMWV,,,,,*19";
	double windAngle;
	char reference;
	double windSpeed;
	char windSpeedUnits;
	char sensorStatus;

	BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseMWV(nmeaMWV, windAngle, reference, windSpeed,
			windSpeedUnits, sensorStatus));
}

//  ----------------------------------------- 21  WIMWD -----------------------------------------
BOOST_AUTO_TEST_CASE( parseMWD ) {
	std::string nmeaMWD = "$WIMWD,004.2,T,004.2,M,007.2,N,003.7,M*19";
	//std::string nmeaMWD = "$WIMWD,,,,,,,,*19";
	double trueWindDirection;
	char truee;
	double magneticWindDirection;
	char magnetic;
	double windSpeedKnots;
	char knots;
	double windSpeedMeters;
	char meters;

	BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseMWD(nmeaMWD, trueWindDirection, truee,
			magneticWindDirection, magnetic, windSpeedKnots, knots,
			windSpeedMeters, meters));
}

//  ----------------------------------------- 22  IIXDR -----------------------------------------
BOOST_AUTO_TEST_CASE( parseXDR ) {
		std::string nmeaXDR = "$IIXDR,C,25.4,C,TEMP,P,28.4,B,PRESS,H,27.4,P,RH*19";
		//std::string nmeaXDR = "IIXDR,,,a,,G,0.5,b,PRESS,G,2.1,,HUM";
		//std::string nmeaXDR = "$IIXDR,,,,,,,,,,,,*19";
		char typeOfSensorTemperature;
		double temperatureReading;
		char centigrade;
		std::string nameOfTransducer;
		char typeOfSensorPressure;
		double pressureReading;
		char bars;
		std::string nameOfPressureSensor;
		char typeOfSensorHumidity;
		double humidity;
		char humidityUnitsOfMeasurePercent;
		std::string nameOfRelativeHumiditySensor;

		BOOST_REQUIRE_NO_THROW(
		NmeaParser::parseXDR(nmeaXDR, typeOfSensorTemperature, temperatureReading,
				centigrade, nameOfTransducer, typeOfSensorPressure, pressureReading,
				bars, nameOfPressureSensor, typeOfSensorHumidity, humidity,
				humidityUnitsOfMeasurePercent, nameOfRelativeHumiditySensor));
}

//  ----------------------------------------- 23  IITTD -----------------------------------------
BOOST_AUTO_TEST_CASE( parseTTD ) {
		//  -------------------- 23  IITTD --------------------
		std::string nmeaTTD = "$IITTD,FF,FC,9,abcdefghijklmn,4*19";
		//std::string nmeaTTD = "IITTD,,,,,";
		int aaa;
		int bbb;
		int ccc;
		std::string ddd;
		int eee;

		BOOST_REQUIRE_NO_THROW(
		NmeaParser::parseTTD(nmeaTTD, aaa, bbb, ccc, ddd, eee));
}

//  ----------------------------------------- 24  RATLB -----------------------------------------
BOOST_AUTO_TEST_CASE( parseTLB ) {
		std::string nmeaTLB = "$RATLB,5.5,cinco,1.1,uno,2.2,dos,3.3,tres,4.4,cuatro*19";
		//std::string nmeaTLB = "$RATLB*19";
		//std::string nmeaTLB = "$RATLB,01,uno*19";
		std::vector<std::pair<double, std::string>> revenue;

		BOOST_REQUIRE_NO_THROW(
		NmeaParser::parseTLB(nmeaTLB, revenue));
}

//  ----------------------------------------- 25  RAOSD -----------------------------------------
BOOST_AUTO_TEST_CASE( parseOSD ) {
	std::string nmeaOSD = "$RAOSD,154.3,A,16.4,a,16.5,a,16.6,12.3,a*19";
	//std::string nmeaOSD = "$RAOSD,,,,,,,,,*19";
	double heading;
	char status;
	double vesselCourse;
	char referenceCourse;
	double vesselSpeed;
	char referenceSpeed;
	double vesselSet;
	double vesselDrift;
	char speedUnits;

	BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseOSD(nmeaOSD, heading, status, vesselCourse,
			referenceCourse, vesselSpeed, referenceSpeed, vesselSet,
			vesselDrift, speedUnits));

}

//  ----------------------------------------- 26  RARSD -----------------------------------------
BOOST_AUTO_TEST_CASE( parseRSD ) {
		std::string nmeaRSD =
				"$RARSD,4.1,4.2,4.3,4.4,4.5,4.6,4.7,4.8,4.9,4.10,4.11,N,C*19";
		//std::string nmeaRSD =
			//	"$RARSD,,,,,,,,,,,,,*19";
		double origin1Range;
		double origin1BearingDegrees;
		double variableRangeMarker1;
		double bearingLine1;
		double origin2Range;
		double origin2Bearing;
		double vrm2;
		double ebl2;
		double cursorRange;
		double cursorBearing;
		double rangeScale;
		char rangeUnits;
		char displayRotation;

		BOOST_REQUIRE_NO_THROW(
		NmeaParser::parseRSD(nmeaRSD, origin1Range, origin1BearingDegrees,
				variableRangeMarker1, bearingLine1, origin2Range, origin2Bearing,
				vrm2, ebl2, cursorRange, cursorBearing, rangeScale, rangeUnits,
				displayRotation));
}

//	//  -------------------- 27  AIVDM --------------------
//	//std::string nmeaVDM = "!AIVDM,2,1,7,A,5000Htl000000000000<518T<u8pTuwF0000001S0`e43t0Ht0000000,0*1A";
//	//std::string nmeaVDM = "!AIVDM,2,2,7,A,000000000000000,2*23";
//	std::string nmeaVDM = "!AIVDM,1,1,,A,3;DjgwgOh3JNocMq9rALeUi6010P,0*1F";
//	int typeAIS;
//	int mmsiAIS;
//	double latitudeAIS;
//	double longitudeAIS;
//	float speedAIS;
//	float courseAIS;
//	float headingAIS;
//
//	//BOOST_REQUIRE_NO_THROW(
//	NmeaParser::parseVDM(nmeaVDM, typeAIS, mmsiAIS, latitudeAIS, longitudeAIS,
//			speedAIS, courseAIS, headingAIS);

//}
