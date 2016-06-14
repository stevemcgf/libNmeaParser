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

class NmeaParser::impl {
public:
	static bool tokenizeSentence(const std::string& nmea,
			std::vector<std::string>& tokens);
	static bool decodeTime(std::vector<std::string>::iterator &i,
			boost::posix_time::time_duration& out,
			const boost::posix_time::time_duration& def);
	static bool decodeLatLng(std::vector<std::string>::iterator &i, double& out,
			const double& def);
	static bool decodeDate(std::vector<std::string>::iterator &i,
			boost::gregorian::date& out, const boost::gregorian::date& def);
	static double toDecimalDegree(double degrees, double minutes,
			double seconds, char hemisphere);
	static bool decodeString(std::vector<std::string>::iterator &i,
			std::string& out, const std::string& def);
	template<typename Target>
	static bool decodeDefault(std::vector<std::string>::iterator &i,
			Target &out, const Target& def);

};

NmeaParser::NmeaParser() {
	// TODO Auto-generated constructor stub

}

NmeaParser::~NmeaParser() {
	// TODO Auto-generated destructor stub
}

bool NmeaParser::impl::tokenizeSentence(const std::string& nmea,
		std::vector<std::string>& tokens) {

	tokens.clear();

	const boost::char_separator<char> sep(",", 0, boost::keep_empty_tokens);
	const boost::tokenizer<boost::char_separator<char>> t(nmea.begin(),
			nmea.end(), sep);
	std::copy(t.begin(), t.end(), std::back_inserter(tokens));
	return tokens.size() > 0;  // parse is good if there is at least one token
}

