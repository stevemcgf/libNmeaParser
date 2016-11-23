/*
 * NmeaEnums.h
 *
 *  Created on: Feb 13, 2016
 *      Author: steve
 */

#ifndef SRC_NMEAENUMS_H_
#define SRC_NMEAENUMS_H_

#include <iostream>

enum Nmea_GPSQualityIndicator {
	Nmea_GPSQualityIndicator_FixNotValid,
	Nmea_GPSQualityIndicator_GPSFix,
	Nmea_GPSQualityIndicator_GPSFixDifferential,
	Nmea_GPSQualityIndicator_RealTimeKinematic,
	Nmea_GPSQualityIndicator_RealTimeKinematicOmniStar
};
std::ostream& operator<< (std::ostream & out, Nmea_GPSQualityIndicator val);

enum Nmea_SpeedDistanceUnits {
	Nmea_SpeedDistanceUnits_Kph_Kilometers,
	Nmea_SpeedDistanceUnits_Mps_Meters,
	Nmea_SpeedDistanceUnits_Knots_NauticalMiles
};
std::ostream& operator<< (std::ostream & out, Nmea_SpeedDistanceUnits val);

enum Nmea_TargetStatus {
	Nmea_TargetStatus_Lost,
	Nmea_TargetStatus_Query,
	Nmea_TargetStatus_Tracking
};
std::ostream& operator<< (std::ostream & out, Nmea_TargetStatus val);

enum Nmea_TypeOfAcquisition {
	Nmea_TypeOfAcquisition_Automatic,
	Nmea_TypeOfAcquisition_Manual,
	Nmea_TypeOfAcquisition_Reported
};
std::ostream& operator<< (std::ostream & out, Nmea_TypeOfAcquisition val);

enum Nmea_AngleReference {
	Nmea_AngleReference_True,
	Nmea_AngleReference_Relative
};
std::ostream& operator<< (std::ostream & out, Nmea_AngleReference val);

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
std::ostream& operator<< (std::ostream & out, Nmea_TrackStatus val);

enum Nmea_Operation {
	Nmea_Operation_Autonomous,
	Nmea_Operation_TestTarget
};
std::ostream& operator<< (std::ostream & out, Nmea_Operation val);

enum Nmea_SpeedMode {
	Nmea_SpeedMode_TrueSpeedCourse,
	Nmea_SpeedMode_Relative
};
std::ostream& operator<< (std::ostream & out, Nmea_SpeedMode val);

enum Nmea_StabilisationMode {
	Nmea_StabilisationMode_OverGround,
	Nmea_StabilisationMode_ThroughWater
};
std::ostream& operator<< (std::ostream & out, Nmea_StabilisationMode val);

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

enum Nmea_AisMessageType {
	Nmea_AisMessageType_NA,
	Nmea_AisMessageType_PositionReportClassA,
	Nmea_AisMessageType_PositionReportClassA_AssignedSchedule,
	Nmea_AisMessageType_PositionReportClassA_ResponseToInterrogation,
	Nmea_AisMessageType_BaseStationReport,
	Nmea_AisMessageType_StaticAndVoyageRelatedData,
	Nmea_AisMessageType_BinaryAddressedMessage,
	Nmea_AisMessageType_BinaryAcknowledge,
	Nmea_AisMessageType_BinaryBroadcastMessage,
	Nmea_AisMessageType_StandardSARAircraftPositionReport,
	Nmea_AisMessageType_UTCAndDateInquiry,
	Nmea_AisMessageType_UTCAndDateResponse,
	Nmea_AisMessageType_AddressedSafetyRelatedMessage,
	Nmea_AisMessageType_SafetyRelatedAcknowledgement,
	Nmea_AisMessageType_SafetyRelatedBroadcastMessage,
	Nmea_AisMessageType_Interrogation,
	Nmea_AisMessageType_AssignmentModeCommand,
	Nmea_AisMessageType_DGNSSBinaryBroadcastMessage,
	Nmea_AisMessageType_StandardClassBCSPositionReport,
	Nmea_AisMessageType_ExtendedClassBEquipmentPositionReport,
	Nmea_AisMessageType_DataLinkManagement,
	Nmea_AisMessageType_AidToNavigationReport,
	Nmea_AisMessageType_ChannelManagement,
	Nmea_AisMessageType_GroupAssignmentCommand,
	Nmea_AisMessageType_StaticDataReport,
	Nmea_AisMessageType_SingleSlotBinaryMessage,
	Nmea_AisMessageType_MultipleSlotBinaryMessageWithCommunicationsState,
	Nmea_AisMessageType_PositionReportForLongRangeApplications
};
std::ostream& operator<<(std::ostream & out, Nmea_AisMessageType val);

enum Nmea_NavigationStatus {
	Nmea_NavigationStatus_UnderWayUsingEngine,
	Nmea_NavigationStatus_AtAnchor,
	Nmea_NavigationStatus_NotUnderCommand,
	Nmea_NavigationStatus_RestrictedManoeuverability,
	Nmea_NavigationStatus_ConstrainedByHerDraught,
	Nmea_NavigationStatus_Moored,
	Nmea_NavigationStatus_Aground,
	Nmea_NavigationStatus_EngagedInFishing,
	Nmea_NavigationStatus_UnderWaySailing,
	Nmea_NavigationStatus_Reserved_HSC,
	Nmea_NavigationStatus_Reserved_WIG,
	Nmea_NavigationStatus_Reserved1,
	Nmea_NavigationStatus_Reserved2,
	Nmea_NavigationStatus_Reserved3,
	Nmea_NavigationStatus_AIS_SART,
	Nmea_NavigationStatus_NotDefined
};
std::ostream& operator<<(std::ostream & out, Nmea_NavigationStatus val);

enum Nmea_PositionAccuracy {
	Nmea_PositionAccuracy_UnaugmentedGNSSFix,
	Nmea_PositionAccuracy_DGPSQualityFix
};
std::ostream& operator<<(std::ostream & out, Nmea_PositionAccuracy val);

enum Nmea_ManeuverIndicator {
	Nmea_ManeuverIndicator_NotAvailable,
	Nmea_ManeuverIndicator_NoSpecialManeuver,
	Nmea_ManeuverIndicator_SpecialManeuver
};
std::ostream& operator<<(std::ostream & out, Nmea_ManeuverIndicator val);

enum Nmea_RAIM {
	Nmea_RAIM_NotInUse,
	Nmea_RAIM_InUse
};
std::ostream& operator<<(std::ostream & out, Nmea_RAIM val);

struct AISPositionReportClassA {
	int repeatIndicator;
	uint mmsi;
	Nmea_NavigationStatus navigationStatus;
	float rateOfTurn;
	float speedOverGround;
	Nmea_PositionAccuracy positionAccuracy;
	float longitude;
	float latitude;
	float courseOverGround;
	uint trueHeading;
	uint timestapUTCSecond;
	Nmea_ManeuverIndicator maneuverIndicator;
	Nmea_RAIM raim;
};

#endif /* SRC_NMEAENUMS_H_ */
