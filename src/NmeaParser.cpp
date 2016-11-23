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
		BOOST_LOG_TRIVIAL(debug)<<" decodeTime: time " << *i;
		++i;  // consume the match
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
		++i; // consume valor.
	} catch (boost::bad_lexical_cast &) {
		out = def;
	}

	return ret;
}

bool NmeaParser::decodeHex(std::vector<std::string>::iterator &i, uint& out,
		const uint& def) {
	bool ret = false;
	try {
		std::stringstream ss(*i, std::ios_base::in);
		ss >> std::hex >> out;
		ret = true;
		++i;
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
		//BOOST_LOG_TRIVIAL(debug)<<"decodeLatLng: num " << *i;
		++i; // consume the match
		double deg = boost::lexical_cast<double>(m[1].str());
		double min = boost::lexical_cast<double>(m[2].str());
		//BOOST_LOG_TRIVIAL(debug)<<"decodeLatLng: hem " << *i;
		char hemisphere = (*i).at(0);
		++i; // consume hemisphere
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
		//BOOST_LOG_TRIVIAL(debug)<<"decodeDate: date " << *i;
		++i; // consume the match
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
			++i; // consume valor.
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
char defChar = 'X';
std::string defString = "XXXX";

NmeaParserResult NmeaParser::parseZDA(const std::string& nmea,
		boost::posix_time::time_duration& mtime, int& day, int& month,
		int& year, int& localZoneHours, int& localZoneMinutes) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseZDA";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint expectedFieldCount = 8;

	if (fields.size() == expectedFieldCount) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "ZDA") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeTime(itNmea, mtime, deftime)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "mtime = " << mtime;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<int>(itNmea, day, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "day = " << day;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<int>(itNmea, month, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "month = " << month;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<int>(itNmea, year, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "year = " << year;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<int>(itNmea, localZoneHours, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "localZoneHours = " << localZoneHours;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<int>(itNmea, localZoneMinutes, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "localZoneMinutes = " << localZoneMinutes;
		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << expectedFieldCount;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseGLL(const std::string& nmea, double& latitude,
		double& longitude, boost::posix_time::time_duration& mtime,
		char& status, char& modeIndicator) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseGLL";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint expectedFieldCount = 9;

	if (fields.size() == expectedFieldCount) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "GLL") {
			++itNmea;

			/*------------ Field 01,02 ---------------*/
			if (!decodeLatLng(itNmea, latitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "latitude = " << latitude;

			/*------------ Field 03,04 ---------------*/
			if (!decodeLatLng(itNmea, longitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "longitude = " << longitude;

			/*------------ Field 05 ---------------*/
			if (!decodeTime(itNmea, mtime, deftime)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "mtime = " << mtime;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<char>(itNmea, status, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "status = " << status;

			/*------------ Field 07 ---------------*/
			if (!decodeDefault<char>(itNmea, modeIndicator, defChar)) {
				ret.set(idxVar, true);
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "modeIndicator = " << modeIndicator;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << expectedFieldCount;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseGGA(const std::string& nmea,
		boost::posix_time::time_duration& mtime, double& latitude,
		double& longitude, Nmea_GPSQualityIndicator& quality, int& numSV,
		double& hdop, double& orthometricheight, double& geoidseparation,
		double& agediffgps, std::string& refid) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseGGA";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint ggaSizeField = 16;

	if (fields.size() == ggaSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "GGA") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeTime(itNmea, mtime, deftime)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "mtime = " << mtime;

			/*------------ Field 02,03 ---------------*/
			if (!decodeLatLng(itNmea, latitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"latitude = " << latitude;

			/*------------ Field 04,05 ---------------*/
			if (!decodeLatLng(itNmea, longitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"longitude = " << longitude;

			/*------------ Field 06 ---------------*/
			int qualityIndicator;
			if (!decodeDefault<int>(itNmea, qualityIndicator, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			} else {
				switch (qualityIndicator) {
					case 0: {
						quality = Nmea_GPSQualityIndicator_FixNotValid;
						break;
					}
					case 1: {
						quality = Nmea_GPSQualityIndicator_GPSFix;
						break;
					}
					case 2: {
						quality = Nmea_GPSQualityIndicator_GPSFixDifferential;
						break;
					}
					case 3: {
						quality = Nmea_GPSQualityIndicator_RealTimeKinematic;
						break;
					}
					case 4: {
						quality = Nmea_GPSQualityIndicator_RealTimeKinematicOmniStar;
						break;
					}
					default: {
						ret.set(idxVar, true);
						quality = Nmea_GPSQualityIndicator_FixNotValid;
						break;
					}
				}
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "quality = " << quality;

			/*------------ Field 07 ---------------*/
			if (!decodeDefault<int>(itNmea, numSV, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"numSV = " << numSV;

			/*------------ Field 08 ---------------*/
			if (!decodeDefault<double>(itNmea, hdop, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"hdop = " << hdop;

			/*------------ Field 09 ---------------*/
			if (!decodeDefault<double>(itNmea, orthometricheight, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "orthometricheight = " << orthometricheight;

			/*------------ Field 10 ---------------*/
			// Se salta unidad de medida siempre Metros.
			++itNmea;

			/*------------ Field 11 ---------------*/
			if (!decodeDefault<double>(itNmea, geoidseparation, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"geoidseparation = " << geoidseparation;

			/*------------ Field 12 ---------------*/
			// Se salta unidad de medida siempre Metros.
			++itNmea;

			/*------------ Field 13 ---------------*/
			if (!decodeDefault<double>(itNmea, agediffgps, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"agediffgps = " << agediffgps;

			/*------------ Field 14 ---------------*/
			if (!decodeString(itNmea, refid, defString)) {
				ret.set(idxVar, true);
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"refid = " << refid;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << ggaSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseVTG(const std::string& nmea,
		double& coursetrue, double& coursemagnetic, double& speedknots,
		double& speedkph) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseVTG";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint vtgSizeField = 11;

	if (fields.size() == vtgSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "VTG") {
			++itNmea;

			/*------------ Field 01,02 ---------------*/
			if (!decodeDefault<double>(itNmea, coursetrue, 0)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"coursetrue = " << coursetrue;
			++itNmea;
			/*------------ Field 03,04 ---------------*/
			if (!decodeDefault<double>(itNmea, coursemagnetic, 0)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"coursemagnetic = " << coursemagnetic;
			++itNmea;
			/*------------ Field 05,06 ---------------*/
			if (!decodeDefault<double>(itNmea, speedknots, 0)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"speedknots = " << speedknots;
			++itNmea;
			/*------------ Field 07,08 ---------------*/
			if (!decodeDefault<double>(itNmea, speedkph, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"speedkph = " << speedkph;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << vtgSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseRMC(const std::string& nmea,
		boost::posix_time::time_duration& mtime, double& latitude,
		double& longitude, double& speedknots, double& coursetrue,
		boost::gregorian::date& mdate, double& magneticvar) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseRMC";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint rmcSizeField = 15;

	if (fields.size() == rmcSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "RMC") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeTime(itNmea, mtime, deftime)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"mtime = " << mtime;

			/*------------ Field 02 ---------------*/
			++itNmea;

			/*------------ Field 03,04 ---------------*/
			if (!decodeLatLng(itNmea, latitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"latitude = " << latitude;

			/*------------ Field 05,06 ---------------*/
			if (!decodeLatLng(itNmea, longitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"longitude = " << longitude;

			/*------------ Field 07 ---------------*/
			if (!decodeDefault<double>(itNmea, speedknots, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"speedknots = " << speedknots;

			/*------------ Field 08 ---------------*/
			if (!decodeDefault<double>(itNmea, coursetrue, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"coursetrue = " << coursetrue;

			/*------------ Field 09 ---------------*/
			if (!decodeDate(itNmea, mdate, defdate)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"mdate = " << mdate;

			/*------------ Field 10 ---------------*/
			if (!decodeDefault<double>(itNmea, magneticvar, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"magneticvar = " << magneticvar;

			/*------------ Field 11 ---------------*/
			++itNmea;
			/*------------ Field 12 ---------------*/
			++itNmea;
			/*------------ Field 13 ---------------*/
			++itNmea;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << rmcSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseWPL(const std::string& nmea, double& latitude,
		double& longitude, std::string& waypointName) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseWPL";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint wplSizeField = 7;

	if (fields.size() == wplSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "WPL") {
			++itNmea;

			/*------------ Field 01,02 ---------------*/
			if (!decodeLatLng(itNmea, latitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"latitude = " << latitude;

			/*------------ Field 03,04 ---------------*/
			if (!decodeLatLng(itNmea, longitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"longitude = " << longitude;

			/*------------ Field 05 ---------------*/
			if (!decodeString(itNmea, waypointName, defString)) {
				ret.set(idxVar, true);
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"waypointName = " << waypointName;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << wplSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseRTE(const std::string& nmea, int& totalLines,
		int& lineCount, char& messageMode, std::string& routeName,
		std::vector<std::string>& waypointNames) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseRTE";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint rteSizeField = 7;

	if (fields.size() >= rteSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "RTE") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<int>(itNmea, totalLines, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "totalLines = " << totalLines;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<int>(itNmea, lineCount, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "lineCount = " << lineCount;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<char>(itNmea, messageMode, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"messageMode = " << messageMode;

			/*------------ Field 04 ---------------*/
			if (!decodeString(itNmea, routeName, defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"routeName = " << routeName;

			/*------------ Field 05 ---------------*/
			for (uint i = 0; i < (fields.size() - 6); i++) {
				waypointNames.push_back((*itNmea));
				BOOST_LOG_TRIVIAL(debug) <<"waypointName "<< i + 1 <<" = " << (*itNmea);
				++itNmea;
			}
		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << rteSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseVHW(const std::string& nmea,
		double& speedInKnots, char& knots, double& speedInKmH,
		char& kilometers) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseVHW";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint vhwSizeField = 10;

	if (fields.size() == vhwSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "VHW") {
			++itNmea;
			/*------------ Field 01 ---------------*/
			++itNmea;
			/*------------ Field 02 ---------------*/
			++itNmea;
			/*------------ Field 03 ---------------*/
			++itNmea;
			/*------------ Field 04 ---------------*/
			++itNmea;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<double>(itNmea, speedInKnots, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"speedInKnots = " << speedInKnots;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<char>(itNmea, knots, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"knots = " << knots;

			/*------------ Field 07 ---------------*/
			if (!decodeDefault<double>(itNmea, speedInKmH, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"speedInKmH = " << speedInKmH;

			/*------------ Field 08 ---------------*/
			if (!decodeDefault<char>(itNmea, kilometers, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"kilometers = " << kilometers;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << vhwSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseMTW(const std::string& nmea, double& degrees,
		char& celcius) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseMTW";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint mtwSizeField = 4;

	if (fields.size() == mtwSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "MTW") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, degrees, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"degrees = " << degrees;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<char>(itNmea, celcius, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"celcius = " << celcius;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << mtwSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseVBW(const std::string& nmea,
		double& longitudinalWaterSpeed, double& transverseWaterSpeed,
		char& waterDataStatus, double& longitudinalGroundSpeed,
		double& transverseGroundSpeed, char& groundDataStatus) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseVBW";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint vbwSizeField = 12;

	if (fields.size() == vbwSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "VBW") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, longitudinalWaterSpeed, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "longitudinalWaterSpeed = " << longitudinalWaterSpeed;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<double>(itNmea, transverseWaterSpeed, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "transverseWaterSpeed = " << transverseWaterSpeed;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<char>(itNmea, waterDataStatus, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"waterDataStatus = " << waterDataStatus;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<double>(itNmea, longitudinalGroundSpeed, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "longitudinalGroundSpeed = " << longitudinalGroundSpeed;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<double>(itNmea, transverseGroundSpeed, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "transverseGroundSpeed = " << transverseGroundSpeed;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<char>(itNmea, groundDataStatus, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"groundDataStatus = " << groundDataStatus;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << vbwSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseVLW(const std::string& nmea,
		double& totalCumulativeDistance, char& nauticalMiles1,
		double& distanceSinceReset, char& nauticalMiles2) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseVLW";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint vlwSizeField = 6;

	if (fields.size() == vlwSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "VLW") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, totalCumulativeDistance, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "totalCumulativeDistance = " << totalCumulativeDistance;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<char>(itNmea, nauticalMiles1, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"nauticalMiles1 = " << nauticalMiles1;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<double>(itNmea, distanceSinceReset, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"distanceSinceReset = " << distanceSinceReset;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<char>(itNmea, nauticalMiles2, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"nauticalMiles2 = " << nauticalMiles2;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << vlwSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseDPT(const std::string& nmea,
		double& waterDepthRelativeToTheTransducer, double& offsetFromTransducer,
		double& maximumRangeScaleInUse) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseDPT";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint dptSizeField = 5;

	if (fields.size() == dptSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "DPT") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea,
							waterDepthRelativeToTheTransducer, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "waterDepthRelativeToTheTransducer = " << waterDepthRelativeToTheTransducer;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<double>(itNmea, offsetFromTransducer, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "offsetFromTransducer = " << offsetFromTransducer;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<double>(itNmea, maximumRangeScaleInUse, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "maximumRangeScaleInUse = " << maximumRangeScaleInUse;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << dptSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseDBT(const std::string& nmea,
		double& waterDepthInFeet, char& feet, double& waterDepthInMeters,
		char& meters, double& waterDepthInFathoms, char& fathoms) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseDBT";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint dbtSizeField = 8;

	if (fields.size() == dbtSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "DBT") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, waterDepthInFeet, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"waterDepthInFeet = " << waterDepthInFeet;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<char>(itNmea, feet, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"feet = " << feet;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<double>(itNmea, waterDepthInMeters, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"waterDepthInMeters = " << waterDepthInMeters;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<char>(itNmea, meters, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"meters = " << meters;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<double>(itNmea, waterDepthInFathoms, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "waterDepthInFathoms = " << waterDepthInFathoms;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<char>(itNmea, fathoms, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"fathoms = " << fathoms;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << dbtSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseDBK(const std::string& nmea,
		double& depthBelowKeelFeet, char& feet, double& depthBelowKeelMeters,
		char& meters, double& depthBelowKeelFathoms, char& fathoms) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseDBK";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint dbkSizeField = 8;

	if (fields.size() == dbkSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "DBK") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, depthBelowKeelFeet, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"depthBelowKeelFeet = " << depthBelowKeelFeet;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<char>(itNmea, feet, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"feet = " << feet;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<double>(itNmea, depthBelowKeelMeters, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "depthBelowKeelMeters = " << depthBelowKeelMeters;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<char>(itNmea, meters, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "meters = " << meters;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<double>(itNmea, depthBelowKeelFathoms, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "depthBelowKeelFathoms = " << depthBelowKeelFathoms;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<char>(itNmea, fathoms, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "fathoms = " << fathoms;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << dbkSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parsePSKPDPT(const std::string& nmea,
		double& waterDepthRelativeToTheTransducer, double& offsetFromTransducer,
		double& maximumRangeScaleInUse, int& bottomEchoStrength,
		int& echoSounderChannelNumber, std::string& transducerLocation) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parsePSKPDPT";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint dptSizeField = 8;

	if (fields.size() == dptSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(1, 7) == "PSKPDPT") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea,
							waterDepthRelativeToTheTransducer, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "waterDepthRelativeToTheTransducer = " << waterDepthRelativeToTheTransducer;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<double>(itNmea, offsetFromTransducer, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "offsetFromTransducer = " << offsetFromTransducer;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<double>(itNmea, maximumRangeScaleInUse, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "maximumRangeScaleInUse = " << maximumRangeScaleInUse;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<int>(itNmea, bottomEchoStrength, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "bottomEchoStrength = " << bottomEchoStrength;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<int>(itNmea, echoSounderChannelNumber, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "echoSounderChannelNumber = " << echoSounderChannelNumber;

			/*------------ Field 06 ---------------*/
			if (!decodeString(itNmea, transducerLocation, defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "transducerLocation = " << transducerLocation;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << dptSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseHDT(const std::string& nmea,
		double& headingDegreesTrue, char& t) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseHDT";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint hdtSizeField = 4;

	if (fields.size() == hdtSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "HDT") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, headingDegreesTrue, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"headingDegreesTrue = " << headingDegreesTrue;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<char>(itNmea, t, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"t = " << t;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << hdtSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseHDG(const std::string& nmea,
		double& magneticSensorHeadingInDegrees,
		double& magneticDeviationDegrees, char& magneticDeviationDirection,
		double& magneticVariationDegrees, char& magneticVariationDirection) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseHDG";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint hdgSizeField = 7;

	if (fields.size() == hdgSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "HDG") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, magneticSensorHeadingInDegrees,
							0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "magneticSensorHeadingInDegrees = " << magneticSensorHeadingInDegrees;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<double>(itNmea, magneticDeviationDegrees, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "magneticDeviationDegrees = " << magneticDeviationDegrees;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<char>(itNmea, magneticDeviationDirection, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "magneticDeviationDirection = " << magneticDeviationDirection;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<double>(itNmea, magneticVariationDegrees, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "magneticVariationDegrees = " << magneticVariationDegrees;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<char>(itNmea, magneticVariationDirection, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "magneticVariationDirection = " << magneticVariationDirection;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << hdgSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseHDM(const std::string& nmea,
		double& headingDegreesMagnetic, char& magnetic) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseHDM";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint hdmSizeField = 4;

	if (fields.size() == hdmSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "HDM") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, headingDegreesMagnetic, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "headingDegreesMagnetic = " << headingDegreesMagnetic;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<char>(itNmea, magnetic, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "magnetic = " << magnetic;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << hdmSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseROT(const std::string& nmea,
		double& rateOfTurn, char& status) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseROT";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint rotSizeField = 4;

	if (fields.size() == rotSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "ROT") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, rateOfTurn, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "rateOfTurn = " << rateOfTurn;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<char>(itNmea, status, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "status = " << status;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << rotSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseMWV(const std::string& nmea,
		double& windAngle, char& reference, double& windSpeed,
		char& windSpeedUnits, char& sensorStatus) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseMWV";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint mwvSizeField = 7;

	if (fields.size() == mwvSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "MWV") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, windAngle, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"windAngle = " << windAngle;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<char>(itNmea, reference, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"reference = " << reference;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<double>(itNmea, windSpeed, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"windSpeed = " << windSpeed;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<char>(itNmea, windSpeedUnits, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"windSpeedUnits = " << windSpeedUnits;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<char>(itNmea, sensorStatus, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"sensorStatus = " << sensorStatus;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << mwvSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseMWD(const std::string& nmea,
		double& trueWindDirection, char& t, double& magneticWindDirection,
		char& magnetic, double& windSpeedKnots, char& knots,
		double& windSpeedMeters, char& meters) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseMWD";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint mwdSizeField = 10;

	if (fields.size() == mwdSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "MWD") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, trueWindDirection, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"trueWindDirection = " << trueWindDirection;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<char>(itNmea, t, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"t = " << t;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<double>(itNmea, magneticWindDirection, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "magneticWindDirection = " << magneticWindDirection;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<char>(itNmea, magnetic, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"magnetic = " << magnetic;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<double>(itNmea, windSpeedKnots, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"windSpeedKnots = " << windSpeedKnots;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<char>(itNmea, knots, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"knots = " << knots;

			/*------------ Field 07 ---------------*/
			if (!decodeDefault<double>(itNmea, windSpeedMeters, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"windSpeedMeters = " << windSpeedMeters;

			/*------------ Field 08 ---------------*/
			if (!decodeDefault<char>(itNmea, meters, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"meters = " << meters;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << mwdSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseXDR(const std::string& nmea,
		char& typeOfSensorTemperature, double& temperatureReading,
		char& centigrade, std::string& nameOfTransducer,
		char& typeOfSensorPressure, double& pressureReading, char& bars,
		std::string& nameOfPressureSensor, char& typeOfSensorHumidity,
		double& humidity, char& humidityUnitsOfMeasurePercent,
		std::string& nameOfRelativeHumiditySensor) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseXDR";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint xdrSizeField = 14;

	if (fields.size() == xdrSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "XDR") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<char>(itNmea, typeOfSensorTemperature, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "typeOfSensorTemperature = " << typeOfSensorTemperature;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<double>(itNmea, temperatureReading, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"temperatureReading = " << temperatureReading;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<char>(itNmea, centigrade, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"centigrade = " << centigrade;

			/*------------ Field 04 ---------------*/
			if (!decodeString(itNmea, nameOfTransducer, defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"nameOfTransducer = " << nameOfTransducer;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<char>(itNmea, typeOfSensorPressure, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "typeOfSensorPressure = " << typeOfSensorPressure;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<double>(itNmea, pressureReading, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "pressureReading = " << pressureReading;

			/*------------ Field 07 ---------------*/
			if (!decodeDefault<char>(itNmea, bars, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "bars = " << bars;

			/*------------ Field 08 ---------------*/
			if (!decodeString(itNmea, nameOfPressureSensor, defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "nameOfPressureSensor = " << nameOfPressureSensor;

			/*------------ Field 09 ---------------*/
			if (!decodeDefault<char>(itNmea, typeOfSensorHumidity, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "typeOfSensorHumidity = " << typeOfSensorHumidity;

			/*------------ Field 10 ---------------*/
			if (!decodeDefault<double>(itNmea, humidity, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "humidity = " << humidity;

			/*------------ Field 11 ---------------*/
			if (!decodeDefault<char>(itNmea, humidityUnitsOfMeasurePercent, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "humidityUnitsOfMeasurePercent = " << humidityUnitsOfMeasurePercent;

			/*------------ Field 12 ---------------*/
			if (!decodeString(itNmea, nameOfRelativeHumiditySensor,
							defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "nameOfRelativeHumiditySensor = " << nameOfRelativeHumiditySensor;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << xdrSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseTTM(const std::string& nmea,
		int& targetNumber, double& targetDistance, double& targetBearing,
		Nmea_AngleReference& targetBearingReference, double& targetSpeed,
		double& targetCourse, Nmea_AngleReference& targetCourseReference,
		Nmea_SpeedDistanceUnits& speedDistanceUnits, std::string& targetName,
		Nmea_TargetStatus& targetStatus,
		boost::posix_time::time_duration& timeOfData,
		Nmea_TypeOfAcquisition& typeOfAcquisition) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseTTM";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	char auxChar;

	uint ttmSizeField = 17;
	if (fields.size() == ttmSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "TTM") {
			itNmea++;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<int>(itNmea, targetNumber, 0)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "targetNumber = " << targetNumber;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<double>(itNmea, targetDistance, 0)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "targetDistance = " << targetDistance;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<double>(itNmea, targetBearing, 0)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "targetBearing = " << targetBearing;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<char>(itNmea, auxChar, defChar)) {
				ret.set(idxVar, true);
				itNmea++;
			} else {
				if (auxChar == 'T') {
					targetBearingReference = Nmea_AngleReference_True;
				} else {
					targetBearingReference = Nmea_AngleReference_Relative;
				}
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "targetBearingReference = " << auxChar;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<double>(itNmea, targetSpeed, 0)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "targetSpeed = " << targetSpeed;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<double>(itNmea, targetCourse, 0)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "targetCourse = " << targetCourse;

			/*------------ Field 07 ---------------*/
			if (!decodeDefault<char>(itNmea, auxChar, defChar)) {
				ret.set(idxVar, true);
				itNmea++;
			} else {
				if (auxChar == 'T') {
					targetCourseReference = Nmea_AngleReference_True;
				} else {
					targetCourseReference = Nmea_AngleReference_Relative;
				}
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "targetCourseReference = " << auxChar;

			/*------------ Field 08 ---------------*/
			// ignorar
			itNmea++;

			/*------------ Field 09 ---------------*/
			// ignorar
			itNmea++;

			/*------------ Field 10 ---------------*/
			if (!decodeDefault<char>(itNmea, auxChar, defChar)) {
				ret.set(idxVar, true);
				itNmea++;
			} else {
				if (auxChar == 'K') {
					speedDistanceUnits = Nmea_SpeedDistanceUnits_Kph_Kilometers;
				} else if (auxChar == 'N') {
					speedDistanceUnits =
					Nmea_SpeedDistanceUnits_Knots_NauticalMiles;
				} else {
					speedDistanceUnits = Nmea_SpeedDistanceUnits_Mps_Meters;
				}
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "speedDistanceUnits = " << auxChar;

			/*------------ Field 11 ---------------*/
			if (!decodeString(itNmea, targetName, defString)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "targetName = " << targetName;

			/*------------ Field 12 ---------------*/
			if (!decodeDefault<char>(itNmea, auxChar, defChar)) {
				ret.set(idxVar, true);
				itNmea++;
			} else {
				if (auxChar == 'L') {
					targetStatus = Nmea_TargetStatus_Lost;
				} else if (auxChar == 'Q') {
					targetStatus = Nmea_TargetStatus_Query;
				} else {
					targetStatus = Nmea_TargetStatus_Tracking;
				}
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "targetStatus = " << auxChar;

			/*------------ Field 13 ---------------*/
			// ignorar
			itNmea++;

			/*------------ Field 14 ---------------*/
			if (!decodeTime(itNmea, timeOfData, deftime)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "timeOfData = " << timeOfData;

			/*------------ Field 15 ---------------*/
			if (!decodeDefault<char>(itNmea, auxChar, defChar)) {
				ret.set(idxVar, true);
				itNmea++;
			} else {
				if (auxChar == 'A') {
					typeOfAcquisition = Nmea_TypeOfAcquisition_Automatic;
				} else if (auxChar == 'M') {
					typeOfAcquisition = Nmea_TypeOfAcquisition_Manual;
				} else {
					typeOfAcquisition = Nmea_TypeOfAcquisition_Reported;
				}
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "typeOfAcquisition = " << auxChar;
		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << ttmSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;
}

NmeaParserResult NmeaParser::parseTTD(const std::string& nmea, int& totalLines,
		int& lineCount, int& sequenceIdentifier, std::string& trackData,
		int& fillBits) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseTTD";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint ttdSizeField = 7;

	if (fields.size() == ttdSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "TTD") {
			++itNmea;

			uint aux;
			/*------------ Field 01 ---------------*/
			if (!decodeHex(itNmea, aux, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			totalLines = aux;
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"totalLines = " << totalLines;

			/*------------ Field 02 ---------------*/
			if (!decodeHex(itNmea, aux, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			lineCount = aux;
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"lineCount = " << lineCount;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<int>(itNmea, sequenceIdentifier, -1)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "sequenciaIdentifier = " << sequenceIdentifier;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<std::string>(itNmea, trackData, defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"trackData = " << trackData;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<int>(itNmea, fillBits, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"fillBits = " << fillBits;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << ttdSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseTLB(const std::string& nmea,
		std::vector<std::pair<int, std::string>>& trackNumbernLabel) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseTLB";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint tlbSizeField = 4;

	if (fields.size() >= tlbSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "TLB") {
			++itNmea;

			std::pair<int, std::string> trackPair;
			for (uint i = 0; i < (fields.size() - 2) / 2; i++) {
				BOOST_LOG_TRIVIAL(debug) <<"i = " << i;
				/*------------ targetNumber ---------------*/
				if (!decodeDefault<int>(itNmea, trackPair.first, 0)) {
					ret.set(i, true);
					++itNmea;
				}
				BOOST_LOG_TRIVIAL(debug) <<"targetNumber = " << trackPair.first;

				/*------------ targetLabel ---------------*/
				if (!decodeDefault<std::string>(itNmea, trackPair.second,
								defString)) {
					ret.set(i + 1, true);
					++itNmea;
				}
				BOOST_LOG_TRIVIAL(debug) <<"targetLabel = " << trackPair.second;

				trackNumbernLabel.push_back(trackPair);

			}
		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << tlbSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseOSD(const std::string& nmea, double& heading,
		char& status, double& vesselCourse, char& referenceCourse,
		double& vesselSpeed, char& referenceSpeed, double& vesselSet,
		double& vesselDrift, char& speedUnits) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseOSD";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint osdSizeField = 11;

	if (fields.size() == osdSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "OSD") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, heading, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"heading = " << heading;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<char>(itNmea, status, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"status = " << status;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<double>(itNmea, vesselCourse, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"vesselCourse = " << vesselCourse;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<char>(itNmea, referenceCourse, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"referenceCourse = " << referenceCourse;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<double>(itNmea, vesselSpeed, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"vesselSpeed = " << vesselSpeed;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<char>(itNmea, referenceSpeed, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"referenceSpeed = " << referenceSpeed;

			/*------------ Field 07 ---------------*/
			if (!decodeDefault<double>(itNmea, vesselSet, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"vesselSet = " << vesselSet;

			/*------------ Field 08 ---------------*/
			if (!decodeDefault<double>(itNmea, vesselDrift, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"vesselDrift = " << vesselDrift;

			/*------------ Field 09 ---------------*/
			if (!decodeDefault<char>(itNmea, speedUnits, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"speedUnits = " << speedUnits;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << osdSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseRSD(const std::string& nmea,
		double& origin1Range, double& origin1BearingDegrees,
		double& variableRangeMarker1, double& bearingLine1,
		double& origin2Range, double& origin2Bearing, double& vrm2,
		double& ebl2, double& cursorRange, double& cursorBearing,
		double& rangeScale, char& rangeUnits, char& displayRotation) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseRSD";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint rsdSizeField = 15;

	if (fields.size() == rsdSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "RSD") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!decodeDefault<double>(itNmea, origin1Range, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "origin1Range = " << origin1Range;

			/*------------ Field 02 ---------------*/
			if (!decodeDefault<double>(itNmea, origin1BearingDegrees, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "origin1BearingDegrees = " << origin1BearingDegrees;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<double>(itNmea, variableRangeMarker1, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "variableRangeMarker1 = " << variableRangeMarker1;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<double>(itNmea, bearingLine1, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"bearingLine1 = " << bearingLine1;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<double>(itNmea, origin2Range, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"origin2Range = " << origin2Range;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<double>(itNmea, origin2Bearing, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"origin2Bearing = " << origin2Bearing;

			/*------------ Field 07 ---------------*/
			if (!decodeDefault<double>(itNmea, vrm2, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"vrm2 = " << vrm2;

			/*------------ Field 08 ---------------*/
			if (!decodeDefault<double>(itNmea, ebl2, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"ebl2 = " << ebl2;

			/*------------ Field 09 ---------------*/
			if (!decodeDefault<double>(itNmea, cursorRange, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"cursorRange = " << cursorRange;

			/*------------ Field 10 ---------------*/
			if (!decodeDefault<double>(itNmea, cursorBearing, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"cursorBearing = " << cursorBearing;

			/*------------ Field 11 ---------------*/
			if (!decodeDefault<double>(itNmea, rangeScale, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"rangeScale = " << rangeScale;

			/*------------ Field 12 ---------------*/
			if (!decodeDefault<char>(itNmea, rangeUnits, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"rangeUnits = " << rangeUnits;

			/*------------ Field 13 ---------------*/
			if (!decodeDefault<char>(itNmea, displayRotation, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"displayRotation = " << displayRotation;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << rsdSizeField;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;
}

NmeaParserResult NmeaParser::parseVDM(const std::string& nmea, int& totalLines,
		int& lineCount, int& sequenceIdentifier, char& aisChannel,
		std::string& encodedData, int& fillBits) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseVDM";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint expectedFieldCount = 8;

	if (fields.size() == expectedFieldCount) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "VDM") {
			++itNmea;

			uint aux;
			/*------------ Field 01 ---------------*/
			if (!decodeHex(itNmea, aux, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			totalLines = aux;
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"totalLines = " << totalLines;

			/*------------ Field 02 ---------------*/
			if (!decodeHex(itNmea, aux, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			lineCount = aux;
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"lineCount = " << lineCount;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<int>(itNmea, sequenceIdentifier, -1)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "sequenciaIdentifier = " << sequenceIdentifier;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<char>(itNmea, aisChannel, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) << "aisChannel = " << aisChannel;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<std::string>(itNmea, encodedData, defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"encodedData = " << encodedData;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<int>(itNmea, fillBits, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			BOOST_LOG_TRIVIAL(debug) <<"fillBits = " << fillBits;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << expectedFieldCount;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;
}

NmeaParserResult NmeaParser::parseVDO(const std::string& nmea, int& totalLines,
		int& lineCount, int& sequenceIdentifier, char& aisChannel,
		std::string& encodedData, int& fillBits) {

	BOOST_LOG_TRIVIAL(trace)<< "NmeaParser::parseVDO";
	BOOST_LOG_TRIVIAL(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	tokenizeSentence(nmea, fields);

	uint expectedFieldCount = 8;

	if (fields.size() == expectedFieldCount) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "VDO") {
			++itNmea;

			uint aux;
			/*------------ Field 01 ---------------*/
			if (!decodeHex(itNmea, aux, 0)) {
				ret.set(0, true);
				++itNmea;
			}
			totalLines = aux;
			BOOST_LOG_TRIVIAL(debug) <<"totalLines = " << totalLines;

			/*------------ Field 02 ---------------*/
			if (!decodeHex(itNmea, aux, 0)) {
				ret.set(1, true);
				++itNmea;
			}
			lineCount = aux;
			BOOST_LOG_TRIVIAL(debug) <<"lineCount = " << lineCount;

			/*------------ Field 03 ---------------*/
			if (!decodeDefault<int>(itNmea, sequenceIdentifier, -1)) {
				ret.set(2, true);
				++itNmea;
			}
			BOOST_LOG_TRIVIAL(debug) << "sequenciaIdentifier = " << sequenceIdentifier;

			/*------------ Field 04 ---------------*/
			if (!decodeDefault<char>(itNmea, aisChannel, defChar)) {
				ret.set(3, true);
				++itNmea;
			}
			BOOST_LOG_TRIVIAL(debug) << "aisChannel = " << aisChannel;

			/*------------ Field 05 ---------------*/
			if (!decodeDefault<std::string>(itNmea, encodedData, defString)) {
				ret.set(4, true);
				++itNmea;
			}
			BOOST_LOG_TRIVIAL(debug) <<"encodedData = " << encodedData;

			/*------------ Field 06 ---------------*/
			if (!decodeDefault<int>(itNmea, fillBits, 0)) {
				ret.set(5, true);
				++itNmea;
			}
			BOOST_LOG_TRIVIAL(debug) <<"fillBits = " << fillBits;

		} else {
			BOOST_LOG_TRIVIAL(error) << "Cabecera incorrecta";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Campos esperados : " << expectedFieldCount;
		BOOST_LOG_TRIVIAL(error) << "Campos recibidos : " << fields.size();
	}

	BOOST_LOG_TRIVIAL(debug) << "retorno binario : " << ret;

	return ret;
}

bool NmeaParser::parseTTDPayload(const std::string& trackData, std::vector<NmeaTrackData>& tracks)
{
	bool ret = false;

	BOOST_LOG_TRIVIAL(trace) << "NmeaParser::parseTTDPayload";
	BOOST_LOG_TRIVIAL(debug) << "trackData = " << trackData;

	const int TDD_TOTALBITS = 90;
	const int TDD_TOTALCHARS = 15;

    if (trackData.size() % TDD_TOTALCHARS == 0)
    {
    	ret = true;

    	// Cada track ocupa 15 caracteres.
        int trackCount = trackData.size() / TDD_TOTALCHARS;
    	BOOST_LOG_TRIVIAL(debug) << "trackCount = " << trackCount;
    	tracks.resize(trackCount);

        for (int i = 0; i < trackCount; ++i)
        {
        	boost::dynamic_bitset<> trackBinary(TDD_TOTALBITS);
            uint offset = i * TDD_TOTALCHARS;

            // Decodifica cadena a array de bits
            BOOST_LOG_TRIVIAL(debug) << "parseTTDPayload : decode INIT";
            for (int j = 0; j < TDD_TOTALCHARS; ++j)
            {
                int pos = offset + j;
                SixBit bitsetDecode = decodeSixBit(trackData.at(pos));
                BOOST_LOG_TRIVIAL(debug) << "Char: " << trackData.at(pos) << " Bits: " << bitsetDecode;

                // Concatenate all six-bit quantities found in the payload, MSB first
                concatSixBitMSBFirst(j * 6, trackBinary, bitsetDecode);
            }
            BOOST_LOG_TRIVIAL(debug) << "parseTTDPayload : decode END " << trackBinary;

            int cursor = 0;

            // Solo version 0
            if (trackBinary[cursor++] == false
                    && trackBinary[cursor++] == false)
            {
            	tracks[i].targetNumber = decodeUInt(trackBinary, cursor, 10);
            	cursor += 10;
                BOOST_LOG_TRIVIAL(debug) << "TargetNumber = " << tracks[i].targetNumber;

                tracks[i].trueBearing = decodeUInt(trackBinary, cursor, 12) * 0.1f;
                cursor += 12;
                BOOST_LOG_TRIVIAL(debug) << "TrueBearing = " << tracks[i].trueBearing;

                tracks[i].speed = decodeUInt(trackBinary, cursor, 12) * 0.1f;
                cursor += 12;
                BOOST_LOG_TRIVIAL(debug) << "Speed = " << tracks[i].speed;

                tracks[i].course = decodeUInt(trackBinary, cursor, 12) * 0.1f;
                cursor += 12;
                BOOST_LOG_TRIVIAL(debug) << "Course = " << tracks[i].course;

                tracks[i].aisHeading = decodeUInt(trackBinary, cursor, 12) * 0.1f;
                cursor += 12;
                BOOST_LOG_TRIVIAL(debug) << "AisHeading = " << tracks[i].aisHeading;

                // Track status
                uint status = decodeUInt(trackBinary, cursor, 3);
                cursor += 3;
                switch (status)
                {
                case 0:
                	tracks[i].status = Nmea_TrackStatus_Non_tracking;
                	break;

                case 1:
                	tracks[i].status = Nmea_TrackStatus_Acquiring;
                	break;

                case 2:
                	tracks[i].status = Nmea_TrackStatus_Lost;
                	break;

                case 3:
                	tracks[i].status = Nmea_TrackStatus_Reserved_1;
                	break;

                case 4:
                	tracks[i].status = Nmea_TrackStatus_Tracking;
                	break;

                case 5:
                	tracks[i].status = Nmea_TrackStatus_Reserved_2;
                	break;

                case 6:
                	tracks[i].status = Nmea_TrackStatus_Tracking_CPA_Alarm;
                	break;

                case 7:
                	tracks[i].status = Nmea_TrackStatus_Tracking_CPA_Alarm_Ack;
                	break;
                }
                BOOST_LOG_TRIVIAL(debug) << "Status = " << tracks[i].status;

                // Operation mode
                if (trackBinary[cursor++] == false)
                {
                	tracks[i].operation = Nmea_Operation_Autonomous;
                } else {
                	tracks[i].operation = Nmea_Operation_TestTarget;
                }
                BOOST_LOG_TRIVIAL(debug) << "Operation = " << tracks[i].operation;

                tracks[i].distance = decodeUInt(trackBinary, cursor, 14) * 0.01f;
                cursor += 14;
                BOOST_LOG_TRIVIAL(debug) << "Distance = " << tracks[i].distance;

                // Speed mode
                if (trackBinary[cursor++] == false)
                {
                	tracks[i].speedMode = Nmea_SpeedMode_TrueSpeedCourse;
                } else {
                	tracks[i].speedMode = Nmea_SpeedMode_Relative;
                }
                BOOST_LOG_TRIVIAL(debug) << "Speed mode = " << tracks[i].speedMode;

                // Stabilisation mode
                if (trackBinary[cursor++] == false)
                {
                	tracks[i].stabilisationMode = Nmea_StabilisationMode_OverGround;
                } else {
                	tracks[i].stabilisationMode = Nmea_StabilisationMode_ThroughWater;
                }
                BOOST_LOG_TRIVIAL(debug) << "Stabilisation mode = " << tracks[i].stabilisationMode;

                // Reservado
                // Skip 2 bit
                cursor++;
                cursor++;

                tracks[i].correlationNumber = decodeUInt(trackBinary, cursor, 8);
                BOOST_LOG_TRIVIAL(debug) << "CorrelationNumber = " << tracks[i].correlationNumber;
            }
        }
    }

    return ret;
}

bool NmeaParser::parseAISMessageType(const std::string& encodedData, int& messageType)
{
	bool ret = false;

	BOOST_LOG_TRIVIAL(trace) << "NmeaParser::parseAISMessageType";
	BOOST_LOG_TRIVIAL(debug) << "encodedData = " << encodedData;

    SixBit bitsetDecode = decodeSixBit(encodedData.at(0));
	BOOST_LOG_TRIVIAL(debug) << "Char: " << encodedData.at(0) << " Bits: " << bitsetDecode;

	messageType = bitsetDecode.to_ulong();
	BOOST_LOG_TRIVIAL(debug) << "MessageType = " << messageType;

	return ret;
}