const boost::regex hms("(\\d{2})(\\d{2})(\\d{2})(?:(.\\d*))?");
bool NmeaParser::impl::decodeTime(std::vector<std::string>::iterator &i,
		boost::posix_time::time_duration& out,
		const boost::posix_time::time_duration& def) {
	bool ret = false;
	boost::smatch m;
	if (boost::regex_match(*i, m, hms)) {
		ret = true;
		BOOST_LOG_TRIVIAL(debug)<< "decodeTime: time " << *i;
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

double NmeaParser::impl::toDecimalDegree(double degrees, double minutes,
		double seconds, char hemisphere) {
	degrees += minutes / 60.0 + seconds / 3600.0;
	if ((hemisphere == 'S') || (hemisphere == 'W'))
		degrees = -degrees;
	return degrees;
}

// because fields to convert may be empty
template<typename Target>
bool NmeaParser::impl::decodeDefault(std::vector<std::string>::iterator &i,
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
bool NmeaParser::impl::decodeLatLng(std::vector<std::string>::iterator &i,
		double& out, const double& def) {
	bool ret = false;
	boost::smatch m;
	if (boost::regex_match(*i, m, latlng)) {
		ret = true;
		BOOST_LOG_TRIVIAL(debug)<< "decodeLatLng: num " << *i;
		i++; // consume the match
		double deg = boost::lexical_cast<double>(m[1].str());
		double min = boost::lexical_cast<double>(m[2].str());
		BOOST_LOG_TRIVIAL(debug)<< "decodeLatLng: hem " << *i;
		char hemisphere = (*i).at(0);
		i++; // consume hemisphere
		out = toDecimalDegree(deg, min, 0.0, hemisphere);
	} else {
		out = def;
	}

	return ret;
}

const boost::regex dmy("(\\d{2})(\\d{2})(\\d{2})");
bool NmeaParser::impl::decodeDate(std::vector<std::string>::iterator &i,
		boost::gregorian::date& out, const boost::gregorian::date& def) {
	bool ret = false;
	boost::smatch m;
	if (boost::regex_match(*i, m, dmy)) {
		ret = true;
		BOOST_LOG_TRIVIAL(debug)<< "decodeDate: date " << *i;
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

//------------------------------------- Parseo de tramas NMEA -------------------------------
// definicion de valores por defecto
boost::posix_time::time_duration deftime(0, 0, 0, 0);
boost::gregorian::date defdate(1400, 1, 1);
const double defLatLong = 98.999;
char c = 'X';
std::string defString = "XXXXXX";

std::bitset<6> NmeaParser::parseZDA(const std::string& nmea,
		boost::posix_time::time_duration& mtime, int& day, int& month,
		int& year, int& localZoneHours, int& localZoneMinutes) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 01  GPZDA --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	std::bitset<6> ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint zdaSizeField = 7;

	if (zdaSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		BOOST_LOG_TRIVIAL(debug)<< "parseZDA: 1 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "ZDA") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeTime(itNmea, mtime, deftime)) {
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "mtime = " << mtime;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, day, 0))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "day = " << day;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, month, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "month = " << month;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, year, 0))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "year = " << year;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, localZoneHours, 0))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "localZoneHours = " << localZoneHours;

			/*------------ Position 05 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, localZoneMinutes, 0))
			{
				ret.set(5, true);
			}
			BOOST_LOG_TRIVIAL(debug)<< "localZoneMinutes = " << localZoneMinutes;

		} else {
			//ERROR
		}
	}
	else {
		BOOST_LOG_TRIVIAL(debug)<< "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug)<< "\033[033m" << "campos esperados : " << zdaSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug)
		<< "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseGLL(const std::string& nmea, double& latitude,
		double& longitude, boost::posix_time::time_duration& mtime,
		char& status, char& modeIndicator) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 02  GPGLL --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint gllSizeField = 8;

	if (gllSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		BOOST_LOG_TRIVIAL(debug)<< "parseGLL: 2 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "GLL") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeLatLng(itNmea, latitude, defLatLong))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "latitude = " << latitude;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeLatLng(itNmea, longitude, defLatLong))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "longitude = " << longitude;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeTime(itNmea, mtime, deftime)) {
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "mtime = " << mtime;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, status, c))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "status = " << status;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, modeIndicator, c))
			{
				ret.set(4, true);
			}
			BOOST_LOG_TRIVIAL(debug)<< "modeIndicator = " << modeIndicator;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << gllSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseGGA(const std::string& nmea,
		boost::posix_time::time_duration& mtime, double& latitude,
		double& longitude, Nmea_GPSQualityIndicator& quality, int& numSV,
		double& hdop, double& orthometricheight, double& geoidseparation,
		double& agediffgps, std::string& refid) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 03  GPGGA --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint ggaSizeField = 15;

	if (ggaSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseGGA: 3 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "GGA") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeTime(itNmea, mtime, deftime)) {
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "mtime = " << mtime;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeLatLng(itNmea, latitude, defLatLong))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "latitude = " << latitude;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeLatLng(itNmea, longitude, defLatLong))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "longitude = " << longitude;

			/*------------ Position 03 ---------------*/
			int qualityIndicator;
			if (!impl::decodeDefault<int>(itNmea, qualityIndicator, 0))
			{
				ret.set(3, true);
				itNmea++;
			}
			//BOOST_LOG_TRIVIAL(debug)<< "qualityIndicator = " << qualityIndicator;

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
				default :
				{
					ret.set(3, true);
					quality = Nmea_GPSQualityIndicator::FixNotValid;
					break;
				}
			}
			BOOST_LOG_TRIVIAL(debug)<< "quality = " << quality;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, numSV, 0))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "numSV = " << numSV;

			/*------------ Position 05 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, hdop, 0))
			{
				ret.set(5, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< std::fixed << std::setprecision(2) << "hdop = " << hdop;

			/*------------ Position 06 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, orthometricheight, 0))
			{
				ret.set(6, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "orthometricheight = " << orthometricheight;
			itNmea++;

			/*------------ Position 07 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, geoidseparation, 0))
			{
				ret.set(7, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "geoidseparation = " << geoidseparation;
			itNmea++;

			/*------------ Position 08 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, agediffgps, 0))
			{
				ret.set(8, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "agediffgps = " << agediffgps;

			/*------------ Position 09 ---------------*/
			if (!impl::decodeDefault<std::string>(itNmea, refid, defString))
			{
				ret.set(9, true);
			}
			BOOST_LOG_TRIVIAL(debug)<< "refid = " << refid;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << ggaSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseVTG(const std::string& nmea, double& coursetrue,
		double& coursemagnetic, double& speedknots, double& speedkph) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 04  GPVTG --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint vtgSizeField = 10;

	if (vtgSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseVTG: 4 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "VTG") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, coursetrue, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "coursetrue = " << coursetrue;
			itNmea++;
			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, coursemagnetic, 0))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "coursemagnetic = " << coursemagnetic;
			itNmea++;
			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, speedknots, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "speedknots = " << speedknots;
			itNmea++;
			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, speedkph, 0))
			{
				ret.set(3, true);
			}
			BOOST_LOG_TRIVIAL(debug)<< "speedkph = " << speedkph;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << vtgSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseRMC(const std::string& nmea,
		boost::posix_time::time_duration& mtime, double& latitude,
		double& longitude, double& speedknots, double& coursetrue,
		boost::gregorian::date& mdate, double& magneticvar) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 05  GPRMC --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint rmcSizeField = 14;

	if (rmcSizeField >= fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseRMC: 5 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "RMC") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeTime(itNmea, mtime, deftime)) {
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "mtime = " << mtime;
			itNmea++;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeLatLng(itNmea, latitude, defLatLong))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "latitude = " << latitude;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeLatLng(itNmea, longitude, defLatLong))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "longitude = " << longitude;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, speedknots, 0))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "speedknots = " << speedknots;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, coursetrue, 0))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "coursetrue = " << coursetrue;

			/*------------ Position 05 ---------------*/
			if (!impl::decodeDate(itNmea, mdate, defdate)) {
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "mdate = " << mdate;

			/*------------ Position 06 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, magneticvar, 0))
			{
				ret.set(6, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "magneticvar = " << magneticvar;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << rmcSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseWPL(const std::string& nmea, double& latitude,
		double& longitude, std::string& waypointName) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 06  GPWPL --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint wplSizeField = 6;

	if (wplSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseWPL: 6 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "WPL") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeLatLng(itNmea, latitude, defLatLong))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "latitude = " << latitude;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeLatLng(itNmea, longitude, defLatLong))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "longitude = " << longitude;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<std::string>(itNmea, waypointName, defString))
			{
				ret.set(2, true);
			}
			BOOST_LOG_TRIVIAL(debug)<< "waypointName = " << waypointName;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << wplSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseRTE(const std::string& nmea,
		double& messagesTransmitted, double& messageNumber, char& messageMode,
		std::string& routeIdentifier, std::vector<std::string>& revenue1) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 07  GPRTE --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint rteSizeField = 6;

	if (rteSizeField <= fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseRTE: 7 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "RTE") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, messagesTransmitted, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "messagesTransmitted = " << messagesTransmitted;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, messageNumber, 0))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "messageNumber = " << messageNumber;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, messageMode, c))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "messageMode = " << messageMode;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<std::string>(itNmea, routeIdentifier, defString))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "routeIdentifier = " << routeIdentifier;

			std::string WaypointIdentifier;
			for (uint i=0;i< (fields.size()-5);i++) {
				WaypointIdentifier = (*itNmea);
				//BOOST_LOG_TRIVIAL(debug)<< "WaypointIdentifier "<< i + 1 <<" = " << WaypointIdentifier;
				itNmea++;
				revenue1.push_back(WaypointIdentifier);
			}
			// impresion
			uint pos;

			for (std::vector<std::string>::iterator it = revenue1.begin( ); it != revenue1.end( ); ++it) {
				//BOOST_LOG_TRIVIAL(debug)<< "pairs: " << it->first << " --> " << it->second.c_str();
				pos= std::distance(revenue1.begin(), it);
				BOOST_LOG_TRIVIAL(debug)<< "WaypointIdentifier "<< pos + 1 <<" = " << *it;
			}

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << rteSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseVHW(const std::string& nmea,
		double& speedInKnots, char& knots, double& speedInKmH,
		char& kilometers) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 08  VDVHW --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint vhwSizeField = 9;

	if (vhwSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug) << "parseVHW: 8 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "VHW") {
			itNmea++;
			itNmea++;
			itNmea++;
			itNmea++;
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, speedInKnots, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "speedInKnots = " << speedInKnots;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, knots, c))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "knots = " << knots;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, speedInKmH, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "speedInKmH = " << speedInKmH;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, kilometers, c))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "kilometers = " << kilometers;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << vhwSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseMTW(const std::string& nmea, double& degrees,
		char& celcius) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 09  VDMTW --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint mtwSizeField = 3;

	if (mtwSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseMTW: 9 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "MTW") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, degrees, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "degrees = " << degrees;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, celcius, c))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "celcius = " << celcius;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << mtwSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseVBW(const std::string& nmea,
		double& longitudinalWaterSpeed, double& transverseWaterSpeed,
		char& waterDataStatus, double& longitudinalGroundSpeed,
		double& transverseGroundSpeed, char& groundDataStatus) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 10  VDVBW --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint vbwSizeField = 11;

	if (vbwSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseVBW: 10 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "VBW") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, longitudinalWaterSpeed, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "longitudinalWaterSpeed = " << longitudinalWaterSpeed;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, transverseWaterSpeed, 0))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "transverseWaterSpeed = " << transverseWaterSpeed;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, waterDataStatus, c))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "waterDataStatus = " << waterDataStatus;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, longitudinalGroundSpeed, 0))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "longitudinalGroundSpeed = " << longitudinalGroundSpeed;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, transverseGroundSpeed, 0))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "transverseGroundSpeed = " << transverseGroundSpeed;

			/*------------ Position 05 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, groundDataStatus, c))
			{
				ret.set(5, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "groundDataStatus = " << groundDataStatus;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << vbwSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseVLW(const std::string& nmea,
		double& totalCumulativeDistance, char& nauticalMiles1,
		double& distanceSinceReset, char& nauticalMiles2) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 11  VDVLW --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint vlwSizeField = 5;

	if (vlwSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseVLW: 11 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "VLW") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, totalCumulativeDistance, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "totalCumulativeDistance = " << totalCumulativeDistance;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, nauticalMiles1, c))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "nauticalMiles1 = " << nauticalMiles1;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, distanceSinceReset, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "distanceSinceReset = " << distanceSinceReset;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, nauticalMiles2, c))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "nauticalMiles2 = " << nauticalMiles2;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << vlwSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseDPT(const std::string& nmea,
		double& waterDepthRelativeToTheTransducer, double& offsetFromTransducer,
		double& maximumRangeScaleInUse) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 12  SDDPT --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint dptSizeField = 4;

	if (dptSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseDPT: 12 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "DPT") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, waterDepthRelativeToTheTransducer, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "waterDepthRelativeToTheTransducer = " << waterDepthRelativeToTheTransducer;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, offsetFromTransducer, 0))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "offsetFromTransducer = " << offsetFromTransducer;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, maximumRangeScaleInUse, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "maximumRangeScaleInUse = " << maximumRangeScaleInUse;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << dptSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseDBT(const std::string& nmea,
		double& waterDepthInFeet, char& feet, double& waterDepthInMeters,
		char& meters, double& waterDepthInFathoms, char& fathoms) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 13  SDDBT --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint dbtSizeField = 7;

	if (dbtSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseDBT: 13 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "DBT") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, waterDepthInFeet, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "waterDepthInFeet = " << waterDepthInFeet;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, feet, c))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "feet = " << feet;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, waterDepthInMeters, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "waterDepthInMeters = " << waterDepthInMeters;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, meters, c))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "meters = " << meters;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, waterDepthInFathoms, 0))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "waterDepthInFathoms = " << waterDepthInFathoms;

			/*------------ Position 05 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, fathoms, c))
			{
				ret.set(5, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "fathoms = " << fathoms;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << dbtSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseDBK(const std::string& nmea,
		double& depthBelowKeelFeet, char& feet, double& depthBelowKeelMeters,
		char& meters, double& depthBelowKeelFathoms, char& fathoms) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 14  SDDBK --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint dbkSizeField = 7;

	if (dbkSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseDBK: 14 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "DBK") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, depthBelowKeelFeet, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "depthBelowKeelFeet = " << depthBelowKeelFeet;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, feet, c))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "feet = " << feet;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, depthBelowKeelMeters, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "depthBelowKeelMeters = " << depthBelowKeelMeters;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, meters, c))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "meters = " << meters;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, depthBelowKeelFathoms, 0))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "depthBelowKeelFathoms = " << depthBelowKeelFathoms;

			/*------------ Position 05 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, fathoms, c))
			{
				ret.set(5, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "fathoms = " << fathoms;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << dbkSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parsePSKPDPT(const std::string& nmea,
		double& waterDepthRelativeToTheTransducer, double& offsetFromTransducer,
		double& maximumRangeScaleInUse, int& bottomEchoStrength,
		int& echoSounderChannelNumber, std::string& transducerLocation) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 15  PSKPDPT --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint dptSizeField = 7;

	if (dptSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parsePSKPDPT: 15 = " << *itNmea;
		if ((*itNmea).substr(0, 7) == "PSKPDPT") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, waterDepthRelativeToTheTransducer, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "waterDepthRelativeToTheTransducer = " << waterDepthRelativeToTheTransducer;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, offsetFromTransducer, 0))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "offsetFromTransducer = " << offsetFromTransducer;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, maximumRangeScaleInUse, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "maximumRangeScaleInUse = " << maximumRangeScaleInUse;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, bottomEchoStrength, 0))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "bottomEchoStrength = " << bottomEchoStrength;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, echoSounderChannelNumber, 0))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "echoSounderChannelNumber = " << echoSounderChannelNumber;

			/*------------ Position 05 ---------------*/
			if (!impl::decodeDefault<std::string>(itNmea, transducerLocation, defString))
			{
				ret.set(5, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "transducerLocation = " << transducerLocation;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << dptSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseHDT(const std::string& nmea,
		double& headingDegreesTrue, char& t) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 16  HCHDT --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint hdtSizeField = 3;

	if (hdtSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseHDT: 16 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "HDT") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, headingDegreesTrue, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "headingDegreesTrue = " << headingDegreesTrue;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, t, c))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "t = " << t;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << hdtSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseHDG(const std::string& nmea,
		double& magneticSensorHeadingInDegrees,
		double& magneticDeviationDegrees, char& magneticDeviationDirection,
		double& magneticVariationDegrees, char& magneticVariationDirection) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 17  HCHDG --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint hdgSizeField = 6;

	if (hdgSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseHDG: 17 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "HDG") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, magneticSensorHeadingInDegrees, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "magneticSensorHeadingInDegrees = " << magneticSensorHeadingInDegrees;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, magneticDeviationDegrees, 0))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "magneticDeviationDegrees = " << magneticDeviationDegrees;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, magneticDeviationDirection, c))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "magneticDeviationDirection = " << magneticDeviationDirection;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, magneticVariationDegrees, 0))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "magneticVariationDegrees = " << magneticVariationDegrees;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, magneticVariationDirection, c))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "magneticVariationDirection = " << magneticVariationDirection;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << hdgSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseHDM(const std::string& nmea,
		double& headingDegreesMagnetic, char& magnetic) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 18  HCHDM --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint hdmSizeField = 3;

	if (hdmSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseHDM: 18 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "HDM") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, headingDegreesMagnetic, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "headingDegreesMagnetic = " << headingDegreesMagnetic;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, magnetic, c))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "magnetic = " << magnetic;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << hdmSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseROT(const std::string& nmea, double& rateOfTurn,
		char& status) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 19  TIROT --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint rotSizeField = 3;

	if (rotSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseROT: 19 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "ROT") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, rateOfTurn, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "rateOfTurn = " << rateOfTurn;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, status, c))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "status = " << status;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << rotSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseMWV(const std::string& nmea, double& windAngle,
		char& reference, double& windSpeed, char& windSpeedUnits,
		char& sensorStatus) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 20  IIMWV --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint mwvSizeField = 6;

	if (mwvSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseMWV: 20 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "MWV") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, windAngle, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "windAngle = " << windAngle;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, reference, c))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "reference = " << reference;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, windSpeed, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "windSpeed = " << windSpeed;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, windSpeedUnits, c))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "windSpeedUnits = " << windSpeedUnits;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, sensorStatus, c))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "sensorStatus = " << sensorStatus;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << mwvSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseMWD(const std::string& nmea,
		double& trueWindDirection, char& t, double& magneticWindDirection,
		char& magnetic, double& windSpeedKnots, char& knots,
		double& windSpeedMeters, char& meters) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 21  WIMWD --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint mwdSizeField = 9;

	if (mwdSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseMWD: 21 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "MWD") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, trueWindDirection, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "trueWindDirection = " << trueWindDirection;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, t, c))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "t = " << t;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, magneticWindDirection, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "magneticWindDirection = " << magneticWindDirection;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, magnetic, c))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "magnetic = " << magnetic;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, windSpeedKnots, 0))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "windSpeedKnots = " << windSpeedKnots;

			/*------------ Position 05 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, knots, c))
			{
				ret.set(5, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "knots = " << knots;

			/*------------ Position 06 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, windSpeedMeters, 0))
			{
				ret.set(6, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "windSpeedMeters = " << windSpeedMeters;

			/*------------ Position 07 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, meters, c))
			{
				ret.set(7, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "meters = " << meters;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << mwdSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseXDR(const std::string& nmea,
		char& typeOfSensorTemperature, double& temperatureReading,
		char& centigrade, std::string& nameOfTransducer,
		char& typeOfSensorPressure, double& pressureReading, char& bars,
		std::string& nameOfPressureSensor, char& typeOfSensorHumidity,
		double& humidity, char& humidityUnitsOfMeasurePercent,
		std::string& nameOfRelativeHumiditySensor) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 22  IIXDR --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint xdrSizeField = 13;

	if (xdrSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseXDR: 22 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "XDR") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, typeOfSensorTemperature, c))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "typeOfSensorTemperature = " << typeOfSensorTemperature;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, temperatureReading, 0))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "temperatureReading = " << temperatureReading;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, centigrade, c))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "centigrade = " << centigrade;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<std::string>(itNmea, nameOfTransducer, defString))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "nameOfTransducer = " << nameOfTransducer;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, typeOfSensorPressure, c))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "typeOfSensorPressure = " << typeOfSensorPressure;

			/*------------ Position 05 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, pressureReading, 0))
			{
				ret.set(5, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "pressureReading = " << pressureReading;

			/*------------ Position 06 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, bars, c))
			{
				ret.set(6, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "bars = " << bars;

			/*------------ Position 07 ---------------*/
			if (!impl::decodeDefault<std::string>(itNmea, nameOfPressureSensor, defString))
			{
				ret.set(7, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "nameOfPressureSensor = " << nameOfPressureSensor;

			/*------------ Position 08 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, typeOfSensorHumidity, c))
			{
				ret.set(8, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "typeOfSensorHumidity = " << typeOfSensorHumidity;

			/*------------ Position 09 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, humidity, 0))
			{
				ret.set(9, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "humidity = " << humidity;

			/*------------ Position 10 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, humidityUnitsOfMeasurePercent, c))
			{
				ret.set(10, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "humidityUnitsOfMeasurePercent = " << humidityUnitsOfMeasurePercent;

			/*------------ Position 11 ---------------*/
			if (!impl::decodeDefault<std::string>(itNmea, nameOfRelativeHumiditySensor, defString))
			{
				ret.set(7, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "nameOfRelativeHumiditySensor = " << nameOfRelativeHumiditySensor;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << xdrSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseTTD(const std::string& nmea, int& aaa, int& bbb,
		int& ccc, std::string& ddd, int& eee) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 23  IITTD --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint ttdSizeField = 6;

	if (ttdSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseTTD: 23 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "TTD") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, aaa, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "aaa = " << aaa;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, bbb, 0))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "bbb = " << bbb;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, ccc, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "ccc = " << ccc;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<std::string>(itNmea, ddd, defString))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "ddd = " << ddd;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, eee, 0))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "eee = " << eee;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug)<< "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << ttdSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

