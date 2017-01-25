/*
 * test.cpp
 *
 *  Created on: Mar 2, 2016
 *      Author: steve
 */

#define BOOST_TEST_MODULE libNmeaParser test
#include <boost/test/included/unit_test.hpp>
#include "NmeaParser.h"

//int main() {

//  ----------------------------------------- 01  GPZDA -----------------------------------------
BOOST_AUTO_TEST_CASE( parseZDA ) {
	std::string nmeaZDA;

	boost::posix_time::time_duration mtime;
	int day;
	int month;
	int year;
	int localZoneHours;
	int localZoneMinutes;

	nmeaZDA = "$GPZDA,160619.00,20,04,2016,8,3*6C";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseZDA(nmeaZDA, mtime, day, month, year,
					localZoneHours, localZoneMinutes), 0UL);

	nmeaZDA = "$GPZDA,,20,04,2016,8,3*6C";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseZDA(nmeaZDA, mtime, day, month, year,
					localZoneHours, localZoneMinutes), 0b0000000000000001);

	nmeaZDA = "$GPZDA,,,,,,*19";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseZDA(nmeaZDA, mtime, day, month, year,
					localZoneHours, localZoneMinutes), 0b0000000000111111);
}

//  ----------------------------------------- 02  GPGLL -----------------------------------------
BOOST_AUTO_TEST_CASE( parseGLL ) {
	std::string nmeaGLL;

	double latitude;
	double longitude;
	boost::posix_time::time_duration mtime;
	char status;
	char modeIndicator;

	nmeaGLL = "$GPGLL,1202.5313138,S,07708.5464255,W,155730.00,A,A*65";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseGLL(nmeaGLL, latitude, longitude, mtime, status,
					modeIndicator), 0UL);

	nmeaGLL = "$GPGLL,1151.08,S,07718.65,W,165700,A*26";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseGLL(nmeaGLL, latitude, longitude, mtime, status,
					modeIndicator), 0b0000000000010000);

	nmeaGLL = "$GPGLL,,,,,,,*19";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseGLL(nmeaGLL, latitude, longitude, mtime, status,
					modeIndicator), 0b0000000000011111);

}

//  ----------------------------------------- 03  GPGGA -----------------------------------------
BOOST_AUTO_TEST_CASE( parseGGA ) {
	std::string nmeaGGA;

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

	nmeaGGA =
			"$GPGGA,172814.0,3723.46587704,N,12202.26957864,W,4,6,1.2,18.893,M,-25.669,M,2.5,0031*19";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseGGA(nmeaGGA, mtime, latitude, longitude, quality,
					numSV, hdop, orthometricheight, geoidseparation, agediffgps,
					refid), 0UL);

	nmeaGGA =
			"$GPGGA,165702,1151.0742,S,07718.6472,W,1,09,00.9,24.9,M,10.6,M,,*49";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseGGA(nmeaGGA, mtime, latitude, longitude, quality,
					numSV, hdop, orthometricheight, geoidseparation, agediffgps,
					refid), 0b0000001100000000);

}

//  ----------------------------------------- 04  GPVTG -----------------------------------------
BOOST_AUTO_TEST_CASE( parseVTG ) {
	std::string nmeaVTG;

	//std::string nmeaVTG = "$GPVTG,,,,,,,,,*19";
	double coursetrue;
	double coursemagnetic;
	double speedknots;
	double speedkph;

	nmeaVTG = "$GPVTG,8.86,T,10.29,M,0.02,N,0.04,K,A*19";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseVTG(nmeaVTG, coursetrue, coursemagnetic,
					speedknots, speedkph), 0UL);

	nmeaVTG = "$GPVTG,212,T,215,M,12.8,N,23.8,K*4B";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseVTG(nmeaVTG, coursetrue, coursemagnetic,
					speedknots, speedkph), 0UL);

	nmeaVTG = "$GPVTG,044,T,044,M,00.0,N,,*1B";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseVTG(nmeaVTG, coursetrue, coursemagnetic,
					speedknots, speedkph), 0b0000000000001000);

}

