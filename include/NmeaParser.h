/**
 *	@file NmeaParser.h
 *	@brief Header for NmeaParser class
 *
 *   NmeaParser class with all parsing methods.
 */

#ifndef SRC_NMEAPARSER_H_
#define SRC_NMEAPARSER_H_

#include <vector>
#include <string>
#include <boost/date_time.hpp>
#include "NmeaEnums.h"
#include <bitset>
#include <boost/dynamic_bitset.hpp>

typedef std::bitset<16> NmeaParserResult; //!<  Bitset. Each index represents the validity of each output parameter.

typedef std::bitset<6> SixBit; //!< Helper type used to parse binary encoded data on NMEA Messages.

/**
 * @brief State-less class for static methods used for Parsing NMEA.
 */
class NmeaParser {
public:
	/**
	 * @brief ZDA NMEA Message parser
	 *
	 * <b>ZDA NMEA message fields</b><br>
	 * <i>Time & Date - UTC, day, month, year and local time zone</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $--ZDA
	 * 1 | UTC
	 * 2 | Day, ranging between 01 and 31
	 * 3 | Month, ranging between 01 and 12
	 * 4 | Year
	 * 5 | Local time zone offset from GMT, ranging from 00 through ±13 hours
	 * 6 | Local time zone offset from GMT, ranging from 00 through 59 minutes
	 * 7 | The checksum data, always begins with *
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] mtime UTC time
	 * @param [out] day UTC Day
	 * @param [out] month UTC Month
	 * @param [out] year UTC Year
	 * @param [out] localZoneHours Local time zone Hours
	 * @param [out] localZoneMinutes Local time zone Minutes
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseZDA(const std::string& nmea,
			boost::posix_time::time_duration& mtime, int& day, int& month,
			int& year, int& localZoneHours, int& localZoneMinutes);

	/**
	 * @brief GLL NMEA Message parser
	 *
	 * <b>GLL NMEA message fields</b><br>
	 * <i>Geographic Position - Latitude/Longitude</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $--GLL
	 * 1 | Latitude in dd mm,mmmm format (0-7 decimal places)
	 * 2 | Direction of latitude N: North S: South
	 * 3 | Longitude in ddd mm,mmmm format (0-7 decimal places)
	 * 4 | Direction of longitude E: East W: West
	 * 5 | UTC of position in hhmmss.ss format
	 * 6 | Fixed text "A" shows that data is valid
	 * 7 | Fixed text "A" shows that data is valid
	 * 8 | The checksum data, always begins with *
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] latitude Latitude
	 * @param [out] longitude Longitude
	 * @param [out] mtime UTC time
	 * @param [out] status Status
	 * @param [out] modeIndicator Mode Indicator
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseGLL(const std::string& nmea, double& latitude,
			double& longitude, boost::posix_time::time_duration& mtime,
			char& status, char& modeIndicator);

	/**
	 * @brief GGA NMEA Message parser
	 *
	 * <b>GGA NMEA message fields</b><br>
	 * <i>Global Positioning System Fix Data</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $--GGA
	 * 1 | UTC of position fix
	 * 2 | Latitude
	 * 3 | Direction of latitude: N: North S: South
	 * 4 | Longitude
	 * 5 | Direction of longitude: E: East W: West
	 * 6 | GPS Quality indicator: 0: Fix not valid 1: GPS fix 2: Differential GPS fix, OmniSTAR VBS 4: Real-Time Kinematic, fixed integers 5: Real-Time Kinematic, float integers, OmniSTAR XP/HP or Location RTK
	 * 7 | Number of SVs in use, range from 00 through to 24+
	 * 8 | HDOP
	 * 9 | Orthometric height (MSL reference)
	 * 10 | M: unit of measure for orthometric height is meters
	 * 11 | Geoid separation
	 * 12 | M: geoid separation measured in meters
	 * 13 | Age of differential GPS data record, Type 1 or Type 9. Null field when DGPS is not used.
	 * 14 | Reference station ID, range 0000-4095. A null field when any reference station ID is selected and no corrections are received1.
	 * 15 | The checksum data, always begins with *
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] mtime UTC time
	 * @param [out] latitude Latitude
	 * @param [out] longitude Longitude
	 * @param [out] quality Quality Indicator
	 * @param [out] numSV SVs in use
	 * @param [out] hdop HDOP
	 * @param [out] orthometricheight Orthometric height (MSL reference)
	 * @param [out] geoidseparation geoid separation measured in meters
	 * @param [out] agediffgps Age of differential GPS data record
	 * @param [out] refid Reference station ID
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseGGA(const std::string& nmea,
			boost::posix_time::time_duration& mtime, double& latitude,
			double& longitude, Nmea_GPSQualityIndicator& quality, int& numSV,
			double& hdop, double& orthometricheight, double& geoidseparation,
			double& agediffgps, std::string& refid);

	/**
	 * @brief VTG NMEA Message parser
	 *
	 * <b>VTG NMEA message fields</b><br>
	 * <i>Track made good and Ground speed</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $--VTG
	 * 1 | Course over ground (degrees true)
	 * 2 | T: relative to true north
	 * 3 | Course over ground (degrees magnetic)
	 * 4 | M: relative to magnetic north
	 * 5 | Speed, in knots
	 * 6 | N: speed is measured in knots
	 * 7 | Speed over ground in kilometers/hour (kph)
	 * 8 | K: speed over ground is measured in kph
	 * 9 | The checksum data, always begins with *
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] coursetrue Course Over Ground
	 * @param [out] coursemagnetic Course Over Ground (relative magnetic north)
	 * @param [out] speedknots Speed in knots
	 * @param [out] speedkph Speed in Kph
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseVTG(const std::string& nmea,
			double& coursetrue, double& coursemagnetic, double& speedknots,
			double& speedkph);

	/**
	 * @brief RMC NMEA Message parser
	 *
	 * <b>RMC NMEA message fields</b><br>
	 * <i>Recommended Minimum Navigation Information</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $--RMC
	 * 1 | UTC of position fix
	 * 2 | Status A=active or V=void
	 * 3 | Latitude
	 * 4 | Direction of latitude: N: North S: South
	 * 5 | Longitude
	 * 6 | Direction of longitude: E: East W: West
	 * 7 | Speed over the ground in knots
	 * 8 | Course over ground, degrees true
	 * 9 | Date
	 * 10 | Magnetic variation
	 * 11 | degrees E/W
	 * 12 | Mode indicator
	 * 13 | Navigational status
	 * 14 | The checksum data, always begins with *
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] mtime UTC time
	 * @param [out] latitude Latitude
	 * @param [out] longitude Longitude
	 * @param [out] speedknots Speed in Knots
	 * @param [out] coursetrue Course relative to true north
	 * @param [out] mdate UTC date
	 * @param [out] magneticvar Magnetic variation
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseRMC(const std::string& nmea,
			boost::posix_time::time_duration& mtime, double& latitude,
			double& longitude, double& speedknots, double& coursetrue,
			boost::gregorian::date& mdate, double& magneticvar);

	/**
	 * @brief WPL NMEA Message parser
	 *
	 * <b>WPL NMEA message fields</b><br>
	 * <i>Waypoint Location</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $--WPL
	 * 1 | Latitude
	 * 2 | N or S (North or South)
	 * 3 | Longitude
	 * 4 | E or W (East or West)
	 * 5 | Waypoint Name
	 * 6 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] latitude Latitude
	 * @param [out] longitude Longitude
	 * @param [out] waypointName Waypoint Name
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseWPL(const std::string& nmea, double& latitude,
			double& longitude, std::string& waypointName);

	/**
	 * @brief RTE NMEA Message parser
	 *
	 * <b>RTE NMEA message fields</b><br>
	 * <i>RoutesSFI - Scanning Frequency Information</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $--RTE
	 * 1 | Number of sentences in sequence
	 * 2 | Sentence number
	 * 3 | 'c' = Current active route, 'w' = waypoint list starts with destination waypoint
	 * 4 | Name or number of the active route
	 * 5 | onwards, Names of waypoints in Route
	 * 6 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] totalLines Total Lines
	 * @param [out] lineCount Current Line
	 * @param [out] messageMode Message mode
	 * @param [out] routeIdentifier Route Identifier
	 * @param [out] waypointNames Waypoint Names
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseRTE(const std::string& nmea, int& totalLines,
			int& lineCount, char& messageMode, std::string& routeIdentifier,
			std::vector<std::string>& waypointNames);

	/**
	 * @brief VHW NMEA Message parser
	 *
	 * <b>VHW NMEA message fields</b><br>
	 * <i>Water speed and heading</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $VDVHW
	 * 1 | Degress True
	 * 2 | T = True
	 * 3 | Degrees Magnetic
	 * 4 | M = Magnetic
	 * 5 | Knots (speed of vessel relative to the water)
	 * 6 | N = Knots
	 * 7 | Kilometers (speed of vessel relative to the water)
	 * 8 | K = Kilometres
	 * 9 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] speedInKnots Speed in Knots
	 * @param [out] speedInKmH Speed in Km/h
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseVHW(const std::string& nmea,
			double& speedInKnots, double& speedInKmH);

	/**
	 * @brief MTW NMEA Message parser
	 *
	 * <b>MTW NMEA message fields</b><br>
	 * <i>Mean Temperature of Water</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $VDMTW
	 * 1 | Degrees
	 * 2 | Unit of Measurement, Celsius
	 * 3 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] degrees Temperature degrees
	 * @param [out] units Temperature Units
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseMTW(const std::string& nmea, double& degrees,
			char& units);

	/**
	 * @brief VBW NMEA Message parser
	 *
	 * <b>VBW NMEA message fields</b><br>
	 * <i>Dual Ground/Water Speed</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $VDVBW
	 * 1 | Longitudinal water speed, "-" means astern
	 * 2 | Transverse water speed, "-" means port
	 * 3 | Water Data Status, A = data valid
	 * 4 | Longitudinal ground speed, "-" means astern
	 * 5 | Transverse ground speed, "-" means port
	 * 6 | Ground Data Status, A = data valid
	 * 7 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] longitudinalWaterSpeed Longitudinal water speed, negative means astern
	 * @param [out] transverseWaterSpeed Transverse water speed, negative means port
	 * @param [out] waterDataStatus Water Data Status
	 * @param [out] longitudinalGroundSpeed Longitudinal ground speed, negative means astern
	 * @param [out] transverseGroundSpeed Transverse ground speed, negative means port
	 * @param [out] groundDataStatus Ground Data Status
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseVBW(const std::string& nmea,
			double& longitudinalWaterSpeed, double& transverseWaterSpeed,
			char& waterDataStatus, double& longitudinalGroundSpeed,
			double& transverseGroundSpeed, char& groundDataStatus);

	/**
	 * @brief VLW NMEA Message parser
	 *
	 * <b>VLW NMEA message fields</b><br>
	 * <i>Distance Traveled through Water</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $VDVLW
	 * 1 | Total cumulative distance
	 * 2 | N = Nautical Miles
	 * 3 | Distance since Reset
	 * 4 | N = Nautical Miles
	 * 5 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] totalCumulativeDistance Total cumulative distance in Nautical Miles
	 * @param [out] distanceSinceReset Distance since reset in Nautical Miles
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseVLW(const std::string& nmea,
			double& totalCumulativeDistance, double& distanceSinceReset);

	/**
	 * @brief DPT NMEA Message parser
	 *
	 * <b>DPT NMEA message fields</b><br>
	 * <i>Depth of Water</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $SDDPT
	 * 1 | Water Depth Relative, meters
	 * 2 | Offset from transducer
	 * 3 | Maximum range scale in use, meters
	 * 4 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] waterDepthRelativeToTheTransducer Water Depth Relative to transducer in meters
	 * @param [out] offsetFromTransducer Offset from transducer
	 * @param [out] maximumRangeScaleInUse Maximum range scale in use
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseDPT(const std::string& nmea,
			double& waterDepthRelativeToTheTransducer,
			double& offsetFromTransducer, double& maximumRangeScaleInUse);

	/**
	 * @brief DBT NMEA Message parser
	 *
	 * <b>DBT NMEA message fields</b><br>
	 * <i>Depth below transducer</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $SDDBT
	 * 1 | Depth, feet
	 * 2 | f = feet
	 * 3 | Depth, meters
	 * 4 | M = meters
	 * 5 | Depth, Fathoms
	 * 6 | F = Fathoms
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] waterDepthInFeet Water Depth in Feet
	 * @param [out] waterDepthInMeters Water Depth in Meters
	 * @param [out] waterDepthInFathoms Water Depth in Fathoms
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseDBT(const std::string& nmea,
			double& waterDepthInFeet, double& waterDepthInMeters,
			double& waterDepthInFathoms);

	/**
	 * @brief DBK NMEA Message parser
	 *
	 * <b>DBK NMEA message fields</b><br>
	 * <i>Depth Below Keel</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $SDDBK
	 * 1 | Depth, feet
	 * 2 | f = feet
	 * 3 | Depth, meters
	 * 4 | M = meters
	 * 5 | Depth, Fathoms
	 * 6 | F = Fathoms
	 * 7 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] depthBelowKeelFeet Depth below Keel in Feet
	 * @param [out] depthBelowKeelMeters Depth below Keel in Meter
	 * @param [out] depthBelowKeelFathoms Depth below Keel in Fathoms
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseDBK(const std::string& nmea,
			double& depthBelowKeelFeet, double& depthBelowKeelMeters,
			double& depthBelowKeelFathoms);

	/**
	 * @brief PSKPDPT NMEA Message parser
	 *
	 * <b>PSKPDPT NMEA message fields</b><br>
	 * <i>SKIPPER proprietary sentence for multiple transducers installation</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $PSKPDPT
	 * 1 | Water depth relative to transducer, meters
	 * 2 | Offset from transducer, meters
	 * 3 | Maximum range scale in use, meters
	 * 4 | Bottom echo strength (0,9)
	 * 5 | Echo sounder channel number (0-99)
	 * 6 | Transducer location
	 * 7 | Check sum, possible to turn on/off (see screen 8)
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] waterDepthRelativeToTheTransducer Water Depth Relative to the Transducer
	 * @param [out] offsetFromTransducer Offset from Transducer
	 * @param [out] maximumRangeScaleInUse Maximum Range Scale in Use
	 * @param [out] bottomEchoStrength Bottom Echo Strength
	 * @param [out] echoSounderChannelNumber Echo Sounder Channel Number
	 * @param [out] transducerLocation Transducer Location
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parsePSKPDPT(const std::string& nmea,
			double& waterDepthRelativeToTheTransducer,
			double& offsetFromTransducer, double& maximumRangeScaleInUse,
			int& bottomEchoStrength, int& echoSounderChannelNumber,
			std::string& transducerLocation);

	/**
	 * @brief HDT NMEA Message parser
	 *
	 * <b>HDT NMEA message fields</b><br>
	 * <i>Heading - 1</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $HCHDT
	 * 1 | Heading Degrees, true
	 * 2 | T = True
	 * 3 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] headingDegreesTrue Heading degrees relative to true north
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseHDT(const std::string& nmea,
			double& headingDegreesTrue);

	/**
	 * @brief HDG NMEA Message parser
	 *
	 * <b>HDG NMEA message fields</b><br>
	 * <i>Heading - Deviation & Variation</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $HCHDG
	 * 1 | Magnetic Sensor heading in degrees
	 * 2 | Magnetic Deviation, degrees
	 * 3 | Magnetic Deviation direction, E = Easterly, W = Westerly
	 * 4 | Magnetic Variation degrees
	 * 5 | Magnetic Variation direction, E = Easterly, W = Westerly
	 * 6 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] magneticSensorHeadingInDegrees Magnetic Sensor Heading in Degrees
	 * @param [out] magneticDeviationDegrees Magnetic Deviation Degrees
	 * @param [out] magneticDeviationDirection Magnetic Deviation Direction
	 * @param [out] magneticVariationDegrees Magnetic Variation Degrees
	 * @param [out] magneticVariationDirection Magnetic Variation Direction
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseHDG(const std::string& nmea,
			double& magneticSensorHeadingInDegrees,
			double& magneticDeviationDegrees, char& magneticDeviationDirection,
			double& magneticVariationDegrees, char& magneticVariationDirection);

	/**
	 * @brief HDM NMEA Message parser
	 *
	 * <b>HDM NMEA message fields</b><br>
	 * <i>Heading - Magnetic</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $HCHDM
	 * 1 | Heading Degrees, magnetic
	 * 2 | M = magnetic
	 * 3 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] headingDegreesMagnetic Heading Degrees relative to magnetic North
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseHDM(const std::string& nmea,
			double& headingDegreesMagnetic);

	/**
	 * @brief ROT NMEA Message parser
	 *
	 * <b>ROT NMEA message fields</b><br>
	 * <i>Rate Of Turn</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $--ROT
	 * 1 | Rate Of Turn, degrees per minute, "-" means bow turns to port
	 * 2 | Status, A means data is valid
	 * 3 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] rateOfTurn Rate of Turn, Degrees per minute. Negative means to port.
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseROT(const std::string& nmea,
			double& rateOfTurn);

	/**
	 * @brief MWV NMEA Message parser
	 *
	 * <b>MWV NMEA message fields</b><br>
	 * <i>Wind Speed and Angle</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $IIMWV
	 * 1 | Wind Angle, 0 to 360 degrees
	 * 2 | Reference, R = Relative, T = True
	 * 3 | Wind Speed
	 * 4 | Wind Speed Units, K = km/hr, M = m/sec, N = kt
	 * 5 | Sensor Status, A = Valid, V = Void
	 * 6 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] windAngle Wind Angle in degrees
	 * @param [out] reference Reference True or Relative
	 * @param [out] windSpeed Wind Speed
	 * @param [out] windSpeedUnits Wind Speed Units
	 * @param [out] sensorStatus Sensor Status
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseMWV(const std::string& nmea, double& windAngle,
			char& reference, double& windSpeed, char& windSpeedUnits,
			char& sensorStatus);

	/**
	 * @brief MWD NMEA Message parser
	 *
	 * <b>MWD NMEA message fields</b><br>
	 * <i>Wind Direction & Speed</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID --MWD
	 * 1 | Wind direction, 0° to 359º
	 * 2 | True
	 * 3 | Wind direction, 0° to 359º
	 * 4 | Magnetic
	 * 5 | Wind speed in Knots
	 * 6 | Knots
	 * 7 | Wind speed in Meters per second
	 * 8 | meters/second
	 * 9 | checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] trueWindDirection Wind Direction in Degrees relative to True North.
	 * @param [out] magneticWindDirection Wind Direction in Degrees relative to Magnetic North.
	 * @param [out] windSpeedKnots Wind Speed in Knots.
	 * @param [out] windSpeedMeters Wind Speed in Meters per second.
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseMWD(const std::string& nmea,
			double& trueWindDirection, double& magneticWindDirection,
			double& windSpeedKnots, double& windSpeedMeters);

	/**
	 * @brief XDR NMEA Message parser
	 *
	 * <b>XDR NMEA message fields</b><br>
	 * <i>Transducer Measurement</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID --XDR
	 * 1 | Transducer Type
	 * 2 | Measurement Data
	 * 3 | Units of Measurement
	 * 4 | Name of Transducer
	 * ... | More Quadruplets like This.
	 * n | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] measurements Vector of measurements. Each item have Transducer Type, Measurement Data, Units and Name of Transducer.
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseXDR(const std::string& nmea,
			std::vector<TransducerMeasurement>& measurements);

	/**
	 * @brief TTM NMEA Message parser
	 *
	 * <b>TTM NMEA message fields</b><br>
	 * <i>Tracked Target Message</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID $--TTM
	 * 1 | Target Number (0-99)
	 * 2 | Target Distance from own ship
	 * 3 | Bearing from own ship
	 * 4 | (T/R) true/relative
	 * 5 | Target Speed
	 * 6 | Target Course
	 * 7 | (T/R) true relative
	 * 8 | Distance of closest-point-of-approach
	 * 9 | Time to CPA, min., "-" means increasing
	 * 10 | Speed units, (K/N/S)
	 * 11 | Target Name
	 * 12 | Target status
	 * 13 | Reference Target R, null otherwise
	 * 14 | Time of data UTC
	 * 15 | Type of acquisition A/M/R
	 * 16 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] targetNumber Target Number
	 * @param [out] targetDistance Distance to Target
	 * @param [out] targetBearing Bearing to Target
	 * @param [out] targetBearingReference Relative or True North Reference
	 * @param [out] targetSpeed Target Speed
	 * @param [out] targetCourse Target Course
	 * @param [out] targetCourseReference Relative or True North Reference
	 * @param [out] speedDistanceUnits Speed and Distance Units
	 * @param [out] targetName Target Name
	 * @param [out] targetStatus Target Status
	 * @param [out] timeOfData Time of acquisition
	 * @param [out] typeOfAcquisition Type of acquisition
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseTTM(const std::string& nmea, int& targetNumber,
			double& targetDistance, double& targetBearing,
			Nmea_AngleReference& targetBearingReference, double& targetSpeed,
			double& targetCourse, Nmea_AngleReference& targetCourseReference,
			Nmea_SpeedDistanceUnits& speedDistanceUnits,
			std::string& targetName, Nmea_TargetStatus& targetStatus,
			boost::posix_time::time_duration& timeOfData,
			Nmea_TypeOfAcquisition& typeOfAcquisition);

	/**
	 * @brief TTD NMEA Message parser
	 *
	 * <b>TTD NMEA message fields</b><br>
	 * <i>Tracked Target Data</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID --TTD
	 * 1 | Total hex number of sentences needed to transfer the message, 1 to FF
	 * 2 | Hex sentence number, 1 to FF
	 * 3 | Sequential message identifier, 0 to 9
	 * 4 | Encapsulated tracked target data
	 * 5 | Number of fill-bits, 0 to 5
	 * 6 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] totalLines Total lines needed to transfer the binary message
	 * @param [out] lineCount Current line sentence number
	 * @param [out] sequenceIdentifier Sequence identifier
	 * @param [out] trackData Encapsulated tracked target data
	 * @param [out] fillBits Number of fill-bits, 0 to 5
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseTTD(const std::string& nmea, int& totalLines,
			int& lineCount, int& sequenceIdentifier, std::string& trackData,
			int& fillBits);

	/**
	 * @brief TLB NMEA Message parser
	 *
	 * <b>TLB NMEA message fields</b>
	 * <i>Target Label</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID --TBL
	 * 1 | Target number ´'n' reported by device
	 * 2 | Label assigned to target 'n'
	 * ... | Additional label pairs
	 * n | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] trackNumbernLabel Vector of Pairs with Target Number and Target Label
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseTLB(const std::string& nmea,
			std::vector<std::pair<int, std::string>>& trackNumbernLabel);

	/**
	 * @brief OSD NMEA Message parser
	 *
	 * <b>OSD NMEA message fields</b><br>
	 * <i>Own ship data</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID --OSD
	 * 1 | Heading, degrees true
	 * 2 | Status, A = Data Valid
	 * 3 | Vessel Course, degrees True
	 * 4 | Course Reference
	 * 5 | Vessel Speed
	 * 6 | Speed Reference
	 * 7 | Vessel Set, degrees True
	 * 8 | Vessel drift (speed)
	 * 9 | Speed Units
	 * 10 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] heading Degrees from True North
	 * @param [out] status Data Validity
	 * @param [out] vesselCourse Ship Course
	 * @param [out] referenceCourse Course Relative or True North Reference
	 * @param [out] vesselSpeed Ship Speed
	 * @param [out] referenceSpeed Reference Speed
	 * @param [out] vesselSet Vessel Set
	 * @param [out] vesselDrift Vessel Drift (Speed)
	 * @param [out] speedUnits Speed Units
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseOSD(const std::string& nmea, double& heading,
			char& status, double& vesselCourse, char& referenceCourse,
			double& vesselSpeed, char& referenceSpeed, double& vesselSet,
			double& vesselDrift, char& speedUnits);

	/**
	 * @brief RSD NMEA Message parser
	 *
	 * <b>RSD NMEA message fields</b><br>
	 * <i>RADAR System Data</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID --RSD
	 * 1 | Original range from own ship
	 * 2 | Original bearing degrees from 0
	 * 3 | Variable range marker 1 range
	 * 4 | Bearing Line 1(EBL1) degrees from 0
	 * 5 | origin2 range
	 * 6 | origin2 bearing
	 * 7 | VRM2, range
	 * 8 | EBL2 degrees
	 * 9 | Cursor range, from own ship
	 * 10 | Cursor bearing, degrees CW from 0
	 * 11 | range scale (maximum)
	 * 12 | range units, K/N/S
	 * 13 | display rotation
	 * 14 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] origin1Range Original range from own ship
	 * @param [out] origin1BearingDegrees Original bearing degrees from 0
	 * @param [out] variableRangeMarker1 Variable range marker 1 range
	 * @param [out] bearingLine1 Bearing Line 1(EBL1) degrees from 0
	 * @param [out] origin2Range Origin2 range
	 * @param [out] origin2Bearing Origin2 bearing
	 * @param [out] vrm2 VRM2, range
	 * @param [out] ebl2 EBL2 degrees
	 * @param [out] cursorRange Cursor range, from own ship
	 * @param [out] cursorBearing Cursor bearing, degrees CW from 0
	 * @param [out] rangeScale range scale (maximum)
	 * @param [out] rangeUnits range units, K/N/S
	 * @param [out] displayRotation display rotation
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseRSD(const std::string& nmea,
			double& origin1Range, double& origin1BearingDegrees,
			double& variableRangeMarker1, double& bearingLine1,
			double& origin2Range, double& origin2Bearing, double& vrm2,
			double& ebl2, double& cursorRange, double& cursorBearing,
			double& rangeScale, char& rangeUnits, char& displayRotation);

	/**
	 * @brief VDM NMEA Message parser
	 *
	 * <b>VDM NMEA message fields</b><br>
	 * <i>AIS VHF data-link message</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID --VDM
	 * 1 | Total hex number of sentences needed to transfer the message, 1 to FF
	 * 2 | Hex sentence number, 1 to FF
	 * 3 | Sequential message identifier, 0 to 9
	 * 4 | AIS Channel
	 * 5 | Encapsulated data
	 * 6 | Number of fill-bits, 0 to 5
	 * 7 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] totalLines Total lines needed to transfer the binary message
	 * @param [out] lineCount Current line sentence number
	 * @param [out] sequenceIdentifier Sequence identifier
	 * @param [out] aisChannel AIS Channel
	 * @param [out] encodedData Encapsulated tracked target data
	 * @param [out] fillBits Number of fill-bits, 0 to 5
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseVDM(const std::string& nmea, int& totalLines,
			int& lineCount, int& sequenceIdentifier, char& aisChannel,
			std::string& encodedData, int& fillBits);

	/**
	 * @brief VDO NMEA Message parser
	 *
	 * <b>VDO NMEA message fields</b><br>
	 * <i>AIS VHF data-link own-vessel report</i>
	 *
	 * Field | Meaning
	 * ------|---------
	 * 0 | Message ID --VDO
	 * 1 | Total hex number of sentences needed to transfer the message, 1 to FF
	 * 2 | Hex sentence number, 1 to FF
	 * 3 | Sequential message identifier, 0 to 9
	 * 4 | AIS Channel
	 * 5 | Encapsulated data
	 * 6 | Number of fill-bits, 0 to 5
	 * 7 | Checksum
	 *
	 * @param [in] nmea String with NMEA Sentence
	 * @param [out] totalLines Total lines needed to transfer the binary message
	 * @param [out] lineCount Current line sentence number
	 * @param [out] sequenceIdentifier Sequence identifier
	 * @param [out] aisChannel AIS Channel
	 * @param [out] encodedData Encapsulated tracked target data
	 * @param [out] fillBits Number of fill-bits, 0 to 5
	 *
	 * @return Bitset each index represents the validity of each output parameter respectively.
	 */
	static NmeaParserResult parseVDO(const std::string& nmea, int& totalLines,
			int& lineCount, int& sequenceIdentifier, char& aisChannel,
			std::string& encodedData, int& fillBits);