//NmeaParserResult NmeaParser::parseTLB(const std::string& nmea,
//		std::vector<std::pair<double, std::string> >& revenue) {
//
//	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 24  RATLB --------------------" << "\033[0m";
//	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";
//
//	NmeaParserResult ret;
//	ret.reset();
//	ret.set(0, true);
//
//	std::vector<std::string> fields;
//
//	impl::tokenizeSentence(nmea, fields);
//
//	uint tlbSizeField = 3;
//
//	double targetNumber;
//	std::string secondData;
//
//	if (tlbSizeField <= fields.size()) {
//
//		std::vector<std::string>::iterator itNmea = fields.begin();
//		BOOST_LOG_TRIVIAL(debug)<< "parseTLB: 24 = " << *itNmea;
//		if ((*itNmea).substr(2, 3) == "TLB") {
//			itNmea++;
//
//			pair<double,std::string> aPair;
//			for (uint i=0;i< fields.size()/2;i++) {
//				targetNumber = impl::lexical_cast_default<double>(*itNmea++, 0);
//				//BOOST_LOG_TRIVIAL(debug)<< "targetNumber = " << targetNumber;
//
//				secondData = (*itNmea);
//				//BOOST_LOG_TRIVIAL(debug)<< "secondData = " << secondData;
//				itNmea++;
//
//				aPair.first = targetNumber;
//				aPair.second = secondData;
//
//				revenue.push_back(aPair);
//				//BOOST_LOG_TRIVIAL(debug)<< "i = " << i;
//				i+1;
//
//			}
//			// impresion
//
//			for (std::vector<pair<double,std::string>>::iterator it = revenue.begin( ); it != revenue.end( ); ++it) {
//				BOOST_LOG_TRIVIAL(debug)<< "pairs: " << it->first << " --> " << it->second.c_str();
//			}
//
//		} else {
//			//ERROR
//		}
//	} else {
//		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
//		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << tlbSizeField << "\033[0m";
//		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
//	}
//
//	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
//	return ret;
//
//}

