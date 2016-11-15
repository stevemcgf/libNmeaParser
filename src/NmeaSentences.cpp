/*
 * NmeaSentences.cpp
 *
 *  Created on: May 12, 2016
 *      Author: steve
 */

#include "NmeaSentences.h"

const std::map<std::string, std::string> NmeaTalkerIdMap::mapTalkerId = { {
		"AB", "Independent AIS Base Station" }, { "AD",
		"Dependent AIS Base Station" }, { "AG", "Autopilot - General" }, { "AP",
		"Autopilot - Magnetic" }, { "BN",
		"Bridge navigational watch alarm system" }, { "CC",
		"Computer - Programmed Calculator (obsolete)" }, { "CD",
		"Communications - Digital Selective Calling (DSC)" }, { "CM",
		"Computer - Memory Data (obsolete)" }, { "CS",
		"Communications - Satellite" }, { "CT",
		"Communications - Radio-Telephone (MF/HF)" }, { "CV",
		"Communications - Radio-Telephone (VHF)" }, { "CX",
		"Communications - Scanning Receiver" }, { "DE",
		"DECCA Navigation (obsolete)" }, { "DF", "Direction Finder" }, { "DU",
		"Duplex repeater station" }, { "EC",
		"Electronic Chart Display & Information System (ECDIS)" }, { "EP",
		"Emergency Position Indicating Beacon (EPIRB)" }, { "ER",
		"Engine Room Monitoring Systems" }, { "GP",
		"Global Positioning System (GPS)" }, { "HC",
		"Heading - Magnetic Compass" },
		{ "HE", "Heading - North Seeking Gyro" }, { "HN",
				"Heading - Non North Seeking Gyro" }, { "II",
				"Integrated Instrumentation" },
		{ "IN", "Integrated Navigation" }, { "LA", "Loran A (obsolete)" }, {
				"LC", "Loran C (obsolete)" }, { "MP",
				"Microwave Positioning System (obsolete)" }, { "NL",
				"Navigation light controller" }, { "OM",
				"OMEGA Navigation System (obsolete)" }, { "OS",
				"Distress Alarm System (obsolete)" }, { "RA",
				"RADAR and/or ARPA" }, { "SD", "Sounder, Depth" }, { "SN",
				"Electronic Positioning System, other/general" }, { "SS",
				"Sounder, Scanning" }, { "TI", "Turn Rate Indicator" }, { "TR",
				"TRANSIT Navigation System" }, { "U#",
				"# is a digit 0 … 9; User Configured" }, { "UP",
				"Microprocessor controller" }, { "VD",
				"Velocity Sensor, Doppler, other/general" }, { "DM",
				"Velocity Sensor, Speed Log, Water, Magnetic" }, { "VW",
				"Velocity Sensor, Speed Log, Water, Mechanical" }, { "WI",
				"Weather Instruments" }, { "YC",
				"Transducer - Temperature (obsolete)" }, { "YD",
				"Transducer - Displacement, Angular or Linear (obsolete)" }, {
				"YF", "Transducer - Frequency (obsolete)" }, { "YL",
				"Transducer - Level (obsolete)" }, { "YP",
				"Transducer - Pressure (obsolete)" }, { "YR",
				"Transducer - Flow Rate (obsolete)" }, { "YT",
				"Transducer - Tachometer (obsolete)" }, { "YV",
				"Transducer - Volume (obsolete)" }, { "YX", "Transducer" }, {
				"ZA", "Timekeeper - Atomic Clock" }, { "ZC",
				"Timekeeper - Chronometer" }, { "ZQ", "Timekeeper - Quartz" }, {
				"ZV", "Timekeeper - Radio Update, WWV or WWVH" } };

const std::map<std::string, std::string>& NmeaTalkerIdMap::get() {
	return mapTalkerId;
}