	/**
	 * @brief Parse TTD Binary data
	 *
	 * @param [in] trackData Track Binary Encoded Data
	 * @param [out] tracks Vector filled with each Target
	 *
	 * @return True on success.
	 */
	static bool parseTTDPayload(const std::string& trackData,
			std::vector<NmeaTrackData>& tracks);

	/**
	 * @brief Parse AIS Message Type
	 *
	 * @param [in] encodedData AIS Binary Encoded Data
	 * @param [out] messageType Message type for the given data
	 *
	 * @return True on success.
	 */
	static bool parseAISMessageType(const std::string& encodedData,
			Nmea_AisMessageType& messageType);

	/**
	 * @brief Parse AIS Position Report Class A - Types 1, 2 and 3
	 *
	 * @param [in] encodedData AIS Binary Encoded Data
	 * @param [out] data Struct filled with the decoded data
	 *
	 * @return True on success.
	 */
	static bool parseAISPositionReportClassA(const std::string& encodedData,
			AISPositionReportClassA& data);

	/**
	 * @brief Parse AIS Base Station Report - Type 4
	 *
	 * @param [in] encodedData AIS Binary Encoded Data
	 * @param [out] data Struct filled with the decoded data
	 *
	 * @return True on success.
	 */
	static bool parseAISBaseStationReport(const std::string& encodedData,
			AISBaseStationReport& data);