//  ----------------------------------------- 05  GPRMC -----------------------------------------
BOOST_AUTO_TEST_CASE( parseRMC ) {
	std::string nmeaRMC =
			"$GPRMC,160618.00,A,1202.5313983,S,07708.5478298,W,0.10,166.87,200416,1.4,W,A,S*56";
	//std::string nmeaRMC = "$GPRMC,,,,,,,,,,,,,*56";

	boost::posix_time::time_duration mtime;
	double latitude;
	double longitude;
	double speedknots;
	double coursetrue;
	boost::gregorian::date mdate;
	double magneticvar;

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseRMC(nmeaRMC, mtime, latitude, longitude,
					speedknots, coursetrue, mdate, magneticvar), 0UL);
}

//  ----------------------------------------- 06  GPWPL -----------------------------------------
BOOST_AUTO_TEST_CASE( parseWPL ) {
	std::string nmeaWPL = "$GPWPL,4917.16,N,12310.64,W,003*19";
	//std::string nmeaWPL = "$GPWPL,,,,,*19";
	double latitude;
	double longitude;
	std::string waypointName;

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseWPL(nmeaWPL, latitude, longitude, waypointName),
			0UL);
}

//  ----------------------------------------- 07  GPRTE -----------------------------------------
BOOST_AUTO_TEST_CASE( parseRTE ) {
	//std::string nmeaRTE = "$GPRTE,3,3,c,WPT4,RTE-A,RTE-B,RTE-C,RTE-D,RTE-E,RTE-F*19";
	std::string nmeaRTE = "$GPRTE,1,1,w,DAVID,JAVIER,VICTOR*19";
	//std::string nmeaRTE = "$GPRTE,,,,,,,,,,*19";
	int messagesTransmitted;
	int messageNumber;
	char messageMode;
	std::string routeName;
	std::vector<std::string> waypointNames;

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseRTE(nmeaRTE, messagesTransmitted, messageNumber,
					messageMode, routeName, waypointNames), 0UL);
}

//  ----------------------------------------- 08  VDVHW -----------------------------------------
BOOST_AUTO_TEST_CASE( parseVHW ) {
	std::string nmeaVHW = "$VDVHW,147.0,T,147.0,M,11.0,N,20.4,K*19";
	//std::string nmeaVHW = "$VDVHW,,,,,,,,*19";
	double speedInKnots;
	char knots;
	double speedInKmH;
	char kilometers;

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseVHW(nmeaVHW, speedInKnots, knots, speedInKmH,
					kilometers), 0UL);
}

//  ----------------------------------------- 09  VDMTW -----------------------------------------
BOOST_AUTO_TEST_CASE( parseMTW ) {
	std::string nmeaMTW = "$VDMTW,21.3,C*19";
	//std::string nmeaMTW = "$VDMTW,,*19";
	double degrees;
	char celcius;

	BOOST_REQUIRE_EQUAL(NmeaParser::parseMTW(nmeaMTW, degrees, celcius), 0UL);
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

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseVBW(nmeaVBW, longitudinalWaterSpeed,
					transverseWaterSpeed, waterDataStatus,
					longitudinalGroundSpeed, transverseGroundSpeed,
					groundDataStatus), 0UL);
}

//  ----------------------------------------- 11  VDVLW -----------------------------------------
BOOST_AUTO_TEST_CASE( parseVLW ) {
	std::string nmeaVLW = "$VWVLW,0.225,N,0.238,A*19";
	//std::string nmeaVLW = "$VWVLW,,,,*19";
	double totalCumulativeDistance;
	char nauticalMiles1;
	double distanceSinceReset;
	char nauticalMiles2;

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseVLW(nmeaVLW, totalCumulativeDistance,
					nauticalMiles1, distanceSinceReset, nauticalMiles2), 0UL);
}

//  ----------------------------------------- 12  SDDPT -----------------------------------------
BOOST_AUTO_TEST_CASE( parseDPT ) {
	std::string nmeaDPT = "$SDDPT,1.8,2.6,1.3*19";
	//std::string nmeaDPT = "$SDDPT,,,*19";
	double waterDepthRelativeToTheTransducer;
	double offsetFromTransducer;
	double maximumRangeScaleInUse;

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseDPT(nmeaDPT, waterDepthRelativeToTheTransducer,
					offsetFromTransducer, maximumRangeScaleInUse), 0UL);
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

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseDBT(nmeaDBT, waterDepthInFeet, feet,
					waterDepthInMeters, meters, waterDepthInFathoms, fathoms),
			0UL);
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

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseDBK(nmeaDBK, depthBelowKeelFeet, feet,
					depthBelowKeelMeters, meters, depthBelowKeelFathoms,
					fathoms), 0UL);
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

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parsePSKPDPT(nmeaPSKPDP,
					waterDepthRelativeToTheTransducer, offsetFromTransducer,
					maximumRangeScaleInUse, bottomEchoStrength,
					echoSounderChannelNumber, transducerLocation), 0UL);
}

