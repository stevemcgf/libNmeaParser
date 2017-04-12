/**
*	@file NmeaEnums.h
*	@brief Header for Enumerators and Structures used by NmeaParser
*
*   This set of Enumerators and Structures are use by NmeaParser to return
*   information.
*/

#ifndef SRC_NMEAENUMS_H_
#define SRC_NMEAENUMS_H_

#include <iostream>

/**
 * @brief GPS Quality Indicator in NMEA Sentence GGA. Used in NmeaParser::parseGGA().
 */
enum Nmea_GPSQualityIndicator {
	Nmea_GPSQualityIndicator_FixNotValid,             //!< Fix Not Valid
	Nmea_GPSQualityIndicator_GPSFix,                  //!< GPS Fix
	Nmea_GPSQualityIndicator_GPSFixDifferential,      //!< GPS Fix Differential
	Nmea_GPSQualityIndicator_RealTimeKinematic,       //!< RealTime Kinematic
	Nmea_GPSQualityIndicator_RealTimeKinematicOmniStar//!< RealTime Kinematic OmniStar
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_GPSQualityIndicator.
 * @return ostream to concatenate output.
 */
std::ostream& operator<< (std::ostream & out, Nmea_GPSQualityIndicator val);

/**
 * @brief Speed Distance Units in NMEA Sentence TTM. Used in NmeaParser::parseTTM().
 */
enum Nmea_SpeedDistanceUnits {
	Nmea_SpeedDistanceUnits_Kph_Kilometers,    //!< Speed: Kph Distance: Kilometers
	Nmea_SpeedDistanceUnits_Mps_Meters,        //!< Speed: Mps Distance: Meters
	Nmea_SpeedDistanceUnits_Knots_NauticalMiles//!< Speed: Knots Distance: Nautical Miles
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_SpeedDistanceUnits.
 * @return ostream to concatenate output.
 */
std::ostream& operator<< (std::ostream & out, Nmea_SpeedDistanceUnits val);

/**
 * @brief Target Status in NMEA Sentence TTM. Used in NmeaParser::parseTTM().
 */
enum Nmea_TargetStatus {
	Nmea_TargetStatus_Lost,   //!< Lost
	Nmea_TargetStatus_Query,  //!< Query
	Nmea_TargetStatus_Tracking//!< Tracking
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_TargetStatus.
 * @return ostream to concatenate output.
 */
std::ostream& operator<< (std::ostream & out, Nmea_TargetStatus val);

/**
 * @brief Type Of Acquisition in NMEA Sentence TTM. Used in NmeaParser::parseTTM().
 */
enum Nmea_TypeOfAcquisition {
	Nmea_TypeOfAcquisition_Automatic,//!< Automatic
	Nmea_TypeOfAcquisition_Manual,   //!< Manual
	Nmea_TypeOfAcquisition_Reported  //!< Reported
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_TypeOfAcquisition.
 * @return ostream to concatenate output.
 */
std::ostream& operator<< (std::ostream & out, Nmea_TypeOfAcquisition val);

/**
 * @brief Angle Reference in NMEA Sentence TTM. Used in NmeaParser::parseTTM().
 */
enum Nmea_AngleReference {
	Nmea_AngleReference_True,   //!< Nmea_AngleReference_True
	Nmea_AngleReference_Relative//!< Nmea_AngleReference_Relative
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_AngleReference.
 * @return ostream to concatenate output.
 */
std::ostream& operator<< (std::ostream & out, Nmea_AngleReference val);

/**
 * @brief Track Status in struct NmeaTrackData for NMEA Sentence TTD. Used in NmeaParser::parseTTD().
 */
enum Nmea_TrackStatus {
	Nmea_TrackStatus_Non_tracking,         //!< Non tracking
	Nmea_TrackStatus_Acquiring,            //!< Acquiring
	Nmea_TrackStatus_Lost,                 //!< Lost
	Nmea_TrackStatus_Reserved_1,           //!< Reserved
	Nmea_TrackStatus_Tracking,             //!< Tracking
	Nmea_TrackStatus_Reserved_2,           //!< Reserved
	Nmea_TrackStatus_Tracking_CPA_Alarm,   //!< CPA Alarm
	Nmea_TrackStatus_Tracking_CPA_Alarm_Ack//!< CPA Alarm Ack
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_TrackStatus.
 * @return ostream to concatenate output.
 */
std::ostream& operator<< (std::ostream & out, Nmea_TrackStatus val);

/**
 * @brief Operation in struct NmeaTrackData for NMEA Sentence TTD. Used in NmeaParser::parseTTD().
 */
enum Nmea_Operation {
	Nmea_Operation_Autonomous,//!< Autonomous
	Nmea_Operation_TestTarget //!< TestTarget
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_Operation.
 * @return ostream to concatenate output.
 */
std::ostream& operator<< (std::ostream & out, Nmea_Operation val);

/**
 * @brief Speed Mode in struct NmeaTrackData for NMEA Sentence TTD. Used in NmeaParser::parseTTD().
 */
enum Nmea_SpeedMode {
	Nmea_SpeedMode_TrueSpeedCourse,//!< True Speed Course
	Nmea_SpeedMode_Relative        //!< Relative
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_SpeedMode.
 * @return ostream to concatenate output.
 */
std::ostream& operator<< (std::ostream & out, Nmea_SpeedMode val);

/**
 * @brief Stabilization Mode in struct NmeaTrackData for NMEA Sentence TTD. Used in NmeaParser::parseTTD().
 */
enum Nmea_StabilisationMode {
	Nmea_StabilisationMode_OverGround, //!< Over Ground
	Nmea_StabilisationMode_ThroughWater//!< Through Water
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_StabilisationMode.
 * @return ostream to concatenate output.
 */
std::ostream& operator<< (std::ostream & out, Nmea_StabilisationMode val);

/**
 * @brief Nmea Track Data struct used for parsing TTD binary message. Used in NmeaParser::parseTTD().
 */
struct NmeaTrackData {
	int targetNumber; //!< Target number on radar
	float trueBearing; //!< Target true bearing
	float speed; //!< Target speed
	float course; //!< Target course
	float aisHeading; //!< AIS related target heading
	Nmea_TrackStatus status; //!< Target tracking status
	Nmea_Operation operation; //!< Target operation
	float distance; //!< Target distance
	Nmea_SpeedMode speedMode; //!< Tracking speed mode
	Nmea_StabilisationMode stabilisationMode; //!< Tracking stabilization mode
	int correlationNumber; //!< Correlation number
};

/**
 * @brief Ais Message Type list of valid AIS messages. Used in NmeaParser::parseAISMessageType()
 */
enum Nmea_AisMessageType {
	Nmea_AisMessageType_NA,                                              //!< NA
	Nmea_AisMessageType_PositionReportClassA,                            //!< Position Report Class A
	Nmea_AisMessageType_PositionReportClassA_AssignedSchedule,           //!< Position Report Class A (Assigned Schedule)
	Nmea_AisMessageType_PositionReportClassA_ResponseToInterrogation,    //!< Position Report Class A (Response To Interrogation)
	Nmea_AisMessageType_BaseStationReport,                               //!< Base Station Report
	Nmea_AisMessageType_StaticAndVoyageRelatedData,                      //!< Static And Voyage Related Data
	Nmea_AisMessageType_BinaryAddressedMessage,                          //!< Binary Addressed Message
	Nmea_AisMessageType_BinaryAcknowledge,                               //!< Binary Acknowledge
	Nmea_AisMessageType_BinaryBroadcastMessage,                          //!< Binary Broadcast Message
	Nmea_AisMessageType_StandardSARAircraftPositionReport,               //!< Standard SAR Aircraft Position Report
	Nmea_AisMessageType_UTCAndDateInquiry,                               //!< UTC And Date Inquiry
	Nmea_AisMessageType_UTCAndDateResponse,                              //!< UTC And Date Response
	Nmea_AisMessageType_AddressedSafetyRelatedMessage,                   //!< Addressed Safety Related Message
	Nmea_AisMessageType_SafetyRelatedAcknowledgment,                     //!< Safety Related Acknowledgment
	Nmea_AisMessageType_SafetyRelatedBroadcastMessage,                   //!< Safety Related Broadcast Message
	Nmea_AisMessageType_Interrogation,                                   //!< Interrogation
	Nmea_AisMessageType_AssignmentModeCommand,                           //!< Assignment Mode Command
	Nmea_AisMessageType_DGNSSBinaryBroadcastMessage,                     //!< DGNSS Binary Broadcast Message
	Nmea_AisMessageType_StandardClassBCSPositionReport,                  //!< Standard Class B CS Position Report
	Nmea_AisMessageType_ExtendedClassBEquipmentPositionReport,           //!< Extended Class B Equipment Position Report
	Nmea_AisMessageType_DataLinkManagement,                              //!< Data Link Management
	Nmea_AisMessageType_AidToNavigationReport,                           //!< Aid To Navigation Report
	Nmea_AisMessageType_ChannelManagement,                               //!< Channel Management
	Nmea_AisMessageType_GroupAssignmentCommand,                          //!< Group Assignment Command
	Nmea_AisMessageType_StaticDataReport,                                //!< Static Data Report
	Nmea_AisMessageType_SingleSlotBinaryMessage,                         //!< Single Slot Binary Message
	Nmea_AisMessageType_MultipleSlotBinaryMessageWithCommunicationsState,//!< Multiple Slot Binary Message With Communications State
	Nmea_AisMessageType_PositionReportForLongRangeApplications           //!< Position Report For Long Range Applications
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_AisMessageType.
 * @return ostream to concatenate output.
 */
std::ostream& operator<<(std::ostream & out, Nmea_AisMessageType val);

/**
 * @brief Navigation Status for AIS Class A. Used in AISPositionReportClassA
 */
enum Nmea_NavigationStatus {
	Nmea_NavigationStatus_UnderWayUsingEngine,       //!< Under Way Using Engine
	Nmea_NavigationStatus_AtAnchor,                  //!< At Anchor
	Nmea_NavigationStatus_NotUnderCommand,           //!< Not Under Command
	Nmea_NavigationStatus_RestrictedManeuverability, //!< Restricted Maneuverability
	Nmea_NavigationStatus_ConstrainedByHerDraught,   //!< Constrained By Her Draught
	Nmea_NavigationStatus_Moored,                    //!< Moored
	Nmea_NavigationStatus_Aground,                   //!< Aground
	Nmea_NavigationStatus_EngagedInFishing,          //!< Engaged In Fishing
	Nmea_NavigationStatus_UnderWaySailing,           //!< Under Way Sailing
	Nmea_NavigationStatus_Reserved_HSC,              //!< Reserved HSC
	Nmea_NavigationStatus_Reserved_WIG,              //!< Reserved WIG
	Nmea_NavigationStatus_Reserved1,                 //!< Reserved
	Nmea_NavigationStatus_Reserved2,                 //!< Reserved
	Nmea_NavigationStatus_Reserved3,                 //!< Reserved
	Nmea_NavigationStatus_AIS_SART,                  //!< AIS SART
	Nmea_NavigationStatus_NotDefined                 //!< Not Defined
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_NavigationStatus.
 * @return ostream to concatenate output.
 */
std::ostream& operator<<(std::ostream & out, Nmea_NavigationStatus val);

/**
 * @brief Position Accuracy for AIS. Used in AISPositionReportClassA, AISBaseStationReport and AISStandardClassBCSPositionReport.
 */
enum Nmea_PositionAccuracy {
	Nmea_PositionAccuracy_UnaugmentedGNSSFix,//!< Unaugmented GNSS Fix
	Nmea_PositionAccuracy_DGPSQualityFix     //!< DGPS Quality Fix
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_PositionAccuracy.
 * @return ostream to concatenate output.
 */
std::ostream& operator<<(std::ostream & out, Nmea_PositionAccuracy val);

/**
 * @brief Maneuver Indicator for AIS. Used in AISPositionReportClassA.
 */
enum Nmea_ManeuverIndicator {
	Nmea_ManeuverIndicator_NotAvailable,     //!< Not Available
	Nmea_ManeuverIndicator_NoSpecialManeuver,//!< No Special Maneuver
	Nmea_ManeuverIndicator_SpecialManeuver   //!< Special Maneuver
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_ManeuverIndicator.
 * @return ostream to concatenate output.
 */
std::ostream& operator<<(std::ostream & out, Nmea_ManeuverIndicator val);

/**
 * @brief RAIM for AIS. Used in AISPositionReportClassA and AISBaseStationReport.
 */
enum Nmea_RAIM {
	Nmea_RAIM_NotInUse,//!< Not In Use
	Nmea_RAIM_InUse    //!< In Use
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_RAIM.
 * @return ostream to concatenate output.
 */
std::ostream& operator<<(std::ostream & out, Nmea_RAIM val);

/**
 * @brief EPFDFix for AIS. Used in AISBaseStationReport and AISStaticAndVoyageRelatedData.
 */
enum Nmea_EPFDFix {
	Nmea_EPFDFix_Undefined,                 //!< Undefined
	Nmea_EPFDFix_GPS,                       //!< GPS
	Nmea_EPFDFix_GLONASS,                   //!< GLONASS
	Nmea_EPFDFix_CombinedGPSGLONASS,        //!< Combined GPS GLONASS
	Nmea_EPFDFix_LoranC,                    //!< Loran C
	Nmea_EPFDFix_Chayka,                    //!< Chayka
	Nmea_EPFDFix_IntegratedNavigationSystem,//!< Integrated Navigation System
	Nmea_EPFDFix_Surveyed,                  //!< Surveyed
	Nmea_EPFDFix_Galileo                    //!< Galileo
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_EPFDFix.
 * @return ostream to concatenate output.
 */
std::ostream& operator<<(std::ostream & out, Nmea_EPFDFix val);

/**
 * @brief Ship Type for AIS. Used in AISStaticAndVoyageRelatedData and AISStaticDataReport
 */
enum Nmea_ShipType {
	Nmea_ShipType_NotAvailable,                               //!< Not Available
	Nmea_ShipType_Reserved1,                                  //!< Reserved
	Nmea_ShipType_Reserved2,                                  //!< Reserved
	Nmea_ShipType_Reserved3,                                  //!< Reserved
	Nmea_ShipType_Reserved4,                                  //!< Reserved
	Nmea_ShipType_Reserved5,                                  //!< Reserved
	Nmea_ShipType_Reserved6,                                  //!< Reserved
	Nmea_ShipType_Reserved7,                                  //!< Reserved
	Nmea_ShipType_Reserved8,                                  //!< Reserved
	Nmea_ShipType_Reserved9,                                  //!< Reserved
	Nmea_ShipType_Reserved10,                                 //!< Reserved
	Nmea_ShipType_Reserved11,                                 //!< Reserved
	Nmea_ShipType_Reserved12,                                 //!< Reserved
	Nmea_ShipType_Reserved13,                                 //!< Reserved
	Nmea_ShipType_Reserved14,                                 //!< Reserved
	Nmea_ShipType_Reserved15,                                 //!< Reserved
	Nmea_ShipType_Reserved16,                                 //!< Reserved
	Nmea_ShipType_Reserved17,                                 //!< Reserved
	Nmea_ShipType_Reserved18,                                 //!< Reserved
	Nmea_ShipType_Reserved19,                                 //!< Reserved
	Nmea_ShipType_WingInGround_WIG_AllShipsOfThisType,        //!< Wing In Ground - All Ships Of This Type
	Nmea_ShipType_WingInGround_WIG_HazardousCategoryA,        //!< Wing In Ground - Hazardous Category A
	Nmea_ShipType_WingInGround_WIG_HazardousCategoryB,        //!< Wing In Ground - Hazardous Category B
	Nmea_ShipType_WingInGround_WIG_HazardousCategoryC,        //!< Wing In Ground - Hazardous Category C
	Nmea_ShipType_WingInGround_WIG_HazardousCategoryD,        //!< Wing In Ground - Hazardous Category D
	Nmea_ShipType_WingInGround_WIG_Reserved1,                 //!< Wing In Ground - Reserved
	Nmea_ShipType_WingInGround_WIG_Reserved2,                 //!< Wing In Ground - Reserved
	Nmea_ShipType_WingInGround_WIG_Reserved3,                 //!< Wing In Ground - Reserved
	Nmea_ShipType_WingInGround_WIG_Reserved4,                 //!< Wing In Ground - Reserved
	Nmea_ShipType_WingInGround_WIG_Reserved5,                 //!< Wing In Ground - Reserved
	Nmea_ShipType_Fishing,                                    //!< Fishing
	Nmea_ShipType_Towing,                                     //!< Towing
	Nmea_ShipType_Towing_LengthExceeds200mOrBreadthExceeds25m,//!< Towing - Length Exceeds 200m Or Breadth Exceeds 25m
	Nmea_ShipType_DredgingOrUnderwaterOps,                    //!< Dredging Or Underwater Ops
	Nmea_ShipType_DivingOps,                                  //!< Diving Ops
	Nmea_ShipType_MilitaryOps,                                //!< Military Ops
	Nmea_ShipType_Sailing,                                    //!< Sailing
	Nmea_ShipType_PleasureCraft,                              //!< Pleasure Craft
	Nmea_ShipType_Reserved_1,                                 //!< Reserved
	Nmea_ShipType_Reserved_2,                                 //!< Reserved
	Nmea_ShipType_HighSpeedCraft_HSC_AllShipsOfThisType,      //!< High Speed Craft - All Ships Of This Type
	Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryA,      //!< High Speed Craft - Hazardous Category A
	Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryB,      //!< High Speed Craft - Hazardous Category B
	Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryC,      //!< High Speed Craft - Hazardous Category C
	Nmea_ShipType_HighSpeedCraft_HSC_HazardousCategoryD,      //!< High Speed Craft - Hazardous Category D
	Nmea_ShipType_HighSpeedCraft_HSC_Reserved1,               //!< High Speed Craft - Reserved
	Nmea_ShipType_HighSpeedCraft_HSC_Reserved2,               //!< High Speed Craft - Reserved
	Nmea_ShipType_HighSpeedCraft_HSC_Reserved3,               //!< High Speed Craft - Reserved
	Nmea_ShipType_HighSpeedCraft_HSC_Reserved4,               //!< High Speed Craft - Reserved
	Nmea_ShipType_HighSpeedCraft_HSC_NoAdditionalInformation, //!< High Speed Craft - No Additional Information
	Nmea_ShipType_PilotVessel,                                //!< Pilot Vessel
	Nmea_ShipType_SearchAndRescueVessel,                      //!< Search And Rescue Vessel
	Nmea_ShipType_Tug,                                        //!< Tug
	Nmea_ShipType_PortTender,                                 //!< Port Tender
	Nmea_ShipType_AntiPollutionEquipment,                     //!< Anti Pollution Equipment
	Nmea_ShipType_LawEnforcement,                             //!< Law Enforcement
	Nmea_ShipType_SpareLocalVessel1,                          //!< Spare Local Vessel
	Nmea_ShipType_SpareLocalVessel2,                          //!< Spare Local Vessel
	Nmea_ShipType_MedicalTransport,                           //!< Medical Transport
	Nmea_ShipType_NoncombatantShipAccordingToRR,              //!< Noncombatant Ship According To RR
	Nmea_ShipType_Passenger_AllShipsOfThisType,               //!< Passenger - All Ships Of This Type
	Nmea_ShipType_Passenger_HazardousCategoryA,               //!< Passenger - Hazardous Category A
	Nmea_ShipType_Passenger_HazardousCategoryB,               //!< Passenger - Hazardous Category B
	Nmea_ShipType_Passenger_HazardousCategoryC,               //!< Passenger - Hazardous Category C
	Nmea_ShipType_Passenger_HazardousCategoryD,               //!< Passenger - Hazardous Category D
	Nmea_ShipType_Passenger_Reserved1,                        //!< Passenger - Reserved
	Nmea_ShipType_Passenger_Reserved2,                        //!< Passenger - Reserved
	Nmea_ShipType_Passenger_Reserved3,                        //!< Passenger - Reserved
	Nmea_ShipType_Passenger_Reserved4,                        //!< Passenger - Reserved
	Nmea_ShipType_Passenger_NoAdditionalInformation,          //!< Passenger - No Additional Information
	Nmea_ShipType_Cargo_AllShipsOfThisType,                   //!< Cargo - All Ships Of This Type
	Nmea_ShipType_Cargo_HazardousCategoryA,                   //!< Cargo - Hazardous Category A
	Nmea_ShipType_Cargo_HazardousCategoryB,                   //!< Cargo - Hazardous Category B
	Nmea_ShipType_Cargo_HazardousCategoryC,                   //!< Cargo - Hazardous Category C
	Nmea_ShipType_Cargo_HazardousCategoryD,                   //!< Cargo - Hazardous Category D
	Nmea_ShipType_Cargo_Reserved1,                            //!< Cargo - Reserved
	Nmea_ShipType_Cargo_Reserved2,                            //!< Cargo - Reserved
	Nmea_ShipType_Cargo_Reserved3,                            //!< Cargo - Reserved
	Nmea_ShipType_Cargo_Reserved4,                            //!< Cargo - Reserved
	Nmea_ShipType_Cargo_NoAdditionalInformation,              //!< Cargo - No Additional Information
	Nmea_ShipType_Tanker_AllShipsOfThisType,                  //!< Tanker - All Ships Of This Type
	Nmea_ShipType_Tanker_HazardousCategoryA,                  //!< Tanker - Hazardous Category A
	Nmea_ShipType_Tanker_HazardousCategoryB,                  //!< Tanker - Hazardous Category B
	Nmea_ShipType_Tanker_HazardousCategoryC,                  //!< Tanker - Hazardous Category C
	Nmea_ShipType_Tanker_HazardousCategoryD,                  //!< Tanker - Hazardous Category D
	Nmea_ShipType_Tanker_Reserved1,                           //!< Tanker - Reserved
	Nmea_ShipType_Tanker_Reserved2,                           //!< Tanker - Reserved
	Nmea_ShipType_Tanker_Reserved3,                           //!< Tanker - Reserved
	Nmea_ShipType_Tanker_Reserved4,                           //!< Tanker - Reserved
	Nmea_ShipType_Tanker_NoAdditionalInformation,             //!< Tanker - No Additional Information
	Nmea_ShipType_OtherType_AllShipsOfThisType,               //!< OtherType - All Ships Of This Type
	Nmea_ShipType_OtherType_HazardousCategoryA,               //!< OtherType - Hazardous Category A
	Nmea_ShipType_OtherType_HazardousCategoryB,               //!< OtherType - Hazardous Category B
	Nmea_ShipType_OtherType_HazardousCategoryC,               //!< OtherType - Hazardous Category C
	Nmea_ShipType_OtherType_HazardousCategoryD,               //!< OtherType - Hazardous Category D
	Nmea_ShipType_OtherType_Reserved1,                        //!< OtherType - Reserved
	Nmea_ShipType_OtherType_Reserved2,                        //!< OtherType - Reserved
	Nmea_ShipType_OtherType_Reserved3,                        //!< OtherType - Reserved
	Nmea_ShipType_OtherType_Reserved4,                        //!< OtherType - Reserved
	Nmea_ShipType_OtherType_NoAdditionalInformation           //!< OtherType - No Additional Information
};

/**
 * @brief Operator converts enumerator value to string.
 * @param out ostream to write the string.
 * @param val enumerator value Nmea_ShipType.
 * @return ostream to concatenate output.
 */
std::ostream& operator<<(std::ostream & out, Nmea_ShipType val);

/**
 * @brief Struct used to parse Position Report Class A Ais Message. Used in NmeaParser::parseAISPositionReportClassA().
 */
struct AISPositionReportClassA {
	int repeatIndicator; //!< Message repeat count
	uint mmsi; //!< 9 decimal digits ID
	Nmea_NavigationStatus navigationStatus; //!< Navigation Status
	float rateOfTurn; //!< Rate of Turn
	float speedOverGround; //!< Speed Over Ground
	Nmea_PositionAccuracy positionAccuracy; //!< Position Accuracy
	float longitude; //!< Longitude
	float latitude; //!< Latitude
	float courseOverGround; //!< Course Over Ground
	uint trueHeading; //!< True Heading
	uint timestapUTCSecond; //!< Timestamp UTC second
	Nmea_ManeuverIndicator maneuverIndicator; //!< Maneuver Indicator
	Nmea_RAIM raim; //!< RAIM
};

/**
 * @brief Struct used to parse Base Station Report Ais Message. Used in NmeaParser::parseAISBaseStationReport().
 */
struct AISBaseStationReport {
	int repeatIndicator; //!< Message repeat count
	uint mmsi; //!< 9 decimal digits ID
	int year; //!< UTC Timestamp Year
	int month; //!< UTC Timestamp Month
	int day; //!< UTC Timestamp Day
	int hour; //!< UTC Timestamp Hour
	int minute; //!< UTC Timestamp Minute
	int second; //!< UTC Timestamp Second
	Nmea_PositionAccuracy positionAccuracy; //!< Position Accuracy
	float longitude; //!< Longitude
	float latitude; //!< Latitude
	Nmea_EPFDFix epfd; //!< EPFD Fix
	Nmea_RAIM raim; //!< RAIM
};

/**
 * @brief Struct used to parse Static And Voyage Related Data Ais Message. Used in NmeaParser::parseAISStaticAndVoyageRelatedData().
 */
struct AISStaticAndVoyageRelatedData {
	int repeatIndicator; //!< Message repeat count
	uint mmsi; //!< 9 decimal digits ID
	int aisVersion; //!< AIS Version
	int imoNumber; //!< IMO Ship ID number
	std::string callsign; //!< 7 characters Call Sign
	std::string vesselName; //!< 20 characters Vessel Name
	Nmea_ShipType shipType; //!< Ship Type
	int dimensionToBow; //!< Ship Dimension
	int dimensionToStern; //!< Ship Dimension
	int dimensionToPort; //!< Ship Dimension
	int dimensionToStarboard; //!< Ship Dimension
	Nmea_EPFDFix epfd; //!< EPFD Fix
	int month; //!< ETA Month
	int day; //!< ETA Day
	int hour; //!< ETA Hour
	int minute; //!< ETA Minute
	float draught; //!< Draught
	std::string destination; //!< 20 characters Destination
};

/**
 * @brief Struct used to parse Standard Class B CS Position Report Ais Message. Used in NmeaParser::parseAISStandardClassBCSPositionReport().
 */
struct AISStandardClassBCSPositionReport {
	int repeatIndicator; //!< Message repeat count
	uint mmsi; //!< 9 decimal digits ID
	float speedOverGround; //!< Speed Over Ground
	Nmea_PositionAccuracy positionAccuracy; //!< Position Accuracy
	float longitude; //!< Longitude
	float latitude; //!< Latitude
	float courseOverGround; //!< Course Over Ground
	uint trueHeading; //!< True Heading
	uint timestapUTCSecond; //!< Timestamp UTC second
};

/**
 * @brief Struct used to parse Static Data Report Ais Message. Used in NmeaParser::parseAISStaticDataReport().
 */
struct AISStaticDataReport {
	int repeatIndicator; //!< Message repeat count
	uint mmsi; //!< 9 decimal digits ID
	int partNumber; //!< Indicates which part is valid.  Part A or Part B

	struct {
		std::string vesselName; //!< 20 characters Vessel Name
	} partA; //!< Message Part A

	struct {
		Nmea_ShipType shipType; //!< Ship Type
		std::string vendorId; //!< 3 characters Vendor Id
		int unitModelCode; //!< Unit model code
		int serialNumber; //!< Serial Number
		std::string callsign; //!< 7 characters Call Sign
		int dimensionToBow; //!< Ship Dimension
		int dimensionToStern; //!< Ship Dimension
		int dimensionToPort; //!< Ship Dimension
		int dimensionToStarboard; //!< Ship Dimension
	} partB; //!< Message Part B
};

/**
 * @brief Struct used to pass Transducer Measurement in XDR NMEA message. Used in NmeaParser::parseXDR().
 */
struct TransducerMeasurement {
	char transducerType; //!< Transducer Type
	float measurementData; //!< Measurement Data
	char unitsOfMeasurement; //!< Measurement Units
	std::string nameOfTransducer; //!< Name of transducer
};

#endif /* SRC_NMEAENUMS_H_ */