	/**
	 * @brief Parse AIS Static And Voyage Related Data - Type 5
	 *
	 * @param [in] encodedData AIS Binary Encoded Data
	 * @param [out] data Struct filled with the decoded data
	 *
	 * @return True on success.
	 */
	static bool parseAISStaticAndVoyageRelatedData(
			const std::string& encodedData,
			AISStaticAndVoyageRelatedData& data);

	/**
	 * @brief Parse AIS Standard Class B CS Position Report - Type 18
	 *
	 * @param [in] encodedData AIS Binary Encoded Data
	 * @param [out] data Struct filled with the decoded data
	 *
	 * @return True on success.
	 */
	static bool parseAISStandardClassBCSPositionReport(
			const std::string& encodedData,
			AISStandardClassBCSPositionReport& data);

	/**
	 * @brief Parse AIS Static Data Report - Type 24
	 *
	 * @param [in] encodedData AIS Binary Encoded Data
	 * @param [out] data Struct filled with the decoded data
	 *
	 * @return True on success.
	 */
	static bool parseAISStaticDataReport(const std::string& encodedData,
			AISStaticDataReport& data);

private:
	class impl;

	/**
	 * @brief Private constructor. Prevents creating of class instance.
	 */
	NmeaParser();

};

#endif /* SRC_NMEAPARSER_H_ */
