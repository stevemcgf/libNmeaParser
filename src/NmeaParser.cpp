/*
 * NmeaParser.cpp
 *
 *  Created on: Feb 3, 2016
 *      Author: steve
 */

#include "NmeaParser.h"

#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>
#include <boost/log/trivial.hpp>

#include <iostream>
#include <iomanip>

#undef SIDDN_DEBUG
#include "debug.hpp"
#include "error.hpp"

NmeaParser::NmeaParser() {

}

NmeaParser::~NmeaParser() {

}

bool NmeaParser::tokenizeSentence(const std::string& nmea,
		std::vector<std::string>& tokens) {

	tokens.clear();

	const boost::char_separator<char> sep(",*", 0, boost::keep_empty_tokens);
	const boost::tokenizer<boost::char_separator<char>> t(nmea.begin(),
			nmea.end(), sep);
	std::copy(t.begin(), t.end(), std::back_inserter(tokens));
	return tokens.size() > 0;  // parse is good if there is at least one token
}

const boost::regex hms("(\\d{2})(\\d{2})(\\d{2})(?:(.\\d*))?");
bool NmeaParser::decodeTime(std::vector<std::string>::iterator &i,
		boost::posix_time::time_duration& out,
		const boost::posix_time::time_duration& def) {
	bool ret = false;
	boost::smatch m;
	if (boost::regex_match(*i, m, hms)) {
		ret = true;
		Dout(dbg_nmeaparser, " decodeTime: time " << *i);
		i++;  // consume the match
		const boost::posix_time::hours hr(boost::lexical_cast<int>(m[1].str()));
		const boost::posix_time::minutes min(
				boost::lexical_cast<int>(m[2].str()));
		const boost::posix_time::seconds sec(
				boost::lexical_cast<int>(m[3].str()));
		const boost::posix_time::milliseconds ms(
				(m[4].matched ?
						boost::lexical_cast<int>(
								boost::lexical_cast<double>(m[4].str())
										* 1000) :
						0.0));
		out = boost::posix_time::time_duration(hr + min + sec + ms);
	} else {
		out = def;
	}

	return ret;
}

double NmeaParser::toDecimalDegree(double degrees, double minutes,
		double seconds, char hemisphere) {
	degrees += minutes / 60.0 + seconds / 3600.0;
	if ((hemisphere == 'S') || (hemisphere == 'W'))
		degrees = -degrees;
	return degrees;
}

// because fields to convert may be empty
template<typename Target>
bool NmeaParser::decodeDefault(std::vector<std::string>::iterator &i,
		Target &out, const Target& def) {
	bool ret = false;
	try {
		out = boost::lexical_cast<Target>(*i);
		ret = true;
		i++; // consume valor.
	} catch (boost::bad_lexical_cast &) {
		out = def;
	}

	return ret;
}

const boost::regex latlng("(\\d{2,3})(\\d{2}.\\d+)");
bool NmeaParser::decodeLatLng(std::vector<std::string>::iterator &i,
		double& out, const double& def) {
	bool ret = false;
	boost::smatch m;
	if (boost::regex_match(*i, m, latlng)) {
		ret = true;
		Dout(dbg_nmeaparser, "decodeLatLng: num " << *i);
		i++; // consume the match
		double deg = boost::lexical_cast<double>(m[1].str());
		double min = boost::lexical_cast<double>(m[2].str());
		Dout(dbg_nmeaparser, "decodeLatLng: hem " << *i);
		char hemisphere = (*i).at(0);
		i++; // consume hemisphere
		out = toDecimalDegree(deg, min, 0.0, hemisphere);
	} else {
		out = def;
	}

	return ret;
}

const boost::regex dmy("(\\d{2})(\\d{2})(\\d{2})");
bool NmeaParser::decodeDate(std::vector<std::string>::iterator &i,
		boost::gregorian::date& out, const boost::gregorian::date& def) {
	bool ret = false;
	boost::smatch m;
	if (boost::regex_match(*i, m, dmy)) {
		ret = true;
		Dout(dbg_nmeaparser, "decodeDate: date " << *i);
		i++; // consume the match
		int day = boost::lexical_cast<int>(m[1].str());
		int month = boost::lexical_cast<int>(m[2].str());
		int year = boost::lexical_cast<int>(m[3].str()) + 2000;
		out = boost::gregorian::date(year, month, day);
	} else {
		out = def;
	}

	return ret;
}

bool NmeaParser::decodeString(std::vector<std::string>::iterator &i,
		std::string& out, const std::string& def) {
	bool ret = false;
	try {
		out = boost::lexical_cast<std::string>(*i);

		if (out.size() > 0) {
			i++; // consume valor.
			ret = true;
		} else {
			out = def;
		}
	} catch (boost::bad_lexical_cast &) {
		out = def;
	}

	return ret;
}
//------------------------------------- Parseo de tramas NMEA -------------------------------
// definicion de valores por defecto
boost::posix_time::time_duration deftime(0, 0, 0, 0);
boost::gregorian::date defdate(1400, 1, 1);
const double defLatLong = 99.999;
char c = 'X';
std::string defString = "XXXX";

