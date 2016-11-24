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

enum Nmea_EPFDFix {
	Nmea_EPFDFix_Undefined,
	Nmea_EPFDFix_GPS,
	Nmea_EPFDFix_GLONASS,
	Nmea_EPFDFix_CombinedGPSGLONASS,
	Nmea_EPFDFix_LoranC,
	Nmea_EPFDFix_Chayka,
	Nmea_EPFDFix_IntegratedNavigationSystem,
	Nmea_EPFDFix_Surveyed,
	Nmea_EPFDFix_Galileo
};
std::ostream& operator<<(std::ostream & out, Nmea_EPFDFix val);

enum Nmea_ShipType {
	Nmea_ShipType_NotAvailable,
	Nmea_ShipType_Reserved1,
	Nmea_ShipType_Reserved2,
	Nmea_ShipType_Reserved3,
	Nmea_ShipType_Reserved4,
	Nmea_ShipType_Reserved5,
	Nmea_ShipType_Reserved6,
	Nmea_ShipType_Reserved7,
	Nmea_ShipType_Reserved8,
	Nmea_ShipType_Reserved9,
	Nmea_ShipType_Reserved10,
	Nmea_ShipType_Reserved11,
	Nmea_ShipType_Reserved12,
	Nmea_ShipType_Reserved13,
	Nmea_ShipType_Reserved14,
	Nmea_ShipType_Reserved15,
	Nmea_ShipType_Reserved16,
	Nmea_ShipType_Reserved17,
	Nmea_ShipType_Reserved18,
	Nmea_ShipType_Reserved19,
	Nmea_ShipType_WingInGround_WIG_AllShipsOfThisType,
	Nmea_ShipType_WingInGround_WIG_HazardousCategoryA,
	Nmea_ShipType_WingInGround_WIG_HazardousCategoryB,
	Nmea_ShipType_WingInGround_WIG_HazardousCategoryC,
	Nmea_ShipType_WingInGround_WIG_HazardousCategoryD,
	Nmea_ShipType_WingInGround_WIG_Reserved1,
	Nmea_ShipType_WingInGround_WIG_Reserved2,
	Nmea_ShipType_WingInGround_WIG_Reserved3,
	Nmea_ShipType_WingInGround_WIG_Reserved4,
	Nmea_ShipType_WingInGround_WIG_Reserved5,
	Nmea_ShipType_Fishing,
	Nmea_ShipType_Towing,
	Nmea_ShipType_Towing_LengthExceeds200mOrBreadthExceeds25m,
	Nmea_ShipType_DredgingOrUnderwaterOps,
	Nmea_ShipType_DivingOps,
	Nmea_ShipType_MilitaryOps,
	Nmea_ShipType_Sailing,
	Nmea_ShipType_PleasureCraft,
	Nmea_ShipType_Reserved_1,
	Nmea_ShipType_Reserved_2,
	Nmea_ShipType_HighSpeedCraft_HSC_AllShipsOfThisType,
	Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryA,
	Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryB,
	Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryC,
	Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryD,
	Nmea_ShipType_HighSpeedCraft_HSC_Reserved1,
	Nmea_ShipType_HighSpeedCraft_HSC_Reserved2,
	Nmea_ShipType_HighSpeedCraft_HSC_Reserved3,
	Nmea_ShipType_HighSpeedCraft_HSC_Reserved4,
	Nmea_ShipType_HighSpeedCraft_HSC_NoAdditionalInformation,
	Nmea_ShipType_PilotVessel,
	Nmea_ShipType_SearchAndRescueVessel,
	Nmea_ShipType_Tug,
	Nmea_ShipType_PortTender,
	Nmea_ShipType_AntiPollutionEquipment,
	Nmea_ShipType_LawEnforcement,
	Nmea_ShipType_SpareLocalVessel1,
	Nmea_ShipType_SpareLocalVessel2,
	Nmea_ShipType_MedicalTransport,
	Nmea_ShipType_NoncombatantShipAccordingToRR,
	Nmea_ShipType_Passenger_AllShipsOfThisType,
	Nmea_ShipType_Passenger_HazardousCategoryA,
	Nmea_ShipType_Passenger_HazardousCategoryB,
	Nmea_ShipType_Passenger_HazardousCategoryC,
	Nmea_ShipType_Passenger_HazardousCategoryD,
	Nmea_ShipType_Passenger_Reserved1,
	Nmea_ShipType_Passenger_Reserved2,
	Nmea_ShipType_Passenger_Reserved3,
	Nmea_ShipType_Passenger_Reserved4,
	Nmea_ShipType_Passenger_NoAdditionalInformation,
	Nmea_ShipType_Cargo_AllShipsOfThisType,
	Nmea_ShipType_Cargo_HazardousCategoryA,
	Nmea_ShipType_Cargo_HazardousCategoryB,
	Nmea_ShipType_Cargo_HazardousCategoryC,
	Nmea_ShipType_Cargo_HazardousCategoryD,
	Nmea_ShipType_Cargo_Reserved1,
	Nmea_ShipType_Cargo_Reserved2,
	Nmea_ShipType_Cargo_Reserved3,
	Nmea_ShipType_Cargo_Reserved4,
	Nmea_ShipType_Cargo_NoAdditionalInformation,
	Nmea_ShipType_Tanker_AllShipsOfThisType,
	Nmea_ShipType_Tanker_HazardousCategoryA,
	Nmea_ShipType_Tanker_HazardousCategoryB,
	Nmea_ShipType_Tanker_HazardousCategoryC,
	Nmea_ShipType_Tanker_HazardousCategoryD,
	Nmea_ShipType_Tanker_Reserved1,
	Nmea_ShipType_Tanker_Reserved2,
	Nmea_ShipType_Tanker_Reserved3,
	Nmea_ShipType_Tanker_Reserved4,
	Nmea_ShipType_Tanker_NoAdditionalInformation,
	Nmea_ShipType_OtherType_AllShipsOfThisType,
	Nmea_ShipType_OtherType_HazardousCategoryA,
	Nmea_ShipType_OtherType_HazardousCategoryB,
	Nmea_ShipType_OtherType_HazardousCategoryC,
	Nmea_ShipType_OtherType_HazardousCategoryD,
	Nmea_ShipType_OtherType_Reserved1,
	Nmea_ShipType_OtherType_Reserved2,
	Nmea_ShipType_OtherType_Reserved3,
	Nmea_ShipType_OtherType_Reserved4,
	Nmea_ShipType_OtherType_NoAdditionalInformation
};
std::ostream& operator<<(std::ostream & out, Nmea_ShipType val);

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

