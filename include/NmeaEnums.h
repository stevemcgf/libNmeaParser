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

enum Nmea_TrackStatus {
	Nmea_TrackStatus_Non_tracking,
	Nmea_TrackStatus_Acquiring,
	Nmea_TrackStatus_Lost,
	Nmea_TrackStatus_Reserved_1,
	Nmea_TrackStatus_Tracking,
	Nmea_TrackStatus_Reserved_2,
	Nmea_TrackStatus_Tracking_CPA_Alarm,
	Nmea_TrackStatus_Tracking_CPA_Alarm_Ack
};

enum Nmea_Operation {
	Nmea_Operation_Autonomous,
	Nmea_Operation_TestTarget
};

enum Nmea_SpeedMode {
	Nmea_SpeedMode_TrueSpeedCourse,
	Nmea_SpeedMode_Relative
};

enum Nmea_StabilisationMode {
	Nmea_StabilisationMode_OverGround,
	Nmea_StabilisationMode_ThroughWater
};

struct NmeaTrackData {
	int targetNumber;
	float trueBearing;
	float speed;
	float course;
	float aisHeading;
	Nmea_TrackStatus status;
	Nmea_Operation operation;
	float distance;
	Nmea_SpeedMode speedMode;
	Nmea_StabilisationMode stabilisationMode;
	int correlationNumber;
};

#endif /* SRC_NMEAENUMS_H_ */
