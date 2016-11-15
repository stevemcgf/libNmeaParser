/*
 * NmeaEnums.h
 *
 *  Created on: Feb 13, 2016
 *      Author: steve
 */

#ifndef SRC_NMEAENUMS_H_
#define SRC_NMEAENUMS_H_

enum Nmea_GPSQualityIndicator {
	Nmea_GPSQualityIndicator_FixNotValid,
	Nmea_GPSQualityIndicator_GPSFix,
	Nmea_GPSQualityIndicator_GPSFixDifferential,
	Nmea_GPSQualityIndicator_RealTimeKinematic,
	Nmea_GPSQualityIndicator_RealTimeKinematicOmniStar
};

enum Nmea_SpeedDistanceUnits {
	Nmea_SpeedDistanceUnits_Kph_Kilometers,
	Nmea_SpeedDistanceUnits_Mps_Meters,
	Nmea_SpeedDistanceUnits_Knots_NauticalMiles
};

enum Nmea_TargetStatus {
	Nmea_TargetStatus_Lost,
	Nmea_TargetStatus_Query,
	Nmea_TargetStatus_Tracking
};

enum Nmea_TypeOfAcquisition {
	Nmea_TypeOfAcquisition_Automatic,
	Nmea_TypeOfAcquisition_Manual,
	Nmea_TypeOfAcquisition_Reported
};

enum Nmea_AngleReference {
	Nmea_AngleReference_True,
	Nmea_AngleReference_Relative
};

#endif /* SRC_NMEAENUMS_H_ */