//  ----------------------------------------- 16  HCHDT -----------------------------------------
BOOST_AUTO_TEST_CASE( parseHDT ) {
	std::string nmeaHDT = "$HCHDT,077.5,T*19";
	//std::string nmeaHDT = "$HCHDT,,*19";
	double headingDegreesTrue;
	char t;

	BOOST_REQUIRE_EQUAL(NmeaParser::parseHDT(nmeaHDT, headingDegreesTrue, t),
			0UL);
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

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseHDG(nmeaHDG, magneticSensorHeadingInDegrees,
					magneticDeviationDegrees, magneticDeviationDirection,
					magneticVariationDegrees, magneticVariationDirection), 0UL);
}

//  ----------------------------------------- 18  HCHDM -----------------------------------------
BOOST_AUTO_TEST_CASE( parseHDM ) {
	std::string nmeaHDM = "$HCHDM,77.9,M*19";
	//std::string nmeaHDM = "$HCHDM,,*19";
	double headingDegreesMagnetic;
	char magnetic;

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseHDM(nmeaHDM, headingDegreesMagnetic, magnetic),
			0UL);
}

//  ----------------------------------------- 19  TIROT -----------------------------------------
BOOST_AUTO_TEST_CASE( parseROT ) {
	std::string nmeaROT = "$TIROT,01.7,A*19";
	//std::string nmeaROT = "$TIROT,,*19";
	double rateOfTurn;
	char status;

	BOOST_REQUIRE_EQUAL(NmeaParser::parseROT(nmeaROT, rateOfTurn, status), 0UL);
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

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseMWV(nmeaMWV, windAngle, reference, windSpeed,
					windSpeedUnits, sensorStatus), 0UL);
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

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseMWD(nmeaMWD, trueWindDirection, truee,
					magneticWindDirection, magnetic, windSpeedKnots, knots,
					windSpeedMeters, meters), 0UL);
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

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseXDR(nmeaXDR, typeOfSensorTemperature,
					temperatureReading, centigrade, nameOfTransducer,
					typeOfSensorPressure, pressureReading, bars,
					nameOfPressureSensor, typeOfSensorHumidity, humidity,
					humidityUnitsOfMeasurePercent,
					nameOfRelativeHumiditySensor), 0UL);
}

//  ----------------------------------------- 23  IITTD -----------------------------------------
BOOST_AUTO_TEST_CASE( parseTTD ) {
	//  -------------------- 23  IITTD --------------------
	std::string nmeaTTD = "!INTTD,01,01,,0PP10Eg@wwP74@0,0*2F";
	int totalLines;
	int lineCount;
	int sequenceIdentifier;
	std::string trackData;
	int fillBits;

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseTTD(nmeaTTD, totalLines, lineCount,
					sequenceIdentifier, trackData, fillBits),
			0b0000000000000100);
}

//  ----------------------------------------- 24  RATLB -----------------------------------------
BOOST_AUTO_TEST_CASE( parseTLB ) {
	std::string nmeaTLB;

	//std::string nmeaTLB = "$INTLB,81,,17,,10,cauti*39";
	//std::string nmeaTLB = "$RATLB,01,uno*19";
	std::vector<std::pair<int, std::string>> trackPair;

	nmeaTLB = "$RATLB,5,cinco,1,uno,2,dos,3,tres,4,cuatro*19";
	BOOST_REQUIRE_EQUAL(NmeaParser::parseTLB(nmeaTLB, trackPair), 0UL);
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

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseOSD(nmeaOSD, heading, status, vesselCourse,
					referenceCourse, vesselSpeed, referenceSpeed, vesselSet,
					vesselDrift, speedUnits), 0UL);

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

	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseRSD(nmeaRSD, origin1Range, origin1BearingDegrees,
					variableRangeMarker1, bearingLine1, origin2Range,
					origin2Bearing, vrm2, ebl2, cursorRange, cursorBearing,
					rangeScale, rangeUnits, displayRotation), 0UL);
}

