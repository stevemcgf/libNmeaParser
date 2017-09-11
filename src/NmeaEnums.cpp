/**
 * @file NmeaEnums.cpp
 * @brief Implementation of helper methods to convert enums to strings.
 */

#include "NmeaEnums.h"

std::ostream& operator<<(std::ostream & out, Nmea_GPSQualityIndicator val)
{
	switch (val)
	{
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

std::ostream& operator<<(std::ostream & out, Nmea_SpeedDistanceUnits val)
{
	switch (val)
	{
	case Nmea_SpeedDistanceUnits_Kph_Kilometers:
		return out << "Nmea_SpeedDistanceUnits_Kph_Kilometers";
	case Nmea_SpeedDistanceUnits_Mps_Meters:
		return out << "Nmea_SpeedDistanceUnits_Mps_Meters";
	case Nmea_SpeedDistanceUnits_Knots_NauticalMiles:
		return out << "Nmea_SpeedDistanceUnits_Knots_NauticalMiles";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_TargetStatus val)
{
	switch (val)
	{
	case Nmea_TargetStatus_Lost:
		return out << "Nmea_TargetStatus_Lost";
	case Nmea_TargetStatus_Query:
		return out << "Nmea_TargetStatus_Query";
	case Nmea_TargetStatus_Tracking:
		return out << "Nmea_TargetStatus_Tracking";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_TypeOfAcquisition val)
{
	switch (val)
	{
	case Nmea_TypeOfAcquisition_Automatic:
		return out << "Nmea_TypeOfAcquisition_Automatic";
	case Nmea_TypeOfAcquisition_Manual:
		return out << "Nmea_TypeOfAcquisition_Manual";
	case Nmea_TypeOfAcquisition_Reported:
		return out << "Nmea_TypeOfAcquisition_Reported";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_AngleReference val)
{
	switch (val)
	{
	case Nmea_AngleReference_True:
		return out << "Nmea_AngleReference_True";
	case Nmea_AngleReference_Relative:
		return out << "Nmea_AngleReference_Relative";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_TrackStatus val)
{
	switch (val)
	{
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

std::ostream& operator<<(std::ostream & out, Nmea_Operation val)
{
	switch (val)
	{
	case Nmea_Operation_Autonomous:
		return out << "Nmea_Operation_Autonomous";
	case Nmea_Operation_TestTarget:
		return out << "Nmea_Operation_TestTarget";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_SpeedMode val)
{
	switch (val)
	{
	case Nmea_SpeedMode_TrueSpeedCourse:
		return out << "Nmea_SpeedMode_TrueSpeedCourse";
	case Nmea_SpeedMode_Relative:
		return out << "Nmea_SpeedMode_Relative";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_StabilisationMode val)
{
	switch (val)
	{
	case Nmea_StabilisationMode_OverGround:
		return out << "Nmea_StabilisationMode_OverGround";
	case Nmea_StabilisationMode_ThroughWater:
		return out << "Nmea_StabilisationMode_ThroughWater";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_AisMessageType val)
{
	switch (val)
	{
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
	case Nmea_AisMessageType_SafetyRelatedAcknowledgment:
		return out << "Nmea_AisMessageType_SafetyRelatedAcknowledgment";
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
	switch (val)
	{
	case Nmea_NavigationStatus_UnderWayUsingEngine:
		return out << "Nmea_NavigationStatus_UnderWayUsingEngine";
	case Nmea_NavigationStatus_AtAnchor:
		return out << "Nmea_NavigationStatus_AtAnchor";
	case Nmea_NavigationStatus_NotUnderCommand:
		return out << "Nmea_NavigationStatus_NotUnderCommand";
	case Nmea_NavigationStatus_RestrictedManeuverability:
		return out << "Nmea_NavigationStatus_RestrictedManeuverability";
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
	switch (val)
	{
	case Nmea_PositionAccuracy_UnaugmentedGNSSFix:
		return out << "Nmea_PositionAccuracy_UnaugmentedGNSSFix";
	case Nmea_PositionAccuracy_DGPSQualityFix:
		return out << "Nmea_PositionAccuracy_DGPSQualityFix";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_ManeuverIndicator val)
{
	switch (val)
	{
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
	switch (val)
	{
	case Nmea_RAIM_NotInUse:
		return out << "Nmea_RAIM_NotInUse";
	case Nmea_RAIM_InUse:
		return out << "Nmea_RAIM_InUse";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_EPFDFix val)
{
	switch (val)
	{
	case Nmea_EPFDFix_Undefined:
		return out << "Nmea_EPFDFix_Undefined";
	case Nmea_EPFDFix_GPS:
		return out << "Nmea_EPFDFix_GPS";
	case Nmea_EPFDFix_GLONASS:
		return out << "Nmea_EPFDFix_GLONASS";
	case Nmea_EPFDFix_CombinedGPSGLONASS:
		return out << "Nmea_EPFDFix_CombinedGPSGLONASS";
	case Nmea_EPFDFix_LoranC:
		return out << "Nmea_EPFDFix_LoranC";
	case Nmea_EPFDFix_Chayka:
		return out << "Nmea_EPFDFix_Chayka";
	case Nmea_EPFDFix_IntegratedNavigationSystem:
		return out << "Nmea_EPFDFix_IntegratedNavigationSystem";
	case Nmea_EPFDFix_Surveyed:
		return out << "Nmea_EPFDFix_Surveyed";
	case Nmea_EPFDFix_Galileo:
		return out << "Nmea_EPFDFix_Galileo";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_ShipType val)
{
	switch (val)
	{
	case Nmea_ShipType_NotAvailable:
		return out << "Nmea_ShipType_NotAvailable";
	case Nmea_ShipType_Reserved1:
		return out << "Nmea_ShipType_Reserved1";
	case Nmea_ShipType_Reserved2:
		return out << "Nmea_ShipType_Reserved2";
	case Nmea_ShipType_Reserved3:
		return out << "Nmea_ShipType_Reserved3";
	case Nmea_ShipType_Reserved4:
		return out << "Nmea_ShipType_Reserved4";
	case Nmea_ShipType_Reserved5:
		return out << "Nmea_ShipType_Reserved5";
	case Nmea_ShipType_Reserved6:
		return out << "Nmea_ShipType_Reserved6";
	case Nmea_ShipType_Reserved7:
		return out << "Nmea_ShipType_Reserved7";
	case Nmea_ShipType_Reserved8:
		return out << "Nmea_ShipType_Reserved8";
	case Nmea_ShipType_Reserved9:
		return out << "Nmea_ShipType_Reserved9";
	case Nmea_ShipType_Reserved10:
		return out << "Nmea_ShipType_Reserved10";
	case Nmea_ShipType_Reserved11:
		return out << "Nmea_ShipType_Reserved11";
	case Nmea_ShipType_Reserved12:
		return out << "Nmea_ShipType_Reserved12";
	case Nmea_ShipType_Reserved13:
		return out << "Nmea_ShipType_Reserved13";
	case Nmea_ShipType_Reserved14:
		return out << "Nmea_ShipType_Reserved14";
	case Nmea_ShipType_Reserved15:
		return out << "Nmea_ShipType_Reserved15";
	case Nmea_ShipType_Reserved16:
		return out << "Nmea_ShipType_Reserved16";
	case Nmea_ShipType_Reserved17:
		return out << "Nmea_ShipType_Reserved17";
	case Nmea_ShipType_Reserved18:
		return out << "Nmea_ShipType_Reserved18";
	case Nmea_ShipType_Reserved19:
		return out << "Nmea_ShipType_Reserved19";
	case Nmea_ShipType_WingInGround_WIG_AllShipsOfThisType:
		return out << "Nmea_ShipType_WingInGround_WIG_AllShipsOfThisType";
	case Nmea_ShipType_WingInGround_WIG_HazardousCategoryA:
		return out << "Nmea_ShipType_WingInGround_WIG_HazardousCategoryA";
	case Nmea_ShipType_WingInGround_WIG_HazardousCategoryB:
		return out << "Nmea_ShipType_WingInGround_WIG_HazardousCategoryB";
	case Nmea_ShipType_WingInGround_WIG_HazardousCategoryC:
		return out << "Nmea_ShipType_WingInGround_WIG_HazardousCategoryC";
	case Nmea_ShipType_WingInGround_WIG_HazardousCategoryD:
		return out << "Nmea_ShipType_WingInGround_WIG_HazardousCategoryD";
	case Nmea_ShipType_WingInGround_WIG_Reserved1:
		return out << "Nmea_ShipType_WingInGround_WIG_Reserved1";
	case Nmea_ShipType_WingInGround_WIG_Reserved2:
		return out << "Nmea_ShipType_WingInGround_WIG_Reserved2";
	case Nmea_ShipType_WingInGround_WIG_Reserved3:
		return out << "Nmea_ShipType_WingInGround_WIG_Reserved3";
	case Nmea_ShipType_WingInGround_WIG_Reserved4:
		return out << "Nmea_ShipType_WingInGround_WIG_Reserved4";
	case Nmea_ShipType_WingInGround_WIG_Reserved5:
		return out << "Nmea_ShipType_WingInGround_WIG_Reserved5";
	case Nmea_ShipType_Fishing:
		return out << "Nmea_ShipType_Fishing";
	case Nmea_ShipType_Towing:
		return out << "Nmea_ShipType_Towing";
	case Nmea_ShipType_Towing_LengthExceeds200mOrBreadthExceeds25m:
		return out
				<< "Nmea_ShipType_Towing_LengthExceeds200mOrBreadthExceeds25m";
	case Nmea_ShipType_DredgingOrUnderwaterOps:
		return out << "Nmea_ShipType_DredgingOrUnderwaterOps";
	case Nmea_ShipType_DivingOps:
		return out << "Nmea_ShipType_DivingOps";
	case Nmea_ShipType_MilitaryOps:
		return out << "Nmea_ShipType_MilitaryOps";
	case Nmea_ShipType_Sailing:
		return out << "Nmea_ShipType_Sailing";
	case Nmea_ShipType_PleasureCraft:
		return out << "Nmea_ShipType_PleasureCraft";
	case Nmea_ShipType_Reserved_1:
		return out << "Nmea_ShipType_Reserved_1";
	case Nmea_ShipType_Reserved_2:
		return out << "Nmea_ShipType_Reserved_2";
	case Nmea_ShipType_HighSpeedCraft_HSC_AllShipsOfThisType:
		return out << "Nmea_ShipType_HighSpeedCraft_HSC_AllShipsOfThisType";
	case Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryA:
		return out << "Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryA";
	case Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryB:
		return out << "Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryB";
	case Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryC:
		return out << "Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryC";
	case Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryD:
		return out << "Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryD";
	case Nmea_ShipType_HighSpeedCraft_HSC_Reserved1:
		return out << "Nmea_ShipType_HighSpeedCraft_HSC_Reserved1";
	case Nmea_ShipType_HighSpeedCraft_HSC_Reserved2:
		return out << "Nmea_ShipType_HighSpeedCraft_HSC_Reserved2";
	case Nmea_ShipType_HighSpeedCraft_HSC_Reserved3:
		return out << "Nmea_ShipType_HighSpeedCraft_HSC_Reserved3";
	case Nmea_ShipType_HighSpeedCraft_HSC_Reserved4:
		return out << "Nmea_ShipType_HighSpeedCraft_HSC_Reserved4";
	case Nmea_ShipType_HighSpeedCraft_HSC_NoAdditionalInformation:
		return out << "Nmea_ShipType_HighSpeedCraft_HSC_NoAdditionalInformation";
	case Nmea_ShipType_PilotVessel:
		return out << "Nmea_ShipType_PilotVessel";
	case Nmea_ShipType_SearchAndRescueVessel:
		return out << "Nmea_ShipType_SearchAndRescueVessel";
	case Nmea_ShipType_Tug:
		return out << "Nmea_ShipType_Tug";
	case Nmea_ShipType_PortTender:
		return out << "Nmea_ShipType_PortTender";
	case Nmea_ShipType_AntiPollutionEquipment:
		return out << "Nmea_ShipType_AntiPollutionEquipment";
	case Nmea_ShipType_LawEnforcement:
		return out << "Nmea_ShipType_LawEnforcement";
	case Nmea_ShipType_SpareLocalVessel1:
		return out << "Nmea_ShipType_SpareLocalVessel1";
	case Nmea_ShipType_SpareLocalVessel2:
		return out << "Nmea_ShipType_SpareLocalVessel2";
	case Nmea_ShipType_MedicalTransport:
		return out << "Nmea_ShipType_MedicalTransport";
	case Nmea_ShipType_NoncombatantShipAccordingToRR:
		return out << "Nmea_ShipType_NoncombatantShipAccordingToRR";
	case Nmea_ShipType_Passenger_AllShipsOfThisType:
		return out << "Nmea_ShipType_Passenger_AllShipsOfThisType";
	case Nmea_ShipType_Passenger_HazardousCategoryA:
		return out << "Nmea_ShipType_Passenger_HazardousCategoryA";
	case Nmea_ShipType_Passenger_HazardousCategoryB:
		return out << "Nmea_ShipType_Passenger_HazardousCategoryB";
	case Nmea_ShipType_Passenger_HazardousCategoryC:
		return out << "Nmea_ShipType_Passenger_HazardousCategoryC";
	case Nmea_ShipType_Passenger_HazardousCategoryD:
		return out << "Nmea_ShipType_Passenger_HazardousCategoryD";
	case Nmea_ShipType_Passenger_Reserved1:
		return out << "Nmea_ShipType_Passenger_Reserved1";
	case Nmea_ShipType_Passenger_Reserved2:
		return out << "Nmea_ShipType_Passenger_Reserved2";
	case Nmea_ShipType_Passenger_Reserved3:
		return out << "Nmea_ShipType_Passenger_Reserved3";
	case Nmea_ShipType_Passenger_Reserved4:
		return out << "Nmea_ShipType_Passenger_Reserved4";
	case Nmea_ShipType_Passenger_NoAdditionalInformation:
		return out << "Nmea_ShipType_Passenger_NoAdditionalInformation";
	case Nmea_ShipType_Cargo_AllShipsOfThisType:
		return out << "Nmea_ShipType_Cargo_AllShipsOfThisType";
	case Nmea_ShipType_Cargo_HazardousCategoryA:
		return out << "Nmea_ShipType_Cargo_HazardousCategoryA";
	case Nmea_ShipType_Cargo_HazardousCategoryB:
		return out << "Nmea_ShipType_Cargo_HazardousCategoryB";
	case Nmea_ShipType_Cargo_HazardousCategoryC:
		return out << "Nmea_ShipType_Cargo_HazardousCategoryC";
	case Nmea_ShipType_Cargo_HazardousCategoryD:
		return out << "Nmea_ShipType_Cargo_HazardousCategoryD";
	case Nmea_ShipType_Cargo_Reserved1:
		return out << "Nmea_ShipType_Cargo_Reserved1";
	case Nmea_ShipType_Cargo_Reserved2:
		return out << "Nmea_ShipType_Cargo_Reserved2";
	case Nmea_ShipType_Cargo_Reserved3:
		return out << "Nmea_ShipType_Cargo_Reserved3";
	case Nmea_ShipType_Cargo_Reserved4:
		return out << "Nmea_ShipType_Cargo_Reserved4";
	case Nmea_ShipType_Cargo_NoAdditionalInformation:
		return out << "Nmea_ShipType_Cargo_NoAdditionalInformation";
	case Nmea_ShipType_Tanker_AllShipsOfThisType:
		return out << "Nmea_ShipType_Tanker_AllShipsOfThisType";
	case Nmea_ShipType_Tanker_HazardousCategoryA:
		return out << "Nmea_ShipType_Tanker_HazardousCategoryA";
	case Nmea_ShipType_Tanker_HazardousCategoryB:
		return out << "Nmea_ShipType_Tanker_HazardousCategoryB";
	case Nmea_ShipType_Tanker_HazardousCategoryC:
		return out << "Nmea_ShipType_Tanker_HazardousCategoryC";
	case Nmea_ShipType_Tanker_HazardousCategoryD:
		return out << "Nmea_ShipType_Tanker_HazardousCategoryD";
	case Nmea_ShipType_Tanker_Reserved1:
		return out << "Nmea_ShipType_Tanker_Reserved1";
	case Nmea_ShipType_Tanker_Reserved2:
		return out << "Nmea_ShipType_Tanker_Reserved2";
	case Nmea_ShipType_Tanker_Reserved3:
		return out << "Nmea_ShipType_Tanker_Reserved3";
	case Nmea_ShipType_Tanker_Reserved4:
		return out << "Nmea_ShipType_Tanker_Reserved4";
	case Nmea_ShipType_Tanker_NoAdditionalInformation:
		return out << "Nmea_ShipType_Tanker_NoAdditionalInformation";
	case Nmea_ShipType_OtherType_AllShipsOfThisType:
		return out << "Nmea_ShipType_OtherType_AllShipsOfThisType";
	case Nmea_ShipType_OtherType_HazardousCategoryA:
		return out << "Nmea_ShipType_OtherType_HazardousCategoryA";
	case Nmea_ShipType_OtherType_HazardousCategoryB:
		return out << "Nmea_ShipType_OtherType_HazardousCategoryB";
	case Nmea_ShipType_OtherType_HazardousCategoryC:
		return out << "Nmea_ShipType_OtherType_HazardousCategoryC";
	case Nmea_ShipType_OtherType_HazardousCategoryD:
		return out << "Nmea_ShipType_OtherType_HazardousCategoryD";
	case Nmea_ShipType_OtherType_Reserved1:
		return out << "Nmea_ShipType_OtherType_Reserved1";
	case Nmea_ShipType_OtherType_Reserved2:
		return out << "Nmea_ShipType_OtherType_Reserved2";
	case Nmea_ShipType_OtherType_Reserved3:
		return out << "Nmea_ShipType_OtherType_Reserved3";
	case Nmea_ShipType_OtherType_Reserved4:
		return out << "Nmea_ShipType_OtherType_Reserved4";
	case Nmea_ShipType_OtherType_NoAdditionalInformation:
		return out << "Nmea_ShipType_OtherType_NoAdditionalInformation";
	}
	return out << static_cast<int>(val);
}

std::ostream& operator<<(std::ostream & out, Nmea_NavigationAidType val)
{
	switch (val)
	{
	case Nmea_NavigationAidType_Default:
		return out << "Nmea_NavigationAidType_Default";
	case Nmea_NavigationAidType_ReferencePoint:
		return out << "Nmea_NavigationAidType_ReferencePoint";
	case Nmea_NavigationAidType_RACON:
		return out << "Nmea_NavigationAidType_RACON";
	case Nmea_NavigationAidType_FixedStructureOffShore:
		return out << "Nmea_NavigationAidType_FixedStructureOffShore";
	case Nmea_NavigationAidType_Reserved:
		return out << "Nmea_NavigationAidType_Reserved";
	case Nmea_NavigationAidType_LightWithoutSectors:
		return out << "Nmea_NavigationAidType_LightWithoutSectors";
	case Nmea_NavigationAidType_LightWithSectors:
		return out << "Nmea_NavigationAidType_LightWithSectors";
	case Nmea_NavigationAidType_LeadingLightFront:
		return out << "Nmea_NavigationAidType_LeadingLightFront";
	case Nmea_NavigationAidType_LeadingLightRear:
		return out << "Nmea_NavigationAidType_LeadingLightRear";
	case Nmea_NavigationAidType_BeaconCardinalN:
		return out << "Nmea_NavigationAidType_BeaconCardinalN";
	case Nmea_NavigationAidType_BeaconCardinalE:
		return out << "Nmea_NavigationAidType_BeaconCardinalE";
	case Nmea_NavigationAidType_BeaconCardinalS:
		return out << "Nmea_NavigationAidType_BeaconCardinalS";
	case Nmea_NavigationAidType_BeaconCardinalW:
		return out << "Nmea_NavigationAidType_BeaconCardinalW";
	case Nmea_NavigationAidType_BeaconPortHand:
		return out << "Nmea_NavigationAidType_BeaconPortHand";
	case Nmea_NavigationAidType_BeaconStarboardHand:
		return out << "Nmea_NavigationAidType_BeaconStarboardHand";
	case Nmea_NavigationAidType_BeaconPreferredChannelPortHand:
		return out << "Nmea_NavigationAidType_BeaconPreferredChannelPortHand";
	case Nmea_NavigationAidType_BeaconPreferredChannelStarboardHand:
		return out
				<< "Nmea_NavigationAidType_BeaconPreferredChannelStarboardHand";
	case Nmea_NavigationAidType_BeaconIsolatedDanger:
		return out << "Nmea_NavigationAidType_BeaconIsolatedDanger";
	case Nmea_NavigationAidType_BeaconSafeWater:
		return out << "Nmea_NavigationAidType_BeaconSafeWater";
	case Nmea_NavigationAidType_BeaconSpecialMark:
		return out << "Nmea_NavigationAidType_BeaconSpecialMark";
	case Nmea_NavigationAidType_CardinalMarkN:
		return out << "Nmea_NavigationAidType_CardinalMarkN";
	case Nmea_NavigationAidType_CardinalMarkE:
		return out << "Nmea_NavigationAidType_CardinalMarkE";
	case Nmea_NavigationAidType_CardinalMarkS:
		return out << "Nmea_NavigationAidType_CardinalMarkS";
	case Nmea_NavigationAidType_CardinalMarkW:
		return out << "Nmea_NavigationAidType_CardinalMarkW";
	case Nmea_NavigationAidType_PortHandMark:
		return out << "Nmea_NavigationAidType_PortHandMark";
	case Nmea_NavigationAidType_StarboardHandMark:
		return out << "Nmea_NavigationAidType_StarboardHandMark";
	case Nmea_NavigationAidType_PreferredChannelPortHand:
		return out << "Nmea_NavigationAidType_PreferredChannelPortHand";
	case Nmea_NavigationAidType_PreferredChannelStarboardHand:
		return out << "Nmea_NavigationAidType_PreferredChannelStarboardHand";
	case Nmea_NavigationAidType_IsolatedDanger:
		return out << "Nmea_NavigationAidType_IsolatedDanger";
	case Nmea_NavigationAidType_SafeWater:
		return out << "Nmea_NavigationAidType_SafeWater";
	case Nmea_NavigationAidType_SpecialMark:
		return out << "Nmea_NavigationAidType_SpecialMark";
	case Nmea_NavigationAidType_LightVessel:
		return out << "Nmea_NavigationAidType_LightVessel";
	}
	return out << static_cast<int>(val);
}