struct AISBaseStationReport {
	int repeatIndicator;
	uint mmsi;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	Nmea_PositionAccuracy positionAccuracy;
	float longitude;
	float latitude;
	Nmea_EPFDFix epfd;
	Nmea_RAIM raim;
};

struct AISStaticAndVoyageRelatedData {
	int repeatIndicator;
	uint mmsi;
	int aisVersion;
	int imoNumber;
	std::string callsign;
	std::string vesselName;
	Nmea_ShipType shipType;
	int dimensionToBow;
	int dimensionToStern;
	int dimensionToPort;
	int dimensionToStarboard;
	Nmea_EPFDFix epfd;
	int month;
	int day;
	int hour;
	int minute;
	float draught;
	std::string destination;
};

struct AISStandardClassBCSPositionReport {
	int repeatIndicator;
	uint mmsi;
	float speedOverGround;
	Nmea_PositionAccuracy positionAccuracy;
	float longitude;
	float latitude;
	float courseOverGround;
	uint trueHeading;
	uint timestapUTCSecond;
};

struct AISStaticDataReport {
	int repeatIndicator;
	uint mmsi;
	int partNumber;

	struct {
		std::string vesselName;
	} partA;

	struct {
		Nmea_ShipType shipType;
		std::string vendorId;
		int unitModelCode;
		int serialNumber;
		std::string callsign;
		int dimensionToBow;
		int dimensionToStern;
		int dimensionToPort;
		int dimensionToStarboard;
		uint mothershipmmsi;
	} partB;
};

#endif /* SRC_NMEAENUMS_H_ */