NmeaParserResult NmeaParser::parseZDA(const std::string& nmea,
		boost::posix_time::time_duration& mtime, int& day, int& month,
		int& year, int& localZoneHours, int& localZoneMinutes) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 01  GPZDA --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint zdaSizeField = 8;

	if (zdaSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		Dout(dbg_nmeaparser, "parseZDA: 1 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "ZDA") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeTime(itNmea, mtime, deftime)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "mtime = " << mtime);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<int>(itNmea, day, 0)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "day = " << day);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<int>(itNmea, month, 0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "month = " << month);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<int>(itNmea, year, 0)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "year = " << year);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<int>(itNmea, localZoneHours, 0)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "localZoneHours = " << localZoneHours);

			/*------------ Position 05 ---------------*/
			if (!decodeDefault<int>(itNmea, localZoneMinutes, 0)) {
				ret.set(5, true);
			} Dout(dbg_nmeaparser, "localZoneMinutes = " << localZoneMinutes);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << zdaSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseGLL(const std::string& nmea, double& latitude,
		double& longitude, boost::posix_time::time_duration& mtime,
		char& status, char& modeIndicator) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 02  GPGLL --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint gllSizeField = 9;

	if (gllSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		Dout(dbg_nmeaparser, "parseGLL: 2 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "GLL") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeLatLng(itNmea, latitude, defLatLong)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "latitude = " << latitude);

			/*------------ Position 01 ---------------*/
			if (!decodeLatLng(itNmea, longitude, defLatLong)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "longitude = " << longitude);

			/*------------ Position 02 ---------------*/
			if (!decodeTime(itNmea, mtime, deftime)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "mtime = " << mtime);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<char>(itNmea, status, c)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "status = " << status);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<char>(itNmea, modeIndicator, c)) {
				ret.set(4, true);
			} Dout(dbg_nmeaparser, "modeIndicator = " << modeIndicator);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << gllSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseGGA(const std::string& nmea,
		boost::posix_time::time_duration& mtime, double& latitude,
		double& longitude, Nmea_GPSQualityIndicator& quality, int& numSV,
		double& hdop, double& orthometricheight, double& geoidseparation,
		double& agediffgps, std::string& refid) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 03  GPGGA --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint ggaSizeField = 16;

	if (ggaSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseGGA: 3 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "GGA") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeTime(itNmea, mtime, deftime)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "mtime = " << mtime);

			/*------------ Position 01 ---------------*/
			if (!decodeLatLng(itNmea, latitude, defLatLong)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "latitude = " << latitude);

			/*------------ Position 02 ---------------*/
			if (!decodeLatLng(itNmea, longitude, defLatLong)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "longitude = " << longitude);

			/*------------ Position 03 ---------------*/
			int qualityIndicator;
			if (!decodeDefault<int>(itNmea, qualityIndicator, 0)) {
				ret.set(3, true);
				itNmea++;
			}
			//Dout(dbg_nmeaparser, "qualityIndicator = " << qualityIndicator;

			switch (qualityIndicator) {
			case 0: {
				quality = Nmea_GPSQualityIndicator::FixNotValid;
				break;
			}
			case 1: {
				quality = Nmea_GPSQualityIndicator::GPSFix;
				break;
			}
			case 2: {
				quality = Nmea_GPSQualityIndicator::GPSFixDifferential;
				break;
			}
			case 3: {
				quality = Nmea_GPSQualityIndicator::RealTimeKinematic;
				break;
			}
			case 4: {
				quality = Nmea_GPSQualityIndicator::RealTimeKinematicOmniStar;
				break;
			}
			default: {
				ret.set(3, true);
				quality = Nmea_GPSQualityIndicator::FixNotValid;
				break;
			}
			} Dout(dbg_nmeaparser, "quality = " << quality);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<int>(itNmea, numSV, 0)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "numSV = " << numSV);

			/*------------ Position 05 ---------------*/
			if (!decodeDefault<double>(itNmea, hdop, 0)) {
				ret.set(5, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "hdop = " << hdop);

			/*------------ Position 06 ---------------*/
			if (!decodeDefault<double>(itNmea, orthometricheight, 0)) {
				ret.set(6, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "orthometricheight = " << orthometricheight);
			itNmea++;

			/*------------ Position 07 ---------------*/
			if (!decodeDefault<double>(itNmea, geoidseparation, 0)) {
				ret.set(7, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "geoidseparation = " << geoidseparation);
			itNmea++;

			/*------------ Position 08 ---------------*/
			if (!decodeDefault<double>(itNmea, agediffgps, 0)) {
				ret.set(8, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "agediffgps = " << agediffgps);

			/*------------ Position 09 ---------------*/
			if (!decodeString(itNmea, refid, defString)) {
				ret.set(9, true);
			} Dout(dbg_nmeaparser, "refid = " << refid);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << ggaSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseVTG(const std::string& nmea,
		double& coursetrue, double& coursemagnetic, double& speedknots,
		double& speedkph) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 04  GPVTG --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint vtgSizeField = 11;

	if (vtgSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseVTG: 4 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "VTG") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, coursetrue, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "coursetrue = " << coursetrue);
			itNmea++;
			/*------------ Position 01 ---------------*/
			if (!decodeDefault<double>(itNmea, coursemagnetic, 0)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "coursemagnetic = " << coursemagnetic);
			itNmea++;
			/*------------ Position 02 ---------------*/
			if (!decodeDefault<double>(itNmea, speedknots, 0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "speedknots = " << speedknots);
			itNmea++;
			/*------------ Position 03 ---------------*/
			if (!decodeDefault<double>(itNmea, speedkph, 0)) {
				ret.set(3, true);
			} Dout(dbg_nmeaparser, "speedkph = " << speedkph);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << vtgSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseRMC(const std::string& nmea,
		boost::posix_time::time_duration& mtime, double& latitude,
		double& longitude, double& speedknots, double& coursetrue,
		boost::gregorian::date& mdate, double& magneticvar) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 05  GPRMC --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint rmcSizeField = 15;

	if (rmcSizeField >= fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseRMC: 5 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "RMC") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeTime(itNmea, mtime, deftime)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "mtime = " << mtime);
			itNmea++;

			/*------------ Position 01 ---------------*/
			if (!decodeLatLng(itNmea, latitude, defLatLong)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "latitude = " << latitude);

			/*------------ Position 02 ---------------*/
			if (!decodeLatLng(itNmea, longitude, defLatLong)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "longitude = " << longitude);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<double>(itNmea, speedknots, 0)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "speedknots = " << speedknots);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<double>(itNmea, coursetrue, 0)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "coursetrue = " << coursetrue);

			/*------------ Position 05 ---------------*/
			if (!decodeDate(itNmea, mdate, defdate)) {
				ret.set(5, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "mdate = " << mdate);

			/*------------ Position 06 ---------------*/
			if (!decodeDefault<double>(itNmea, magneticvar, 0)) {
				ret.set(6, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "magneticvar = " << magneticvar);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << rmcSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseWPL(const std::string& nmea, double& latitude,
		double& longitude, std::string& waypointName) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 06  GPWPL --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint wplSizeField = 7;

	if (wplSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseWPL: 6 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "WPL") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeLatLng(itNmea, latitude, defLatLong)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "latitude = " << latitude);

			/*------------ Position 01 ---------------*/
			if (!decodeLatLng(itNmea, longitude, defLatLong)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "longitude = " << longitude);

			/*------------ Position 02 ---------------*/
			if (!decodeString(itNmea, waypointName, defString)) {
				ret.set(2, true);
			} Dout(dbg_nmeaparser, "waypointName = " << waypointName);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << wplSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseRTE(const std::string& nmea,
		double& messagesTransmitted, double& messageNumber, char& messageMode,
		std::string& routeIdentifier, std::vector<std::string>& revenue1) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 07  GPRTE --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint rteSizeField = 7;

	if (rteSizeField <= fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseRTE: 7 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "RTE") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, messagesTransmitted, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "messagesTransmitted = " << messagesTransmitted);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<double>(itNmea, messageNumber, 0)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "messageNumber = " << messageNumber);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<char>(itNmea, messageMode, c)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "messageMode = " << messageMode);

			/*------------ Position 03 ---------------*/
			if (!decodeString(itNmea, routeIdentifier, defString)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "routeIdentifier = " << routeIdentifier);

			std::string WaypointIdentifier;
			for (uint i = 0; i < (fields.size() - 6); i++) {
				WaypointIdentifier = (*itNmea);
				//Dout(dbg_nmeaparser, "WaypointIdentifier "<< i + 1 <<" = " << WaypointIdentifier);
				itNmea++;
				revenue1.push_back(WaypointIdentifier);
			}