//  ----------------------------------------- 26  RATTM -----------------------------------------
BOOST_AUTO_TEST_CASE( parseTTM ) {
	std::string nmeaTTM;

	int targetNumber;
	double targetDistance;
	double targetBearing;
	Nmea_AngleReference targetBearingReference;
	double targetSpeed;
	double targetCourse;
	Nmea_AngleReference targetCourseReference;
	Nmea_SpeedDistanceUnits speedDistanceUnits;
	std::string targetName;
	Nmea_TargetStatus targetStatus;
	boost::posix_time::time_duration timeOfData;
	Nmea_TypeOfAcquisition typeOfAcquisition;

	nmeaTTM = "$RATTM,04,2.18,247.8,T,0.5,350.0,T,2.1,51.0,N,,T,,171443,M*23";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseTTM(nmeaTTM, targetNumber, targetDistance,
					targetBearing, targetBearingReference, targetSpeed,
					targetCourse, targetCourseReference, speedDistanceUnits,
					targetName, targetStatus, timeOfData, typeOfAcquisition),
			0b0000000100000000);

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
//	//BOOST_REQUIRE_EQUAL(
//	NmeaParser::parseVDM(nmeaVDM, typeAIS, mmsiAIS, latitudeAIS, longitudeAIS,
//			speedAIS, courseAIS, headingAIS);

//}

BOOST_AUTO_TEST_CASE( parseTTDPayload ) {
	std::string trackData = "0PP10Eg@wwP74@0";

	std::vector<NmeaTrackData> tracks;

	BOOST_REQUIRE_EQUAL(NmeaParser::parseTTDPayload(trackData, tracks), true);
}

BOOST_AUTO_TEST_CASE( parseAISMessageType ) {

	std::string encodedData;
	Nmea_AisMessageType messageType;

	encodedData = "B6K8C4P006Wf1lNAijT03wt7kP06";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseAISMessageType(encodedData, messageType), true);

	encodedData = "H6K8C4Q<Dq<QF0l59F0pvs>2220";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseAISMessageType(encodedData, messageType), true);

	encodedData =
			"5;Djh9P2=K8@t7;37<0<4t00000000000000000l1P=554Kh0=j0DPSmD`855Ah00000000";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseAISMessageType(encodedData, messageType), true);

	encodedData = "D2C30K41DUH8880";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseAISMessageType(encodedData, messageType), true);

}

BOOST_AUTO_TEST_CASE( parseAISPositionReportClassA ) {

	std::string encodedData;

	AISPositionReportClassA data;

	encodedData = "3;DjhdPP@3JNfEIq6uHjlUCp00w1";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseAISPositionReportClassA(encodedData, data), true);

}

BOOST_AUTO_TEST_CASE( parseAISBaseStationReport ) {

	std::string encodedData;

	AISBaseStationReport data;

	encodedData = "400TcdiuiT7VDR>3nIfr6>i00000";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseAISBaseStationReport(encodedData, data), true);

}

BOOST_AUTO_TEST_CASE( parseAISStaticAndVoyageRelatedData ) {

	std::string encodedData;

	AISStaticAndVoyageRelatedData data;

	encodedData =
			"58wt8Ui`g??r21`7S=:22058<v05Htp000000015>8OA;0skeQ8823mDm3kP00000000000";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseAISStaticAndVoyageRelatedData(encodedData, data),
			true);

}

BOOST_AUTO_TEST_CASE( parseAISStandardClassBCSPositionReport ) {

	std::string encodedData;

	AISStandardClassBCSPositionReport data;

	encodedData = "B;Djf2h01fWd0qNAh;M0cwb7kP06";
	BOOST_REQUIRE_EQUAL(
			NmeaParser::parseAISStandardClassBCSPositionReport(encodedData,
					data), true);

}

BOOST_AUTO_TEST_CASE( parseAISStaticDataReport ) {

	std::string encodedData;

	AISStaticDataReport data;

	encodedData = "H6K8C4Q<Dq<QF0l59F0pvs>2220";
	BOOST_REQUIRE_EQUAL(NmeaParser::parseAISStaticDataReport(encodedData, data),
			true);

}
