/*
 * NmeaEnums.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: steve
 */

#include "NmeaEnums.h"

std::ostream& operator<< (std::ostream & out, Nmea_GPSQualityIndicator val)
{
	switch (val) {
	case Nmea_GPSQualityIndicator_FixNotValid:
		return out << "Nmea_GPSQualityIndicator_FixNotValid";
	case Nmea_GPSQualityIndicator_GPSFix:
		return out << "Nmea_GPSQualityIndicator_GPSFix";
	case Nmea_GPSQualityIndicator_GPSFixDifferential:
		return out << "Nmea_GPSQualityIndicator_GPSFixDifferential";
	case Nmea_GPSQualityIndicator_RealTimeKinematic:
		return out << "Nmea_GPSQualityIndicator_RealTimeKinematic";
	case Nmea_GPSQualityIndicator_RealTimeKinematicOmniStar:
		return out << "Nmea_GPSQualityIndicator_RealTimeKinematicOmniStar";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<< (std::ostream & out, Nmea_SpeedDistanceUnits val)
{
	switch (val) {
	case Nmea_SpeedDistanceUnits_Kph_Kilometers:
		return out << "Nmea_SpeedDistanceUnits_Kph_Kilometers";
	case Nmea_SpeedDistanceUnits_Mps_Meters:
		return out << "Nmea_SpeedDistanceUnits_Mps_Meters";
	case Nmea_SpeedDistanceUnits_Knots_NauticalMiles:
		return out << "Nmea_SpeedDistanceUnits_Knots_NauticalMiles";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<< (std::ostream & out, Nmea_TargetStatus val)
{
	switch (val) {
	case Nmea_TargetStatus_Lost:
		return out << "Nmea_TargetStatus_Lost";
	case Nmea_TargetStatus_Query:
		return out << "Nmea_TargetStatus_Query";
	case Nmea_TargetStatus_Tracking:
		return out << "Nmea_TargetStatus_Tracking";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<< (std::ostream & out, Nmea_TypeOfAcquisition val)
{
	switch (val) {
	case Nmea_TypeOfAcquisition_Automatic:
		return out << "Nmea_TypeOfAcquisition_Automatic";
	case Nmea_TypeOfAcquisition_Manual:
		return out << "Nmea_TypeOfAcquisition_Manual";
	case Nmea_TypeOfAcquisition_Reported:
		return out << "Nmea_TypeOfAcquisition_Reported";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<< (std::ostream & out, Nmea_AngleReference val)
{
	switch (val) {
	case Nmea_AngleReference_True:
		return out << "Nmea_AngleReference_True";
	case Nmea_AngleReference_Relative:
		return out << "Nmea_AngleReference_Relative";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<< (std::ostream & out, Nmea_TrackStatus val)
{
	switch (val) {
	case Nmea_TrackStatus_Non_tracking:
		return out << "Nmea_TrackStatus_Non_tracking";
	case Nmea_TrackStatus_Acquiring:
		return out << "Nmea_TrackStatus_Acquiring";
	case Nmea_TrackStatus_Lost:
		return out << "Nmea_TrackStatus_Lost";
	case Nmea_TrackStatus_Reserved_1:
		return out << "Nmea_TrackStatus_Reserved_1";
	case Nmea_TrackStatus_Tracking:
		return out << "Nmea_TrackStatus_Tracking";
	case Nmea_TrackStatus_Reserved_2:
		return out << "Nmea_TrackStatus_Reserved_2";
	case Nmea_TrackStatus_Tracking_CPA_Alarm:
		return out << "Nmea_TrackStatus_Tracking_CPA_Alarm";
	case Nmea_TrackStatus_Tracking_CPA_Alarm_Ack:
		return out << "Nmea_TrackStatus_Tracking_CPA_Alarm_Ack";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<< (std::ostream & out, Nmea_Operation val)
{
	switch (val) {
	case Nmea_Operation_Autonomous:
		return out << "Nmea_Operation_Autonomous";
	case Nmea_Operation_TestTarget:
		return out << "Nmea_Operation_TestTarget";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<< (std::ostream & out, Nmea_SpeedMode val)
{
	switch (val) {
	case Nmea_SpeedMode_TrueSpeedCourse:
		return out << "Nmea_SpeedMode_TrueSpeedCourse";
	case Nmea_SpeedMode_Relative:
		return out << "Nmea_SpeedMode_Relative";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<< (std::ostream & out, Nmea_StabilisationMode val)
{
	switch (val) {
	case Nmea_StabilisationMode_OverGround:
		return out << "Nmea_StabilisationMode_OverGround";
	case Nmea_StabilisationMode_ThroughWater:
		return out << "Nmea_StabilisationMode_ThroughWater";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_AisMessageType val)
{
	switch (val) {
	case Nmea_AisMessageType_NA:
		return out << "Nmea_AisMessageType_NA";
	case Nmea_AisMessageType_PositionReportClassA:
		return out << "Nmea_AisMessageType_PositionReportClassA";
	case Nmea_AisMessageType_PositionReportClassA_AssignedSchedule:
		return out
				<< "Nmea_AisMessageType_PositionReportClassA_AssignedSchedule";
	case Nmea_AisMessageType_PositionReportClassA_ResponseToInterrogation:
		return out
				<< "Nmea_AisMessageType_PositionReportClassA_ResponseToInterrogation";
	case Nmea_AisMessageType_BaseStationReport:
		return out << "Nmea_AisMessageType_BaseStationReport";
	case Nmea_AisMessageType_StaticAndVoyageRelatedData:
		return out << "Nmea_AisMessageType_StaticAndVoyageRelatedData";
	case Nmea_AisMessageType_BinaryAddressedMessage:
		return out << "Nmea_AisMessageType_BinaryAddressedMessage";
	case Nmea_AisMessageType_BinaryAcknowledge:
		return out << "Nmea_AisMessageType_BinaryAcknowledge";
	case Nmea_AisMessageType_BinaryBroadcastMessage:
		return out << "Nmea_AisMessageType_BinaryBroadcastMessage";
	case Nmea_AisMessageType_StandardSARAircraftPositionReport:
		return out << "Nmea_AisMessageType_StandardSARAircraftPositionReport";
	case Nmea_AisMessageType_UTCAndDateInquiry:
		return out << "Nmea_AisMessageType_UTCAndDateInquiry";
	case Nmea_AisMessageType_UTCAndDateResponse:
		return out << "Nmea_AisMessageType_UTCAndDateResponse";
	case Nmea_AisMessageType_AddressedSafetyRelatedMessage:
		return out << "Nmea_AisMessageType_AddressedSafetyRelatedMessage";
	case Nmea_AisMessageType_SafetyRelatedAcknowledgement:
		return out << "Nmea_AisMessageType_SafetyRelatedAcknowledgement";
	case Nmea_AisMessageType_SafetyRelatedBroadcastMessage:
		return out << "Nmea_AisMessageType_SafetyRelatedBroadcastMessage";
	case Nmea_AisMessageType_Interrogation:
		return out << "Nmea_AisMessageType_Interrogation";
	case Nmea_AisMessageType_AssignmentModeCommand:
		return out << "Nmea_AisMessageType_AssignmentModeCommand";
	case Nmea_AisMessageType_DGNSSBinaryBroadcastMessage:
		return out << "Nmea_AisMessageType_DGNSSBinaryBroadcastMessage";
	case Nmea_AisMessageType_StandardClassBCSPositionReport:
		return out << "Nmea_AisMessageType_StandardClassBCSPositionReport";
	case Nmea_AisMessageType_ExtendedClassBEquipmentPositionReport:
		return out
				<< "Nmea_AisMessageType_ExtendedClassBEquipmentPositionReport";
	case Nmea_AisMessageType_DataLinkManagement:
		return out << "Nmea_AisMessageType_DataLinkManagement";
	case Nmea_AisMessageType_AidToNavigationReport:
		return out << "Nmea_AisMessageType_AidToNavigationReport";
	case Nmea_AisMessageType_ChannelManagement:
		return out << "Nmea_AisMessageType_ChannelManagement";
	case Nmea_AisMessageType_GroupAssignmentCommand:
		return out << "Nmea_AisMessageType_GroupAssignmentCommand";
	case Nmea_AisMessageType_StaticDataReport:
		return out << "Nmea_AisMessageType_StaticDataReport";
	case Nmea_AisMessageType_SingleSlotBinaryMessage:
		return out << "Nmea_AisMessageType_SingleSlotBinaryMessage";
	case Nmea_AisMessageType_MultipleSlotBinaryMessageWithCommunicationsState:
		return out
				<< "Nmea_AisMessageType_MultipleSlotBinaryMessageWithCommunicationsState";
	case Nmea_AisMessageType_PositionReportForLongRangeApplications:
		return out
				<< "Nmea_AisMessageType_PositionReportForLongRangeApplications";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_NavigationStatus val)
{
	switch (val) {
	case Nmea_NavigationStatus_UnderWayUsingEngine:
		return out << "Nmea_NavigationStatus_UnderWayUsingEngine";
	case Nmea_NavigationStatus_AtAnchor:
		return out << "Nmea_NavigationStatus_AtAnchor";
	case Nmea_NavigationStatus_NotUnderCommand:
		return out << "Nmea_NavigationStatus_NotUnderCommand";
	case Nmea_NavigationStatus_RestrictedManoeuverability:
		return out << "Nmea_NavigationStatus_RestrictedManoeuverability";
	case Nmea_NavigationStatus_ConstrainedByHerDraught:
		return out << "Nmea_NavigationStatus_ConstrainedByHerDraught";
	case Nmea_NavigationStatus_Moored:
		return out << "Nmea_NavigationStatus_Moored";
	case Nmea_NavigationStatus_Aground:
		return out << "Nmea_NavigationStatus_Aground";
	case Nmea_NavigationStatus_EngagedInFishing:
		return out << "Nmea_NavigationStatus_EngagedInFishing";
	case Nmea_NavigationStatus_UnderWaySailing:
		return out << "Nmea_NavigationStatus_UnderWaySailing";
	case Nmea_NavigationStatus_Reserved_HSC:
		return out << "Nmea_NavigationStatus_Reserved_HSC";
	case Nmea_NavigationStatus_Reserved_WIG:
		return out << "Nmea_NavigationStatus_Reserved_WIG";
	case Nmea_NavigationStatus_Reserved1:
		return out << "Nmea_NavigationStatus_Reserved1";
	case Nmea_NavigationStatus_Reserved2:
		return out << "Nmea_NavigationStatus_Reserved2";
	case Nmea_NavigationStatus_Reserved3:
		return out << "Nmea_NavigationStatus_Reserved3";
	case Nmea_NavigationStatus_AIS_SART:
		return out << "Nmea_NavigationStatus_AIS_SART";
	case Nmea_NavigationStatus_NotDefined:
		return out << "Nmea_NavigationStatus_NotDefined";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_PositionAccuracy val)
{
	switch (val) {
	case Nmea_PositionAccuracy_UnaugmentedGNSSFix:
		return out << "Nmea_PositionAccuracy_UnaugmentedGNSSFix";
	case Nmea_PositionAccuracy_DGPSQualityFix:
		return out << "Nmea_PositionAccuracy_DGPSQualityFix";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_ManeuverIndicator val)
{
	switch (val) {
	case Nmea_ManeuverIndicator_NotAvailable:
		return out << "Nmea_ManeuverIndicator_NotAvailable";
	case Nmea_ManeuverIndicator_NoSpecialManeuver:
		return out << "Nmea_ManeuverIndicator_NoSpecialManeuver";
	case Nmea_ManeuverIndicator_SpecialManeuver:
		return out << "Nmea_ManeuverIndicator_SpecialManeuver";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_RAIM val)
{
	switch (val) {
	case Nmea_RAIM_NotInUse:
		return out << "Nmea_RAIM_NotInUse";
	case Nmea_RAIM_InUse:
		return out << "Nmea_RAIM_InUse";
	}
	return out << static_cast<int>(val);
}