NmeaParserResult NmeaParser::parseOSD(const std::string& nmea, double& heading,
		char& status, double& vesselCourse, char& referenceCourse,
		double& vesselSpeed, char& referenceSpeed, double& vesselSet,
		double& vesselDrift, char& speedUnits) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 25  RAOSD --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint osdSizeField = 10;

	if (osdSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseOSD: 25 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "OSD") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, heading, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "heading = " << heading;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, status, c))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "status = " << status;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, vesselCourse, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "vesselCourse = " << vesselCourse;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, referenceCourse, c))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "referenceCourse = " << referenceCourse;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, vesselSpeed, 0))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "vesselSpeed = " << vesselSpeed;

			/*------------ Position 05 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, referenceSpeed, c))
			{
				ret.set(5, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "referenceSpeed = " << referenceSpeed;

			/*------------ Position 06 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, vesselSet, 0))
			{
				ret.set(6, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "vesselSet = " << vesselSet;

			/*------------ Position 07 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, vesselDrift, 0))
			{
				ret.set(7, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "vesselDrift = " << vesselDrift;

			/*------------ Position 08 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, speedUnits, c))
			{
				ret.set(8, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "speedUnits = " << speedUnits;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << osdSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
	return ret;

}

NmeaParserResult NmeaParser::parseRSD(const std::string& nmea,
		double& origin1Range, double& origin1BearingDegrees,
		double& variableRangeMarker1, double& bearingLine1,
		double& origin2Range, double& origin2Bearing, double& vrm2,
		double& ebl2, double& cursorRange, double& cursorBearing,
		double& rangeScale, char& rangeUnits, char& displayRotation) {

	BOOST_LOG_TRIVIAL(debug)<< "\033[034m" << "-------------------- 26  RARSD --------------------" << "\033[0m";
	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint rsdSizeField = 14;

	if (rsdSizeField == fields.size()) {

		std::vector<std::string>::iterator itNmea = fields.begin();
		BOOST_LOG_TRIVIAL(debug)<< "parseRSD: 26 = " << *itNmea;
		if ((*itNmea).substr(2, 3) == "RSD") {
			itNmea++;

			/*------------ Position 00 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, origin1Range, 0))
			{
				ret.set(0, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "origin1Range = " << origin1Range;

			/*------------ Position 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, origin1BearingDegrees, 0))
			{
				ret.set(1, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "origin1BearingDegrees = " << origin1BearingDegrees;

			/*------------ Position 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, variableRangeMarker1, 0))
			{
				ret.set(2, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "variableRangeMarker1 = " << variableRangeMarker1;

			/*------------ Position 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, bearingLine1, 0))
			{
				ret.set(3, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "bearingLine1 = " << bearingLine1;

			/*------------ Position 04 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, origin2Range, 0))
			{
				ret.set(4, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "origin2Range = " << origin2Range;

			/*------------ Position 05 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, origin2Bearing, 0))
			{
				ret.set(5, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "origin2Bearing = " << origin2Bearing;

			/*------------ Position 06 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, vrm2, 0))
			{
				ret.set(6, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "vrm2 = " << vrm2;

			/*------------ Position 07 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, ebl2, 0))
			{
				ret.set(7, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "ebl2 = " << ebl2;

			/*------------ Position 08 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, cursorRange, 0))
			{
				ret.set(8, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "cursorRange = " << cursorRange;

			/*------------ Position 09 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, cursorBearing, 0))
			{
				ret.set(9, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "cursorBearing = " << cursorBearing;

			/*------------ Position 10 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, rangeScale, 0))
			{
				ret.set(10, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "rangeScale = " << rangeScale;

			/*------------ Position 11 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, rangeUnits, c))
			{
				ret.set(11, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "rangeUnits = " << rangeUnits;

			/*------------ Position 12 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, displayRotation, c))
			{
				ret.set(12, true);
				itNmea++;
			}
			BOOST_LOG_TRIVIAL(debug)<< "displayRotation = " << displayRotation;

		} else {
			//ERROR
		}
	} else {
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "Error : " << nmea.c_str() << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos esperados : " << rsdSizeField << "\033[0m";
		BOOST_LOG_TRIVIAL(debug) << "\033[033m" << "campos recibidos : " << fields.size()<< "\033[0m";
	}

	BOOST_LOG_TRIVIAL(debug) << "\033[032m" << "retorno binario : " << ret<< "\033[0m";
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
//	BOOST_LOG_TRIVIAL(debug)<< "\033[036m" << nmea.c_str() << "\033[0m";
//
//	typeAIS = aisData.type;
//	BOOST_LOG_TRIVIAL(debug)<< "typeAIS = " << typeAIS;
//
//	mmsiAIS = aisData.mmsi;
//	BOOST_LOG_TRIVIAL(debug)<< "mmsiAIS = " << mmsiAIS;
//
//	latitudeAIS = aisData.latitude;
//	BOOST_LOG_TRIVIAL(debug)<< "latitudeAIS = " << latitudeAIS;
//
//	longitudeAIS = aisData.longitude;
//	BOOST_LOG_TRIVIAL(debug)<< "longitudeAIS = " << longitudeAIS;
//
//	speedAIS = aisData.speed;
//	BOOST_LOG_TRIVIAL(debug)<< "speedAIS = " << speedAIS;
//
//	courseAIS = aisData.course;
//	BOOST_LOG_TRIVIAL(debug)<< "courseAIS = " << courseAIS;
//
//	headingAIS = aisData.heading;
//	BOOST_LOG_TRIVIAL(debug)<< "displayRotation = " << headingAIS;
//
//}