const std::map<std::string, std::string> NmeaSentenceMap::mapSentence = { {
		"AAM", "Waypoint Arrival Alarm" }, { "ALM", "GPS Almanac Data" }, {
		"APA", "Autopilot Sentence \"A\"" },
		{ "APB", "Autopilot Sentence \"B\"" }, { "BOD",
				"Bearing - Waypoint to Waypoint" }, { "BWC",
				"Bearing & Distance to Waypoint - Great Circle" }, { "BWR",
				"Bearing and Distance to Waypoint - Rhumb Line" }, { "BWW",
				"Bearing - Waypoint to Waypoint" },
		{ "DBK", "Depth Below Keel" }, { "DBS", "Depth Below Surface" }, {
				"DBT", "Depth below transducer" }, { "DCN", "Decca Position" },
		{ "DPT", "Depth of Water" }, { "DTM", "Datum Reference" }, { "FSI",
				"Frequency Set Information" }, { "GBS",
				"GPS Satellite Fault Detection" }, { "GGA",
				"Global Positioning System Fix Data" }, { "GLC",
				"Geographic Position, Loran - C" }, { "GLL",
				"Geographic Position - Latitude/Longitude" }, { "GNS",
				"Fix data" }, { "GRS", "GPS Range Residuals" }, { "GST",
				"GPS Pseudorange Noise Statistics" }, { "GSA",
				"GPS DOP and active satellites" },
		{ "GSV", "Satellites in view" }, { "GTD",
				"Geographic Location in Time Differences" }, { "GXA",
				"TRANSIT Position - Latitude/Longitude" }, { "HDG",
				"Heading - Deviation & Variation" }, { "HDM",
				"Heading - Magnetic" }, { "HDT", "Heading - 1" }, { "HFB",
				"Trawl Headrope to Footrope and Bottom" }, { "HSC",
				"Heading Steering Command" }, { "ITS",
				"Trawl Door Spread 2 Distance" }, { "LCD",
				"Loran - C Signal Data" }, { "MSK",
				"Control for a Beacon Receiver" }, { "MSS",
				"Beacon Receiver Status" },
		{ "MTW", "Mean Temperature of Water" },
		{ "MWV", "Wind Speed and Angle" }, { "OLN", "Omega Lane Numbers" }, {
				"OSD", "Own Ship Data" },
		{ "R00", "Waypoints in active route" }, { "RMA",
				"Recommended Minimum Navigation Information" }, { "RMB",
				"Recommended Minimum Navigation Information" }, { "RMC",
				"Recommended Minimum Navigation Information" }, { "ROT",
				"Rate Of Turn" }, { "RPM", "Revolutions" }, { "RSA",
				"Rudder Sensor Angle" }, { "RSD", "RADAR System Data" }, {
				"RTE", "RoutesSFI - Scanning Frequency Information" }, { "STN",
				"Multiple Data ID" }, { "TDS", "Trawl Door Spread Distance" }, {
				"TFI", "Trawl Filling Indicator" }, { "TPC",
				"Trawl Position Cartesian Coordinates" }, { "TPR",
				"Trawl Position Relative Vessel" }, { "TPT",
				"Trawl Position True" }, { "TRF", "TRANSIT Fix Data" }, { "TTM",
				"Tracked Target Message" },
		{ "VBW", "Dual Ground/Water Speed" }, { "VDR", "Set and Drift" }, {
				"VHW", "Water speed and heading" }, { "VLW",
				"Distance Traveled through Water" }, { "VPW",
				"Speed - Measured Parallel to Wind" }, { "VTG",
				"Track made good and Ground speed" }, { "VWR",
				"Relative Wind Speed and Angle" }, { "WCV",
				"Waypoint Closure Velocity" }, { "WNC",
				"Distance - Waypoint to Waypoint" }, { "WPL",
				"Waypoint Location" }, { "XDR", "Transducer Measurement" }, {
				"XTE", "Cross - Track Error, Measured" }, { "XTR",
				"Cross Track Error - Dead Reckoning" }, { "ZDA",
				"Time & Date - UTC, day, month, year and local time zone" }, {
				"ZFO", "UTC & Time from origin Waypoint" }, { "ZTG",
				"UTC & Time to Destination Waypoint" } };

const std::map<std::string, std::string>& NmeaSentenceMap::get() {
	return mapSentence;
}

std::string getNmeaTalkerId(const std::string& nmea) {
	return nmea.substr(1, 2);
}

std::string getNmeaSentence(const std::string& nmea) {
	return nmea.substr(3, 3);
}

std::string getNmeaTalkerIdName(const std::string& nmeaTalkerId) {
	try {
		return NmeaTalkerIdMap::get().at(nmeaTalkerId);
	} catch (std::out_of_range& e) {
		return "ID DESCONOCIDO";
	}
}

std::string getNmeaSentenceDesc(const std::string& nmeaSentence) {
	try {
		return NmeaSentenceMap::get().at(nmeaSentence);
	} catch (std::out_of_range& e) {
		return "MENSAJE DESCONOCIDO";
	}
}