//			Dout(dbg_nmeaparser, "revenue1.size = " << revenue1.size());
//			if ((revenue1.size()-4) < 1){
//				ret.set(4, true);
//			}

			// impresion

			for (std::vector<std::string>::iterator it = revenue1.begin(); it != revenue1.end(); ++it) {
				Dout(dbg_nmeaparser, "item: " << *it  );
			}
		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << rteSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseVHW(const std::string& nmea,
		double& speedInKnots, char& knots, double& speedInKmH,
		char& kilometers) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 08  VDVHW --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint vhwSizeField = 10;

	if (vhwSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseVHW: 8 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "VHW") {
			itNmea++;
			itNmea++;
			itNmea++;
			itNmea++;
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, speedInKnots, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "speedInKnots = " << speedInKnots);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<char>(itNmea, knots, c)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "knots = " << knots);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<double>(itNmea, speedInKmH, 0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "speedInKmH = " << speedInKmH);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<char>(itNmea, kilometers, c)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "kilometers = " << kilometers);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << vhwSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseMTW(const std::string& nmea, double& degrees,
		char& celcius) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 09  VDMTW --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint mtwSizeField = 4;

	if (mtwSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseMTW: 9 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "MTW") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, degrees, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "degrees = " << degrees);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<char>(itNmea, celcius, c)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "celcius = " << celcius);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << mtwSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseVBW(const std::string& nmea,
		double& longitudinalWaterSpeed, double& transverseWaterSpeed,
		char& waterDataStatus, double& longitudinalGroundSpeed,
		double& transverseGroundSpeed, char& groundDataStatus) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 10  VDVBW --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint vbwSizeField = 12;

	if (vbwSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseVBW: 10 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "VBW") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, longitudinalWaterSpeed,
					0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "longitudinalWaterSpeed = " << longitudinalWaterSpeed);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<double>(itNmea, transverseWaterSpeed, 0)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "transverseWaterSpeed = " << transverseWaterSpeed);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<char>(itNmea, waterDataStatus, c)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "waterDataStatus = " << waterDataStatus);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<double>(itNmea, longitudinalGroundSpeed,
					0)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "longitudinalGroundSpeed = " << longitudinalGroundSpeed);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<double>(itNmea, transverseGroundSpeed,
					0)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "transverseGroundSpeed = " << transverseGroundSpeed);

			/*------------ Position 05 ---------------*/
			if (!decodeDefault<char>(itNmea, groundDataStatus, c)) {
				ret.set(5, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "groundDataStatus = " << groundDataStatus);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << vbwSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseVLW(const std::string& nmea,
		double& totalCumulativeDistance, char& nauticalMiles1,
		double& distanceSinceReset, char& nauticalMiles2) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 11  VDVLW --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint vlwSizeField = 6;

	if (vlwSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseVLW: 11 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "VLW") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, totalCumulativeDistance,
					0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "totalCumulativeDistance = " << totalCumulativeDistance);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<char>(itNmea, nauticalMiles1, c)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "nauticalMiles1 = " << nauticalMiles1);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<double>(itNmea, distanceSinceReset, 0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "distanceSinceReset = " << distanceSinceReset);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<char>(itNmea, nauticalMiles2, c)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "nauticalMiles2 = " << nauticalMiles2);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << vlwSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseDPT(const std::string& nmea,
		double& waterDepthRelativeToTheTransducer, double& offsetFromTransducer,
		double& maximumRangeScaleInUse) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 12  SDDPT --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint dptSizeField = 5;

	if (dptSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseDPT: 12 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "DPT") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea,
					waterDepthRelativeToTheTransducer, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "waterDepthRelativeToTheTransducer = " << waterDepthRelativeToTheTransducer);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<double>(itNmea, offsetFromTransducer, 0)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "offsetFromTransducer = " << offsetFromTransducer);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<double>(itNmea, maximumRangeScaleInUse,
					0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "maximumRangeScaleInUse = " << maximumRangeScaleInUse);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << dptSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseDBT(const std::string& nmea,
		double& waterDepthInFeet, char& feet, double& waterDepthInMeters,
		char& meters, double& waterDepthInFathoms, char& fathoms) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 13  SDDBT --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint dbtSizeField = 8;

	if (dbtSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseDBT: 13 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "DBT") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, waterDepthInFeet, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "waterDepthInFeet = " << waterDepthInFeet);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<char>(itNmea, feet, c)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "feet = " << feet);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<double>(itNmea, waterDepthInMeters, 0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "waterDepthInMeters = " << waterDepthInMeters);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<char>(itNmea, meters, c)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "meters = " << meters);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<double>(itNmea, waterDepthInFathoms, 0)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "waterDepthInFathoms = " << waterDepthInFathoms);

			/*------------ Position 05 ---------------*/
			if (!decodeDefault<char>(itNmea, fathoms, c)) {
				ret.set(5, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "fathoms = " << fathoms);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << dbtSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseDBK(const std::string& nmea,
		double& depthBelowKeelFeet, char& feet, double& depthBelowKeelMeters,
		char& meters, double& depthBelowKeelFathoms, char& fathoms) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 14  SDDBK --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint dbkSizeField = 8;

	if (dbkSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseDBK: 14 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "DBK") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, depthBelowKeelFeet, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "depthBelowKeelFeet = " << depthBelowKeelFeet);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<char>(itNmea, feet, c)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "feet = " << feet);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<double>(itNmea, depthBelowKeelMeters, 0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "depthBelowKeelMeters = " << depthBelowKeelMeters);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<char>(itNmea, meters, c)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "meters = " << meters);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<double>(itNmea, depthBelowKeelFathoms,
					0)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "depthBelowKeelFathoms = " << depthBelowKeelFathoms);

			/*------------ Position 05 ---------------*/
			if (!decodeDefault<char>(itNmea, fathoms, c)) {
				ret.set(5, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "fathoms = " << fathoms);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << dbkSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parsePSKPDPT(const std::string& nmea,
		double& waterDepthRelativeToTheTransducer, double& offsetFromTransducer,
		double& maximumRangeScaleInUse, int& bottomEchoStrength,
		int& echoSounderChannelNumber, std::string& transducerLocation) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 15  PSKPDPT --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint dptSizeField = 8;

	if (dptSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parsePSKPDPT: 15 = " << *itNmea);
		if ((*itNmea).substr(1, 7) == "PSKPDPT") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea,
					waterDepthRelativeToTheTransducer, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "waterDepthRelativeToTheTransducer = " << waterDepthRelativeToTheTransducer);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<double>(itNmea, offsetFromTransducer, 0)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "offsetFromTransducer = " << offsetFromTransducer);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<double>(itNmea, maximumRangeScaleInUse,
					0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "maximumRangeScaleInUse = " << maximumRangeScaleInUse);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<int>(itNmea, bottomEchoStrength, 0)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "bottomEchoStrength = " << bottomEchoStrength);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<int>(itNmea, echoSounderChannelNumber,
					0)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "echoSounderChannelNumber = " << echoSounderChannelNumber);

			/*------------ Position 05 ---------------*/
			if (!decodeString(itNmea, transducerLocation, defString)) {
				ret.set(5, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "transducerLocation = " << transducerLocation);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << dptSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseHDT(const std::string& nmea,
		double& headingDegreesTrue, char& t) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 16  HCHDT --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint hdtSizeField = 4;

	if (hdtSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseHDT: 16 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "HDT") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, headingDegreesTrue, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "headingDegreesTrue = " << headingDegreesTrue);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<char>(itNmea, t, c)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "t = " << t);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << hdtSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseHDG(const std::string& nmea,
		double& magneticSensorHeadingInDegrees,
		double& magneticDeviationDegrees, char& magneticDeviationDirection,
		double& magneticVariationDegrees, char& magneticVariationDirection) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 17  HCHDG --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint hdgSizeField = 7;

	if (hdgSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseHDG: 17 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "HDG") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea,
					magneticSensorHeadingInDegrees, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "magneticSensorHeadingInDegrees = " << magneticSensorHeadingInDegrees);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<double>(itNmea, magneticDeviationDegrees,
					0)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "magneticDeviationDegrees = " << magneticDeviationDegrees);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<char>(itNmea, magneticDeviationDirection,
					c)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "magneticDeviationDirection = " << magneticDeviationDirection);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<double>(itNmea, magneticVariationDegrees,
					0)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "magneticVariationDegrees = " << magneticVariationDegrees);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<char>(itNmea, magneticVariationDirection,
					c)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "magneticVariationDirection = " << magneticVariationDirection);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << hdgSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseHDM(const std::string& nmea,
		double& headingDegreesMagnetic, char& magnetic) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 18  HCHDM --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint hdmSizeField = 4;

	if (hdmSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseHDM: 18 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "HDM") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, headingDegreesMagnetic,
					0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "headingDegreesMagnetic = " << headingDegreesMagnetic);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<char>(itNmea, magnetic, c)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "magnetic = " << magnetic);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << hdmSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseROT(const std::string& nmea,
		double& rateOfTurn, char& status) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 19  TIROT --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint rotSizeField = 4;

	if (rotSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseROT: 19 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "ROT") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, rateOfTurn, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "rateOfTurn = " << rateOfTurn);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<char>(itNmea, status, c)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "status = " << status);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << rotSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseMWV(const std::string& nmea,
		double& windAngle, char& reference, double& windSpeed,
		char& windSpeedUnits, char& sensorStatus) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 20  IIMWV --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint mwvSizeField = 7;

	if (mwvSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseMWV: 20 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "MWV") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, windAngle, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "windAngle = " << windAngle);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<char>(itNmea, reference, c)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "reference = " << reference);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<double>(itNmea, windSpeed, 0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "windSpeed = " << windSpeed);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<char>(itNmea, windSpeedUnits, c)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "windSpeedUnits = " << windSpeedUnits);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<char>(itNmea, sensorStatus, c)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "sensorStatus = " << sensorStatus);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << mwvSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseMWD(const std::string& nmea,
		double& trueWindDirection, char& t, double& magneticWindDirection,
		char& magnetic, double& windSpeedKnots, char& knots,
		double& windSpeedMeters, char& meters) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 21  WIMWD --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint mwdSizeField = 10;

	if (mwdSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseMWD: 21 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "MWD") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, trueWindDirection, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "trueWindDirection = " << trueWindDirection);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<char>(itNmea, t, c)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "t = " << t);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<double>(itNmea, magneticWindDirection,
					0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "magneticWindDirection = " << magneticWindDirection);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<char>(itNmea, magnetic, c)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "magnetic = " << magnetic);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<double>(itNmea, windSpeedKnots, 0)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "windSpeedKnots = " << windSpeedKnots);

			/*------------ Position 05 ---------------*/
			if (!decodeDefault<char>(itNmea, knots, c)) {
				ret.set(5, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "knots = " << knots);

			/*------------ Position 06 ---------------*/
			if (!decodeDefault<double>(itNmea, windSpeedMeters, 0)) {
				ret.set(6, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "windSpeedMeters = " << windSpeedMeters);

			/*------------ Position 07 ---------------*/
			if (!decodeDefault<char>(itNmea, meters, c)) {
				ret.set(7, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "meters = " << meters);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << mwdSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseXDR(const std::string& nmea,
		char& typeOfSensorTemperature, double& temperatureReading,
		char& centigrade, std::string& nameOfTransducer,
		char& typeOfSensorPressure, double& pressureReading, char& bars,
		std::string& nameOfPressureSensor, char& typeOfSensorHumidity,
		double& humidity, char& humidityUnitsOfMeasurePercent,
		std::string& nameOfRelativeHumiditySensor) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 22  IIXDR --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint xdrSizeField = 14;

	if (xdrSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseXDR: 22 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "XDR") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<char>(itNmea, typeOfSensorTemperature,
					c)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "typeOfSensorTemperature = " << typeOfSensorTemperature);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<double>(itNmea, temperatureReading, 0)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "temperatureReading = " << temperatureReading);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<char>(itNmea, centigrade, c)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "centigrade = " << centigrade);

			/*------------ Position 03 ---------------*/
			if (!decodeString(itNmea, nameOfTransducer, defString)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "nameOfTransducer = " << nameOfTransducer);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<char>(itNmea, typeOfSensorPressure, c)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "typeOfSensorPressure = " << typeOfSensorPressure);

			/*------------ Position 05 ---------------*/
			if (!decodeDefault<double>(itNmea, pressureReading, 0)) {
				ret.set(5, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "pressureReading = " << pressureReading);

			/*------------ Position 06 ---------------*/
			if (!decodeDefault<char>(itNmea, bars, c)) {
				ret.set(6, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "bars = " << bars);

			/*------------ Position 07 ---------------*/
			if (!decodeString(itNmea, nameOfPressureSensor, defString)) {
				ret.set(7, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "nameOfPressureSensor = " << nameOfPressureSensor);

			/*------------ Position 08 ---------------*/
			if (!decodeDefault<char>(itNmea, typeOfSensorHumidity, c)) {
				ret.set(8, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "typeOfSensorHumidity = " << typeOfSensorHumidity);

			/*------------ Position 09 ---------------*/
			if (!decodeDefault<double>(itNmea, humidity, 0)) {
				ret.set(9, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "humidity = " << humidity);

			/*------------ Position 10 ---------------*/
			if (!decodeDefault<char>(itNmea,
					humidityUnitsOfMeasurePercent, c)) {
				ret.set(10, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "humidityUnitsOfMeasurePercent = " << humidityUnitsOfMeasurePercent);

			/*------------ Position 11 ---------------*/
			if (!decodeString(itNmea, nameOfRelativeHumiditySensor,
					defString)) {
				ret.set(11, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "nameOfRelativeHumiditySensor = " << nameOfRelativeHumiditySensor);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << xdrSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseTTD(const std::string& nmea, int& aaa,
		int& bbb, int& ccc, std::string& ddd, int& eee) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 23  IITTD --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint ttdSizeField = 7;

	if (ttdSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseTTD: 23 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "TTD") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<int>(itNmea, aaa, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "aaa = " << aaa);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<int>(itNmea, bbb, 0)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "bbb = " << bbb);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<int>(itNmea, ccc, 0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "ccc = " << ccc);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<std::string>(itNmea, ddd, defString)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "ddd = " << ddd);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<int>(itNmea, eee, 0)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "eee = " << eee);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << ttdSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseTLB(const std::string& nmea,
		std::vector<std::pair<double, std::string> >& revenue) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 24  RATLB --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint tlbSizeField = 2;

	double targetNumber;
	std::string secondData;

	if (tlbSizeField <= fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseTLB: 24 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "TLB") {
			itNmea++;

			std::pair<double, std::string> aPair;
			for (uint i = 0; i < (fields.size() - 2) / 2; i++) {
				//targetNumber = lexical_cast_default<double>(*itNmea++, 0);

				/*------------ targetNumber ---------------*/
				if (!decodeDefault<double>(itNmea, targetNumber, 0)) {
					//ret.set(0, true);
					//itNmea++;
				}
				//Dout(dbg_nmeaparser, "targetNumber = " << targetNumber);

				/*------------ secondData ---------------*/
				if (!decodeDefault<std::string>(itNmea, secondData,
						defString)) {
					itNmea++;
				}
				//Dout(dbg_nmeaparser, "secondData = " << secondData);

				aPair.first = targetNumber;
				aPair.second = secondData;

				revenue.push_back(aPair);
				//Dout(dbg_nmeaparser, "i = " << i);

			}
			// impresion

			for (std::vector<std::pair<double, std::string>>::iterator it =
					revenue.begin(); it != revenue.end(); ++it) {
				Dout(dbg_nmeaparser, "pairs: " << it->first << " --> " << it->second.c_str());
			}

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << tlbSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseOSD(const std::string& nmea, double& heading,
		char& status, double& vesselCourse, char& referenceCourse,
		double& vesselSpeed, char& referenceSpeed, double& vesselSet,
		double& vesselDrift, char& speedUnits) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 25  RAOSD --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint osdSizeField = 11;

	if (osdSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseOSD: 25 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "OSD") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, heading, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "heading = " << heading);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<char>(itNmea, status, c)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "status = " << status);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<double>(itNmea, vesselCourse, 0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "vesselCourse = " << vesselCourse);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<char>(itNmea, referenceCourse, c)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "referenceCourse = " << referenceCourse);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<double>(itNmea, vesselSpeed, 0)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "vesselSpeed = " << vesselSpeed);

			/*------------ Position 05 ---------------*/
			if (!decodeDefault<char>(itNmea, referenceSpeed, c)) {
				ret.set(5, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "referenceSpeed = " << referenceSpeed);

			/*------------ Position 06 ---------------*/
			if (!decodeDefault<double>(itNmea, vesselSet, 0)) {
				ret.set(6, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "vesselSet = " << vesselSet);

			/*------------ Position 07 ---------------*/
			if (!decodeDefault<double>(itNmea, vesselDrift, 0)) {
				ret.set(7, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "vesselDrift = " << vesselDrift);

			/*------------ Position 08 ---------------*/
			if (!decodeDefault<char>(itNmea, speedUnits, c)) {
				ret.set(8, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "speedUnits = " << speedUnits);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << osdSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

NmeaParserResult NmeaParser::parseRSD(const std::string& nmea,
		double& origin1Range, double& origin1BearingDegrees,
		double& variableRangeMarker1, double& bearingLine1,
		double& origin2Range, double& origin2Bearing, double& vrm2,
		double& ebl2, double& cursorRange, double& cursorBearing,
		double& rangeScale, char& rangeUnits, char& displayRotation) {

	Dout(dbg_nmeaparser, "\033[034m" << "-------------------- 26  RARSD --------------------" << "\033[0m"); Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint rsdSizeField = 15;

	if (rsdSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		Dout(dbg_nmeaparser, "parseRSD: 26 = " << *itNmea);
		if ((*itNmea).substr(3, 3) == "RSD") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!decodeDefault<double>(itNmea, origin1Range, 0)) {
				ret.set(0, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "origin1Range = " << origin1Range);

			/*------------ Position 01 ---------------*/
			if (!decodeDefault<double>(itNmea, origin1BearingDegrees,
					0)) {
				ret.set(1, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "origin1BearingDegrees = " << origin1BearingDegrees);

			/*------------ Position 02 ---------------*/
			if (!decodeDefault<double>(itNmea, variableRangeMarker1, 0)) {
				ret.set(2, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "variableRangeMarker1 = " << variableRangeMarker1);

			/*------------ Position 03 ---------------*/
			if (!decodeDefault<double>(itNmea, bearingLine1, 0)) {
				ret.set(3, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "bearingLine1 = " << bearingLine1);

			/*------------ Position 04 ---------------*/
			if (!decodeDefault<double>(itNmea, origin2Range, 0)) {
				ret.set(4, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "origin2Range = " << origin2Range);

			/*------------ Position 05 ---------------*/
			if (!decodeDefault<double>(itNmea, origin2Bearing, 0)) {
				ret.set(5, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "origin2Bearing = " << origin2Bearing);

			/*------------ Position 06 ---------------*/
			if (!decodeDefault<double>(itNmea, vrm2, 0)) {
				ret.set(6, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "vrm2 = " << vrm2);

			/*------------ Position 07 ---------------*/
			if (!decodeDefault<double>(itNmea, ebl2, 0)) {
				ret.set(7, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "ebl2 = " << ebl2);

			/*------------ Position 08 ---------------*/
			if (!decodeDefault<double>(itNmea, cursorRange, 0)) {
				ret.set(8, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "cursorRange = " << cursorRange);

			/*------------ Position 09 ---------------*/
			if (!decodeDefault<double>(itNmea, cursorBearing, 0)) {
				ret.set(9, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "cursorBearing = " << cursorBearing);

			/*------------ Position 10 ---------------*/
			if (!decodeDefault<double>(itNmea, rangeScale, 0)) {
				ret.set(10, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "rangeScale = " << rangeScale);

			/*------------ Position 11 ---------------*/
			if (!decodeDefault<char>(itNmea, rangeUnits, c)) {
				ret.set(11, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "rangeUnits = " << rangeUnits);

			/*------------ Position 12 ---------------*/
			if (!decodeDefault<char>(itNmea, displayRotation, c)) {
				ret.set(12, true);
				itNmea++;
			} Dout(dbg_nmeaparser, "displayRotation = " << displayRotation);

		} else {
			//ERROR
		}
	} else {
		Dout(dbg_nmeaparser, "\033[033m" << "Error : " << nmea.c_str() << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos esperados : " << rsdSizeField << "\033[0m"); Dout(dbg_nmeaparser, "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m");
	}

	Dout(dbg_nmeaparser, "\033[032m" << "retorno binario : " << ret<< "\033[0m");
	return ret;

}

//void NmeaParser::parseVDM(const std::string& nmea, int& typeAIS, int& mmsiAIS,
//		double& latitudeAIS, double& longitudeAIS, float& speedAIS,
//		float& courseAIS, float& headingAIS) {
//
//	AisReader aa;
//	shipdata aisData;
//	int lettersread = 0;
//	int previoussentence = 0;
//	char aisline[500];
//	int nbytes = nmea.size();
//	const char * nmeabuffer = nmea.c_str();
//	int k, start;
//	int sentences, sentencenumb;
//	int m = 0;
//	int kommas = 0;
//
////	"!AIVDM,1,1,,B,407?owAuvW?UMJNmaiq6G<100HAU,0*33";
//
//	while (nmeabuffer[m++] != '!') {
//		if (m > (nbytes - 1)) {
//			break;
//		}
//	}
//	if (m > (nbytes - 1))
//		//continue; -------------
//		m--;
//
//	if (nmeabuffer[m] != '!' || nmeabuffer[m + 1] != 'A'
//			|| nmeabuffer[m + 5] != 'M') {
//		//std::continue;---------
//	}
//
//	for (k = 0; k < 20; k++) {
//		if (nmeabuffer[k + m] == ',')
//			kommas++;
//		if (kommas == 5)
//			break;
//	}
//	start = k + 1;
//	sentences = nmeabuffer[m + 7] - 0x30;
//	sentencenumb = nmeabuffer[m + 9] - 0x30;
//
//	if ((sentencenumb > 1) && ((previoussentence) != (sentencenumb - 1))) {
//	}
//	// continue;------------------
//
//	if (sentencenumb == 1)
//		lettersread = 0;
//	for (int j = 0; j < (nbytes - start - m); j++) {
//		if (nmeabuffer[m + j + start] == ',')
//			break;
//		aisline[lettersread] = nmeabuffer[m + j + start];
//		lettersread++;
//	}
//
//	if (sentencenumb >= sentences) {
//		aisline[lettersread] = 0;
//		aa.aisdecode(aisline, &aisData);
//	}
//
//	previoussentence = sentencenumb;
//
//	Dout(dbg_nmeaparser, "\033[036m" << nmea.c_str() << "\033[0m");
//
//	typeAIS = aisData.type;
//	Dout(dbg_nmeaparser, "typeAIS = " << typeAIS;
//
//	mmsiAIS = aisData.mmsi;
//	Dout(dbg_nmeaparser, "mmsiAIS = " << mmsiAIS;
//
//	latitudeAIS = aisData.latitude;
//	Dout(dbg_nmeaparser, "latitudeAIS = " << latitudeAIS;
//
//	longitudeAIS = aisData.longitude;
//	Dout(dbg_nmeaparser, "longitudeAIS = " << longitudeAIS;
//
//	speedAIS = aisData.speed;
//	Dout(dbg_nmeaparser, "speedAIS = " << speedAIS;
//
//	courseAIS = aisData.course;
//	Dout(dbg_nmeaparser, "courseAIS = " << courseAIS;
//
//	headingAIS = aisData.heading;
//	Dout(dbg_nmeaparser, "displayRotation = " << headingAIS;
//
//}

