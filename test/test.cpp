/*
 * test.cpp
 *
 *  Created on: Mar 2, 2016
 *      Author: steve
 */

//#define BOOST_TEST_MODULE libNmeaParser test
//#include <boost/test/included/unit_test.hpp>
#include "../src/NmeaParser.h"

int main() {

	//  -------------------- 01  GPZDA --------------------
	//std::string nmeaZDA = "GPZDA,141526.30,12,04,2016,8,30";
	std::string nmeaZDA = "GPZDA,,,,,,";
	boost::posix_time::time_duration mtime;
	int day;
	int month;
	int year;
	int localZoneHours;
	int localZoneMinutes;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseZDA(nmeaZDA, mtime, day, month, year, localZoneHours,
			localZoneMinutes);

	//  -------------------- 02  GPGLL --------------------
	//std::string nmeaGLL = "GPGLL,1205.35883,S,07718.42560,W,154748.47,A,D";
	std::string nmeaGLL = "GPGLL,,,,,,,";
	double latitude;
	double longitude;
	char status;
	char modeIndicator;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseGLL(nmeaGLL, latitude, longitude, mtime, status,
			modeIndicator);

	//  -------------------- 03  GPGGA --------------------
	//std::string nmeaGGA =
		//	"GPGGA,172814.0,3723.46587704,N,12202.26957864,W,4,6,1.2,18.893,M,-25.669,M,2.5,0031";
	std::string nmeaGGA =
			"GPGGA,,,,,,,,,,,,,,";
	Nmea_GPSQualityIndicator quality;
	int numSV;
	double hdop;
	double orthometricheight;
	double geoidseparation;
	double agediffgps;
	std::string refid;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseGGA(nmeaGGA, mtime, latitude, longitude, quality, numSV,
			hdop, orthometricheight, geoidseparation, agediffgps, refid);

	//  -------------------- 04  GPVTG --------------------
	//std::string nmeaVTG = "GPVTG,352,T,354,M,01.2,N,03.3,K,A";
	std::string nmeaVTG = "GPVTG,,,,,,,,,";
	double coursetrue;
	double coursemagnetic;
	double speedknots;
	double speedkph;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseVTG(nmeaVTG, coursetrue, coursemagnetic, speedknots,
			speedkph);

	//  -------------------- 05  GPRMC --------------------
	//std::string nmeaRMC =
	//	"GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280511,,,A,";
	//std::string nmeaRMC =
	//		"GPRMC,151710.62,A,1205.42606,S,07718.72440,W,0013.0,077.0,120416,0.0,W,A";
	//std::string nmeaRMC =
		//	"GPRMC,153146.21,V,1205.39366,S,07718.58040,W,0013.0,077.0,120416,0.4,W,P,V";
	std::string nmeaRMC =
			"GPRMC,,,,,,,,,,,,,";
	boost::gregorian::date mdate;
	double magneticvar;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseRMC(nmeaRMC, mtime, latitude, longitude, speedknots,
			coursetrue, mdate, magneticvar);

	//  -------------------- 06  GPWPL --------------------
	//std::string nmeaWPL = "GPWPL,4917.16,N,12310.64,W,003";
	std::string nmeaWPL = "GPWPL,,,,,";
	std::string waypointName;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseWPL(nmeaWPL, latitude, longitude, waypointName);

	//  -------------------- 07  GPRTE --------------------
	//std::string nmeaRTE = "GPRTE,3,3,c,WPT4,RTE-A,RTE-B,RTE-C,RTE-D,RTE-E,RTE-F";
	std::string nmeaRTE = "GPRTE,,,,,,,,,,";
	double messagesTransmitted;
	double messageNumber;
	char messageMode;
	std::string routeIdentifier;
	std::vector<std::string> revenue1;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseRTE(nmeaRTE, messagesTransmitted, messageNumber,
			messageMode, routeIdentifier, revenue1);

	//  -------------------- 08  VDVHW --------------------
	//std::string nmeaVHW = "VDVHW,147.0,T,147.0,M,11.0,N,20.4,K";
	std::string nmeaVHW = "VDVHW,,,,,,,,";
	double speedInKnots;
	char knots;
	double speedInKmH;
	char kilometers;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseVHW(nmeaVHW, speedInKnots, knots, speedInKmH, kilometers);

	//  -------------------- 09  VDMTW --------------------
	//std::string nmeaMTW = "VDMTW,21.3,C";
	std::string nmeaMTW = "VDMTW,,";
	double degrees;
	char celcius;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseMTW(nmeaMTW, degrees, celcius);

	//  -------------------- 10  VDVBW --------------------
	//std::string nmeaVBW = "VDVBW,10.4,0.35,A,8.3,0.25,A";
	//std::string nmeaVBW = "VDVBW,15.0,02.4,A,15.0,04.6,A,00.0,A,06.8,A";
	std::string nmeaVBW = "VDVBW,,,,,,,,,,";
	double longitudinalWaterSpeed;
	double transverseWaterSpeed;
	char waterDataStatus;
	double longitudinalGroundSpeed;
	double transverseGroundSpeed;
	char groundDataStatus;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseVBW(nmeaVBW, longitudinalWaterSpeed, transverseWaterSpeed,
			waterDataStatus, longitudinalGroundSpeed, transverseGroundSpeed,
			groundDataStatus);

	//  -------------------- 11  VDVLW --------------------
	//std::string nmeaVLW = "VWVLW,0.225,N,0.238,A";
	std::string nmeaVLW = "VWVLW,,,,";
	double totalCumulativeDistance;
	char nauticalMiles1;
	double distanceSinceReset;
	char nauticalMiles2;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseVLW(nmeaVLW, totalCumulativeDistance, nauticalMiles1,
			distanceSinceReset, nauticalMiles2);

	//  -------------------- 12  SDDPT --------------------
	//std::string nmeaDPT = "SDDPT,1.8,2.6,1.3";
	std::string nmeaDPT = "SDDPT,,,";
	double waterDepthRelativeToTheTransducer;
	double offsetFromTransducer;
	double maximumRangeScaleInUse;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseDPT(nmeaDPT, waterDepthRelativeToTheTransducer,
			offsetFromTransducer, maximumRangeScaleInUse);

	//  -------------------- 13  SDDBT --------------------
	//std::string nmeaDBT = "SDDBT,05.9,f,01.8,M,01.0,F";
	std::string nmeaDBT = "SDDBT,,,,,,";
	double waterDepthInFeet;
	char feet;
	double waterDepthInMeters;
	char meters;
	double waterDepthInFathoms;
	char fathoms;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseDBT(nmeaDBT, waterDepthInFeet, feet, waterDepthInMeters,
			meters, waterDepthInFathoms, fathoms);

	//  -------------------- 14  SDDBK --------------------
	//std::string nmeaDBK = "SDDBK,05.9,f,01.8,M,01.0,F";
	std::string nmeaDBK = "SDDBK,,,,,,";
	double depthBelowKeelFeet;
	double depthBelowKeelMeters;
	double depthBelowKeelFathoms;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseDBK(nmeaDBK, depthBelowKeelFeet, feet,
			depthBelowKeelMeters, meters, depthBelowKeelFathoms, fathoms);

	//  -------------------- 15  PSKPDPT --------------------
	//std::string nmeaPSKPDP = "PSKPDPT,22.3,23.4,6.8,4,5,Fet";
	std::string nmeaPSKPDP = "PSKPDPT,,,,,,";
	int bottomEchoStrength;
	int echoSounderChannelNumber;
	std::string transducerLocation;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parsePSKPDPT(nmeaPSKPDP, waterDepthRelativeToTheTransducer,
			offsetFromTransducer, maximumRangeScaleInUse, bottomEchoStrength,
			echoSounderChannelNumber, transducerLocation);

	//  -------------------- 16  HCHDT --------------------
	//std::string nmeaHDT = "HCHDT,077.5,T";
	std::string nmeaHDT = "HCHDT,,";
	double headingDegreesTrue;
	char t;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseHDT(nmeaHDT, headingDegreesTrue, t);

	//  -------------------- 17  HCHDG --------------------
	//std::string nmeaHDG = "HCHDG,19.4,0.7,A,2.1,E";
	//std::string nmeaHDG = "HCHDG,77.0,00.9,E,00.5,W";
	std::string nmeaHDG = "HCHDG,,,,,";
	double magneticSensorHeadingInDegrees;
	double magneticDeviationDegrees;
	char magneticDeviationDirection;
	double magneticVariationDegrees;
	char magneticVariationDirection;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseHDG(nmeaHDG, magneticSensorHeadingInDegrees,
			magneticDeviationDegrees, magneticDeviationDirection,
			magneticVariationDegrees, magneticVariationDirection);

	//  -------------------- 18  HCHDM --------------------
	//std::string nmeaHDM = "HCHDM,77.9,M";
	std::string nmeaHDM = "HCHDM,,";
	double headingDegreesMagnetic;
	char magnetic;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseHDM(nmeaHDM, headingDegreesMagnetic, magnetic);

	//  -------------------- 19  TIROT --------------------
	//std::string nmeaROT = "TIROT,01.7,A";
	std::string nmeaROT = "TIROT,,";
	double rateOfTurn;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseROT(nmeaROT, rateOfTurn, status);

	//  -------------------- 20  IIMWV --------------------
	//std::string nmeaMWV = "IIMWV,004.2,T,007.2,N,A";
	std::string nmeaMWV = "IIMWV,,,,,";
	double windAngle;
	char reference;
	double windSpeed;
	char windSpeedUnits;
	char sensorStatus;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseMWV(nmeaMWV, windAngle, reference, windSpeed,
			windSpeedUnits, sensorStatus);

	//  -------------------- 21  WIMWD --------------------
	//std::string nmeaMWD = "WIMWD,004.2,T,004.2,M,007.2,N,003.7,M";
	std::string nmeaMWD = "WIMWD,,,,,,,,";
	double trueWindDirection;
	char truee;
	double magneticWindDirection;
	double windSpeedKnots;
	double windSpeedMeters;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseMWD(nmeaMWD, trueWindDirection, truee,
			magneticWindDirection, magnetic, windSpeedKnots, knots,
			windSpeedMeters, meters);

	//  -------------------- 22  IIXDR --------------------
	//std::string nmeaXDR = "IIXDR,C,25.4,C,TEMP,P,28.4,B,PRESS,H,27.4,P,RH";
	//std::string nmeaXDR = "IIXDR,,,a,,G,0.5,b,PRESS,G,2.1,,HUM";
	std::string nmeaXDR = "IIXDR,,,,,,,,,,,,";
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

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseXDR(nmeaXDR, typeOfSensorTemperature, temperatureReading,
			centigrade, nameOfTransducer, typeOfSensorPressure, pressureReading,
			bars, nameOfPressureSensor, typeOfSensorHumidity, humidity,
			humidityUnitsOfMeasurePercent, nameOfRelativeHumiditySensor);

	//  -------------------- 23  IITTD --------------------
	std::string nmeaTTD = "IITTD,FF,FF,9,abcdefghijklmn,4";
	//std::string nmeaTTD = "IITTD,,,,,";
	int aaa;
	int bbb;
	int ccc;
	std::string ddd;
	int eee;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseTTD(nmeaTTD, aaa, bbb, ccc, ddd, eee);

//	//  -------------------- 24  RATLB --------------------
//	//std::string nmeaTLB = "RATLB,5.5,cinco,1.1,uno,2.2,dos,3.3,tres,4.4,";
//	std::string nmeaTLB = "RATLB,,,,,,,,,,";
//	//std::string nmeaTLB = "RATLB,01,ENEMIGO";
//	std::vector<std::pair<double, std::string>> revenue;

//	//BOOST_REQUIRE_NO_THROW(
//	NmeaParser::parseTLB(nmeaTLB, revenue);

	//  -------------------- 25  RAOSD --------------------
	//std::string nmeaOSD = "RAOSD,154.3,A,16.4,a,16.5,a,16.6,12.3,a";
	std::string nmeaOSD = "RAOSD,,,,,,,,,";
	double heading;
	double vesselCourse;
	char referenceCourse;
	double vesselSpeed;
	char referenceSpeed;
	double vesselSet;
	double vesselDrift;
	char speedUnits;

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseOSD(nmeaOSD, heading, status, vesselCourse,
			referenceCourse, vesselSpeed, referenceSpeed, vesselSet,
			vesselDrift, speedUnits);

	//  -------------------- 26  RARSD --------------------
	//std::string nmeaRSD =
			//"RARSD,4.1,4.2,4.3,4.4,4.5,4.6,4.7,4.8,4.9,4.10,4.11,N,C";
	std::string nmeaRSD =
			"RARSD,,,,,,,,,,,,,";
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

	//BOOST_REQUIRE_NO_THROW(
	NmeaParser::parseRSD(nmeaRSD, origin1Range, origin1BearingDegrees,
			variableRangeMarker1, bearingLine1, origin2Range, origin2Bearing,
			vrm2, ebl2, cursorRange, cursorBearing, rangeScale, rangeUnits,
			displayRotation);

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

}

//BOOST_AUTO_TEST_CASE( parseGLL )
//{
//
//
//}
//
//BOOST_AUTO_TEST_CASE( parseGGA )
//{
//	// COPIAR EL EJEMPLO
//
//}
