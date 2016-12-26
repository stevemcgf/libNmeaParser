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
#include <boost/algorithm/string/trim.hpp>

#ifdef NP_DEBUG
#define LOG_MESSAGE(lvl) BOOST_LOG_TRIVIAL(lvl)
#else
#define LOG_MESSAGE(lvl) if (false) BOOST_LOG_TRIVIAL(lvl)
#endif

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
	static bool decodeHex(std::vector<std::string>::iterator &i, uint& out,
			const uint& def);
	template<typename Target>
	static bool decodeDefault(std::vector<std::string>::iterator &i,
			Target &out, const Target& def);

	static SixBit decodeSixBit(char data);
	static void concatSixBitMSBFirst(int pointer, boost::dynamic_bitset<>& dataout, const SixBit& datain);
	static uint decodeBitUInt(const boost::dynamic_bitset<>& data, int pointer, int size);
	static int decodeBitInt(const boost::dynamic_bitset<>& data, int pointer, int size);
	static std::string decodeBitString(const boost::dynamic_bitset<>& data, int pointer, int size);
};

NmeaParser::NmeaParser() {

}

NmeaParser::~NmeaParser() {

}

bool NmeaParser::impl::tokenizeSentence(const std::string& nmea,
		std::vector<std::string>& tokens) {

	tokens.clear();

	const boost::char_separator<char> sep(",*", 0, boost::keep_empty_tokens);
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
		LOG_MESSAGE(debug)<<" decodeTime: time " << *i;
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
		++i; // consume valor.
	} catch (boost::bad_lexical_cast &) {
		out = def;
	}

	return ret;
}

bool NmeaParser::impl::decodeHex(std::vector<std::string>::iterator &i, uint& out,
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
bool NmeaParser::impl::decodeLatLng(std::vector<std::string>::iterator &i,
		double& out, const double& def) {
	bool ret = false;
	boost::smatch m;
	if (boost::regex_match(*i, m, latlng)) {
		ret = true;
		//LOG_MESSAGE(debug)<<"decodeLatLng: num " << *i;
		++i; // consume the match
		double deg = boost::lexical_cast<double>(m[1].str());
		double min = boost::lexical_cast<double>(m[2].str());
		//LOG_MESSAGE(debug)<<"decodeLatLng: hem " << *i;
		char hemisphere = (*i).at(0);
		++i; // consume hemisphere
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
		//LOG_MESSAGE(debug)<<"decodeDate: date " << *i;
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

bool NmeaParser::impl::decodeString(std::vector<std::string>::iterator &i,
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

inline SixBit NmeaParser::impl::decodeSixBit(char data) {
	if (data <= 87) {
		return SixBit(static_cast<uint>(data - 48));
	} else {
		return SixBit(static_cast<uint>(data - 56));
	}
}

inline void NmeaParser::impl::concatSixBitMSBFirst(int pointer, boost::dynamic_bitset<>& dataout, const SixBit& datain)
{
    for (int i = 0; i < 6; ++i)
    {
    	dataout.set(pointer + i, datain[5 - i]);
    }
}

inline uint NmeaParser::impl::decodeBitUInt(const boost::dynamic_bitset<>& data, int pointer, int size)
{
	boost::dynamic_bitset<> binVariable(size);
    for (int i = 0; i < size; i++)
    {
    	binVariable[size - 1 - i] = data[pointer + i];
    }
    return binVariable.to_ulong();
}

inline int NmeaParser::impl::decodeBitInt(const boost::dynamic_bitset<>& data, int pointer, int size)
{
    uint val = decodeBitUInt(data, pointer, size);
    int const m = 1U << (size - 1);
    return (val ^ m) - m;
}

std::string NmeaParser::impl::decodeBitString(const boost::dynamic_bitset<>& data, int pointer, int size)
{
	int len = size / 6;
	std::string strVariable(len, '\0');

	SixBit bitChar;

	for (int i = 0; i < len; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			bitChar[5 - j] = data[pointer + i * 6 + j];
		}
		if (bitChar.to_ulong() < 32)
		{
			strVariable[i] = '@' + bitChar.to_ulong();
		} else {
			strVariable[i] = ' ' + (bitChar.to_ulong() - 32);
		}
	}

	boost::trim(strVariable);
	boost::trim_right_if(strVariable, boost::is_any_of("@"));

	return strVariable;
}

// Parseo de tramas NMEA
// definicion de valores por defecto

boost::posix_time::time_duration deftime(0, 0, 0, 0);
boost::gregorian::date defdate(1400, 1, 1);
const double defLatLong = 99.999;
char defChar = 'X';
std::string defString = "XXXX";

NmeaParserResult NmeaParser::parseZDA(const std::string& nmea,
		boost::posix_time::time_duration& mtime, int& day, int& month,
		int& year, int& localZoneHours, int& localZoneMinutes) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseZDA";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint expectedFieldCount = 8;

	if (fields.size() == expectedFieldCount) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "ZDA") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeTime(itNmea, mtime, deftime)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "mtime = " << mtime;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, day, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "day = " << day;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, month, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "month = " << month;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, year, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "year = " << year;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, localZoneHours, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "localZoneHours = " << localZoneHours;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, localZoneMinutes, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "localZoneMinutes = " << localZoneMinutes;
		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << expectedFieldCount;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseGLL(const std::string& nmea, double& latitude,
		double& longitude, boost::posix_time::time_duration& mtime,
		char& status, char& modeIndicator) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseGLL";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint expectedFieldCount = 9;

	if (fields.size() == expectedFieldCount) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "GLL") {
			++itNmea;

			/*------------ Field 01,02 ---------------*/
			if (!impl::decodeLatLng(itNmea, latitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "latitude = " << latitude;

			/*------------ Field 03,04 ---------------*/
			if (!impl::decodeLatLng(itNmea, longitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "longitude = " << longitude;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeTime(itNmea, mtime, deftime)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "mtime = " << mtime;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, status, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "status = " << status;

			/*------------ Field 07 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, modeIndicator, defChar)) {
				ret.set(idxVar, true);
			}
			++idxVar;
			LOG_MESSAGE(debug) << "modeIndicator = " << modeIndicator;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << expectedFieldCount;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseGGA(const std::string& nmea,
		boost::posix_time::time_duration& mtime, double& latitude,
		double& longitude, Nmea_GPSQualityIndicator& quality, int& numSV,
		double& hdop, double& orthometricheight, double& geoidseparation,
		double& agediffgps, std::string& refid) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseGGA";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint ggaSizeField = 16;

	if (fields.size() == ggaSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "GGA") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeTime(itNmea, mtime, deftime)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "mtime = " << mtime;

			/*------------ Field 02,03 ---------------*/
			if (!impl::decodeLatLng(itNmea, latitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"latitude = " << latitude;

			/*------------ Field 04,05 ---------------*/
			if (!impl::decodeLatLng(itNmea, longitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"longitude = " << longitude;

			/*------------ Field 06 ---------------*/
			int qualityIndicator;
			if (!impl::decodeDefault<int>(itNmea, qualityIndicator, 0)) {
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
			LOG_MESSAGE(debug) << "quality = " << quality;

			/*------------ Field 07 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, numSV, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"numSV = " << numSV;

			/*------------ Field 08 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, hdop, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"hdop = " << hdop;

			/*------------ Field 09 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, orthometricheight, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "orthometricheight = " << orthometricheight;

			/*------------ Field 10 ---------------*/
			// Se salta unidad de medida siempre Metros.
			++itNmea;

			/*------------ Field 11 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, geoidseparation, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"geoidseparation = " << geoidseparation;

			/*------------ Field 12 ---------------*/
			// Se salta unidad de medida siempre Metros.
			++itNmea;

			/*------------ Field 13 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, agediffgps, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"agediffgps = " << agediffgps;

			/*------------ Field 14 ---------------*/
			if (!impl::decodeString(itNmea, refid, defString)) {
				ret.set(idxVar, true);
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"refid = " << refid;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << ggaSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseVTG(const std::string& nmea,
		double& coursetrue, double& coursemagnetic, double& speedknots,
		double& speedkph) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseVTG";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint vtgSizeField = 11;

	if (fields.size() == vtgSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "VTG") {
			++itNmea;

			/*------------ Field 01,02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, coursetrue, 0)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"coursetrue = " << coursetrue;
			++itNmea;
			/*------------ Field 03,04 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, coursemagnetic, 0)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"coursemagnetic = " << coursemagnetic;
			++itNmea;
			/*------------ Field 05,06 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, speedknots, 0)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"speedknots = " << speedknots;
			++itNmea;
			/*------------ Field 07,08 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, speedkph, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"speedkph = " << speedkph;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << vtgSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseRMC(const std::string& nmea,
		boost::posix_time::time_duration& mtime, double& latitude,
		double& longitude, double& speedknots, double& coursetrue,
		boost::gregorian::date& mdate, double& magneticvar) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseRMC";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint rmcSizeField = 15;
	uint rmcSizeFieldAlt = 13;

	if (fields.size() == rmcSizeField || fields.size() == rmcSizeFieldAlt) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "RMC") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeTime(itNmea, mtime, deftime)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"mtime = " << mtime;

			/*------------ Field 02 ---------------*/
			++itNmea;

			/*------------ Field 03,04 ---------------*/
			if (!impl::decodeLatLng(itNmea, latitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"latitude = " << latitude;

			/*------------ Field 05,06 ---------------*/
			if (!impl::decodeLatLng(itNmea, longitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"longitude = " << longitude;

			/*------------ Field 07 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, speedknots, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"speedknots = " << speedknots;

			/*------------ Field 08 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, coursetrue, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"coursetrue = " << coursetrue;

			/*------------ Field 09 ---------------*/
			if (!impl::decodeDate(itNmea, mdate, defdate)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"mdate = " << mdate;

			/*------------ Field 10 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, magneticvar, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"magneticvar = " << magneticvar;

			/*------------ Field 11 ---------------*/
			++itNmea;
			/*------------ Field 12 ---------------*/
			++itNmea;
			/*------------ Field 13 ---------------*/
			++itNmea;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << rmcSizeField << " o " << rmcSizeFieldAlt;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseWPL(const std::string& nmea, double& latitude,
		double& longitude, std::string& waypointName) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseWPL";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint wplSizeField = 7;

	if (fields.size() == wplSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "WPL") {
			++itNmea;

			/*------------ Field 01,02 ---------------*/
			if (!impl::decodeLatLng(itNmea, latitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"latitude = " << latitude;

			/*------------ Field 03,04 ---------------*/
			if (!impl::decodeLatLng(itNmea, longitude, defLatLong)) {
				ret.set(idxVar, true);
				++itNmea;
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"longitude = " << longitude;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeString(itNmea, waypointName, defString)) {
				ret.set(idxVar, true);
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"waypointName = " << waypointName;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << wplSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseRTE(const std::string& nmea, int& totalLines,
		int& lineCount, char& messageMode, std::string& routeName,
		std::vector<std::string>& waypointNames) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseRTE";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint rteSizeField = 7;

	if (fields.size() >= rteSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "RTE") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, totalLines, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "totalLines = " << totalLines;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, lineCount, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "lineCount = " << lineCount;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, messageMode, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"messageMode = " << messageMode;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeString(itNmea, routeName, defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"routeName = " << routeName;

			/*------------ Field 05 ---------------*/
			for (uint i = 0; i < (fields.size() - 6); i++) {
				waypointNames.push_back((*itNmea));
				LOG_MESSAGE(debug) <<"waypointName "<< i + 1 <<" = " << (*itNmea);
				++itNmea;
			}
		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << rteSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseVHW(const std::string& nmea,
		double& speedInKnots, char& knots, double& speedInKmH,
		char& kilometers) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseVHW";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

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
			if (!impl::decodeDefault<double>(itNmea, speedInKnots, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"speedInKnots = " << speedInKnots;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, knots, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"knots = " << knots;

			/*------------ Field 07 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, speedInKmH, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"speedInKmH = " << speedInKmH;

			/*------------ Field 08 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, kilometers, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"kilometers = " << kilometers;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << vhwSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseMTW(const std::string& nmea, double& degrees,
		char& celcius) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseMTW";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint mtwSizeField = 4;

	if (fields.size() == mtwSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "MTW") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, degrees, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"degrees = " << degrees;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, celcius, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"celcius = " << celcius;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << mtwSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseVBW(const std::string& nmea,
		double& longitudinalWaterSpeed, double& transverseWaterSpeed,
		char& waterDataStatus, double& longitudinalGroundSpeed,
		double& transverseGroundSpeed, char& groundDataStatus) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseVBW";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint vbwSizeField = 12;

	if (fields.size() == vbwSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "VBW") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, longitudinalWaterSpeed, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "longitudinalWaterSpeed = " << longitudinalWaterSpeed;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, transverseWaterSpeed, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "transverseWaterSpeed = " << transverseWaterSpeed;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, waterDataStatus, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"waterDataStatus = " << waterDataStatus;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, longitudinalGroundSpeed, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "longitudinalGroundSpeed = " << longitudinalGroundSpeed;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, transverseGroundSpeed, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "transverseGroundSpeed = " << transverseGroundSpeed;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, groundDataStatus, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"groundDataStatus = " << groundDataStatus;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << vbwSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseVLW(const std::string& nmea,
		double& totalCumulativeDistance, char& nauticalMiles1,
		double& distanceSinceReset, char& nauticalMiles2) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseVLW";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint vlwSizeField = 6;

	if (fields.size() == vlwSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "VLW") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, totalCumulativeDistance, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "totalCumulativeDistance = " << totalCumulativeDistance;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, nauticalMiles1, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"nauticalMiles1 = " << nauticalMiles1;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, distanceSinceReset, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"distanceSinceReset = " << distanceSinceReset;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, nauticalMiles2, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"nauticalMiles2 = " << nauticalMiles2;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << vlwSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseDPT(const std::string& nmea,
		double& waterDepthRelativeToTheTransducer, double& offsetFromTransducer,
		double& maximumRangeScaleInUse) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseDPT";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint dptSizeField = 5;

	if (fields.size() == dptSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "DPT") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea,
							waterDepthRelativeToTheTransducer, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "waterDepthRelativeToTheTransducer = " << waterDepthRelativeToTheTransducer;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, offsetFromTransducer, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "offsetFromTransducer = " << offsetFromTransducer;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, maximumRangeScaleInUse, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "maximumRangeScaleInUse = " << maximumRangeScaleInUse;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << dptSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseDBT(const std::string& nmea,
		double& waterDepthInFeet, char& feet, double& waterDepthInMeters,
		char& meters, double& waterDepthInFathoms, char& fathoms) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseDBT";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint dbtSizeField = 8;

	if (fields.size() == dbtSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "DBT") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, waterDepthInFeet, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"waterDepthInFeet = " << waterDepthInFeet;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, feet, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"feet = " << feet;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, waterDepthInMeters, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"waterDepthInMeters = " << waterDepthInMeters;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, meters, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"meters = " << meters;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, waterDepthInFathoms, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "waterDepthInFathoms = " << waterDepthInFathoms;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, fathoms, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"fathoms = " << fathoms;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << dbtSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseDBK(const std::string& nmea,
		double& depthBelowKeelFeet, char& feet, double& depthBelowKeelMeters,
		char& meters, double& depthBelowKeelFathoms, char& fathoms) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseDBK";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint dbkSizeField = 8;

	if (fields.size() == dbkSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "DBK") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, depthBelowKeelFeet, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"depthBelowKeelFeet = " << depthBelowKeelFeet;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, feet, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"feet = " << feet;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, depthBelowKeelMeters, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "depthBelowKeelMeters = " << depthBelowKeelMeters;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, meters, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "meters = " << meters;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, depthBelowKeelFathoms, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "depthBelowKeelFathoms = " << depthBelowKeelFathoms;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, fathoms, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "fathoms = " << fathoms;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << dbkSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parsePSKPDPT(const std::string& nmea,
		double& waterDepthRelativeToTheTransducer, double& offsetFromTransducer,
		double& maximumRangeScaleInUse, int& bottomEchoStrength,
		int& echoSounderChannelNumber, std::string& transducerLocation) {

	LOG_MESSAGE(trace)<< "NmeaParser::parsePSKPDPT";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint dptSizeField = 8;

	if (fields.size() == dptSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(1, 7) == "PSKPDPT") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea,
							waterDepthRelativeToTheTransducer, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "waterDepthRelativeToTheTransducer = " << waterDepthRelativeToTheTransducer;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, offsetFromTransducer, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "offsetFromTransducer = " << offsetFromTransducer;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, maximumRangeScaleInUse, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "maximumRangeScaleInUse = " << maximumRangeScaleInUse;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, bottomEchoStrength, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "bottomEchoStrength = " << bottomEchoStrength;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, echoSounderChannelNumber, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "echoSounderChannelNumber = " << echoSounderChannelNumber;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeString(itNmea, transducerLocation, defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "transducerLocation = " << transducerLocation;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << dptSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseHDT(const std::string& nmea,
		double& headingDegreesTrue, char& t) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseHDT";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint hdtSizeField = 4;

	if (fields.size() == hdtSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "HDT") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, headingDegreesTrue, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"headingDegreesTrue = " << headingDegreesTrue;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, t, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"t = " << t;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << hdtSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseHDG(const std::string& nmea,
		double& magneticSensorHeadingInDegrees,
		double& magneticDeviationDegrees, char& magneticDeviationDirection,
		double& magneticVariationDegrees, char& magneticVariationDirection) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseHDG";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint hdgSizeField = 7;

	if (fields.size() == hdgSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "HDG") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, magneticSensorHeadingInDegrees,
							0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "magneticSensorHeadingInDegrees = " << magneticSensorHeadingInDegrees;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, magneticDeviationDegrees, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "magneticDeviationDegrees = " << magneticDeviationDegrees;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, magneticDeviationDirection, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "magneticDeviationDirection = " << magneticDeviationDirection;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, magneticVariationDegrees, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "magneticVariationDegrees = " << magneticVariationDegrees;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, magneticVariationDirection, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "magneticVariationDirection = " << magneticVariationDirection;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << hdgSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseHDM(const std::string& nmea,
		double& headingDegreesMagnetic, char& magnetic) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseHDM";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint hdmSizeField = 4;

	if (fields.size() == hdmSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "HDM") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, headingDegreesMagnetic, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "headingDegreesMagnetic = " << headingDegreesMagnetic;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, magnetic, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "magnetic = " << magnetic;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << hdmSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseROT(const std::string& nmea,
		double& rateOfTurn, char& status) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseROT";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint rotSizeField = 4;

	if (fields.size() == rotSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "ROT") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, rateOfTurn, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "rateOfTurn = " << rateOfTurn;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, status, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "status = " << status;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << rotSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseMWV(const std::string& nmea,
		double& windAngle, char& reference, double& windSpeed,
		char& windSpeedUnits, char& sensorStatus) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseMWV";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint mwvSizeField = 7;

	if (fields.size() == mwvSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "MWV") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, windAngle, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"windAngle = " << windAngle;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, reference, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"reference = " << reference;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, windSpeed, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"windSpeed = " << windSpeed;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, windSpeedUnits, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"windSpeedUnits = " << windSpeedUnits;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, sensorStatus, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"sensorStatus = " << sensorStatus;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << mwvSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseMWD(const std::string& nmea,
		double& trueWindDirection, char& t, double& magneticWindDirection,
		char& magnetic, double& windSpeedKnots, char& knots,
		double& windSpeedMeters, char& meters) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseMWD";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint mwdSizeField = 10;

	if (fields.size() == mwdSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "MWD") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, trueWindDirection, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"trueWindDirection = " << trueWindDirection;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, t, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"t = " << t;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, magneticWindDirection, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "magneticWindDirection = " << magneticWindDirection;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, magnetic, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"magnetic = " << magnetic;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, windSpeedKnots, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"windSpeedKnots = " << windSpeedKnots;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, knots, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"knots = " << knots;

			/*------------ Field 07 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, windSpeedMeters, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"windSpeedMeters = " << windSpeedMeters;

			/*------------ Field 08 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, meters, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"meters = " << meters;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << mwdSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseXDR(const std::string& nmea,
		char& typeOfSensorTemperature, double& temperatureReading,
		char& centigrade, std::string& nameOfTransducer,
		char& typeOfSensorPressure, double& pressureReading, char& bars,
		std::string& nameOfPressureSensor, char& typeOfSensorHumidity,
		double& humidity, char& humidityUnitsOfMeasurePercent,
		std::string& nameOfRelativeHumiditySensor) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseXDR";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint xdrSizeField = 14;

	if (fields.size() == xdrSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "XDR") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, typeOfSensorTemperature, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "typeOfSensorTemperature = " << typeOfSensorTemperature;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, temperatureReading, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"temperatureReading = " << temperatureReading;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, centigrade, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"centigrade = " << centigrade;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeString(itNmea, nameOfTransducer, defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"nameOfTransducer = " << nameOfTransducer;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, typeOfSensorPressure, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "typeOfSensorPressure = " << typeOfSensorPressure;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, pressureReading, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "pressureReading = " << pressureReading;

			/*------------ Field 07 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, bars, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "bars = " << bars;

			/*------------ Field 08 ---------------*/
			if (!impl::decodeString(itNmea, nameOfPressureSensor, defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "nameOfPressureSensor = " << nameOfPressureSensor;

			/*------------ Field 09 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, typeOfSensorHumidity, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "typeOfSensorHumidity = " << typeOfSensorHumidity;

			/*------------ Field 10 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, humidity, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "humidity = " << humidity;

			/*------------ Field 11 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, humidityUnitsOfMeasurePercent, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "humidityUnitsOfMeasurePercent = " << humidityUnitsOfMeasurePercent;

			/*------------ Field 12 ---------------*/
			if (!impl::decodeString(itNmea, nameOfRelativeHumiditySensor,
							defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "nameOfRelativeHumiditySensor = " << nameOfRelativeHumiditySensor;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << xdrSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

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

	LOG_MESSAGE(trace)<< "NmeaParser::parseTTM";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	char auxChar;

	uint ttmSizeField = 17;
	if (fields.size() == ttmSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "TTM") {
			itNmea++;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, targetNumber, 0)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "targetNumber = " << targetNumber;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, targetDistance, 0)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "targetDistance = " << targetDistance;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, targetBearing, 0)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "targetBearing = " << targetBearing;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, auxChar, defChar)) {
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
			LOG_MESSAGE(debug) << "targetBearingReference = " << auxChar;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, targetSpeed, 0)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "targetSpeed = " << targetSpeed;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, targetCourse, 0)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "targetCourse = " << targetCourse;

			/*------------ Field 07 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, auxChar, defChar)) {
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
			LOG_MESSAGE(debug) << "targetCourseReference = " << auxChar;

			/*------------ Field 08 ---------------*/
			// ignorar
			itNmea++;

			/*------------ Field 09 ---------------*/
			// ignorar
			itNmea++;

			/*------------ Field 10 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, auxChar, defChar)) {
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
			LOG_MESSAGE(debug) << "speedDistanceUnits = " << auxChar;

			/*------------ Field 11 ---------------*/
			if (!impl::decodeString(itNmea, targetName, defString)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "targetName = " << targetName;

			/*------------ Field 12 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, auxChar, defChar)) {
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
			LOG_MESSAGE(debug) << "targetStatus = " << auxChar;

			/*------------ Field 13 ---------------*/
			// ignorar
			itNmea++;

			/*------------ Field 14 ---------------*/
			if (!impl::decodeTime(itNmea, timeOfData, deftime)) {
				ret.set(idxVar, true);
				itNmea++;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "timeOfData = " << timeOfData;

			/*------------ Field 15 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, auxChar, defChar)) {
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
			LOG_MESSAGE(debug) << "typeOfAcquisition = " << auxChar;
		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << ttmSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;
}

NmeaParserResult NmeaParser::parseTTD(const std::string& nmea, int& totalLines,
		int& lineCount, int& sequenceIdentifier, std::string& trackData,
		int& fillBits) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseTTD";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint ttdSizeField = 7;

	if (fields.size() == ttdSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "TTD") {
			++itNmea;

			uint aux;
			/*------------ Field 01 ---------------*/
			if (!impl::decodeHex(itNmea, aux, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			totalLines = aux;
			++idxVar;
			LOG_MESSAGE(debug) <<"totalLines = " << totalLines;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeHex(itNmea, aux, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			lineCount = aux;
			++idxVar;
			LOG_MESSAGE(debug) <<"lineCount = " << lineCount;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, sequenceIdentifier, -1)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "sequenciaIdentifier = " << sequenceIdentifier;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<std::string>(itNmea, trackData, defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"trackData = " << trackData;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, fillBits, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"fillBits = " << fillBits;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << ttdSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseTLB(const std::string& nmea,
		std::vector<std::pair<int, std::string>>& trackNumbernLabel) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseTLB";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint tlbSizeField = 4;

	if (fields.size() >= tlbSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "TLB") {
			++itNmea;

			std::pair<int, std::string> trackPair;
			for (uint i = 0; i < (fields.size() - 2) / 2; i++) {
				LOG_MESSAGE(debug) <<"i = " << i;
				/*------------ targetNumber ---------------*/
				if (!impl::decodeDefault<int>(itNmea, trackPair.first, 0)) {
					ret.set(i, true);
					++itNmea;
				}
				LOG_MESSAGE(debug) <<"targetNumber = " << trackPair.first;

				/*------------ targetLabel ---------------*/
				if (!impl::decodeDefault<std::string>(itNmea, trackPair.second,
								defString)) {
					ret.set(i + 1, true);
					++itNmea;
				}
				LOG_MESSAGE(debug) <<"targetLabel = " << trackPair.second;

				trackNumbernLabel.push_back(trackPair);

			}
		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << tlbSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseOSD(const std::string& nmea, double& heading,
		char& status, double& vesselCourse, char& referenceCourse,
		double& vesselSpeed, char& referenceSpeed, double& vesselSet,
		double& vesselDrift, char& speedUnits) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseOSD";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint osdSizeField = 11;

	if (fields.size() == osdSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "OSD") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, heading, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"heading = " << heading;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, status, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"status = " << status;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, vesselCourse, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"vesselCourse = " << vesselCourse;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, referenceCourse, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"referenceCourse = " << referenceCourse;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, vesselSpeed, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"vesselSpeed = " << vesselSpeed;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, referenceSpeed, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"referenceSpeed = " << referenceSpeed;

			/*------------ Field 07 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, vesselSet, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"vesselSet = " << vesselSet;

			/*------------ Field 08 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, vesselDrift, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"vesselDrift = " << vesselDrift;

			/*------------ Field 09 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, speedUnits, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"speedUnits = " << speedUnits;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << osdSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;

}

NmeaParserResult NmeaParser::parseRSD(const std::string& nmea,
		double& origin1Range, double& origin1BearingDegrees,
		double& variableRangeMarker1, double& bearingLine1,
		double& origin2Range, double& origin2Bearing, double& vrm2,
		double& ebl2, double& cursorRange, double& cursorBearing,
		double& rangeScale, char& rangeUnits, char& displayRotation) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseRSD";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint rsdSizeField = 15;

	if (fields.size() == rsdSizeField) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "RSD") {
			++itNmea;

			/*------------ Field 01 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, origin1Range, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "origin1Range = " << origin1Range;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, origin1BearingDegrees, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "origin1BearingDegrees = " << origin1BearingDegrees;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, variableRangeMarker1, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "variableRangeMarker1 = " << variableRangeMarker1;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, bearingLine1, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"bearingLine1 = " << bearingLine1;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, origin2Range, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"origin2Range = " << origin2Range;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, origin2Bearing, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"origin2Bearing = " << origin2Bearing;

			/*------------ Field 07 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, vrm2, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"vrm2 = " << vrm2;

			/*------------ Field 08 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, ebl2, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"ebl2 = " << ebl2;

			/*------------ Field 09 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, cursorRange, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"cursorRange = " << cursorRange;

			/*------------ Field 10 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, cursorBearing, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"cursorBearing = " << cursorBearing;

			/*------------ Field 11 ---------------*/
			if (!impl::decodeDefault<double>(itNmea, rangeScale, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"rangeScale = " << rangeScale;

			/*------------ Field 12 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, rangeUnits, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"rangeUnits = " << rangeUnits;

			/*------------ Field 13 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, displayRotation, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"displayRotation = " << displayRotation;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << rsdSizeField;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;
}

NmeaParserResult NmeaParser::parseVDM(const std::string& nmea, int& totalLines,
		int& lineCount, int& sequenceIdentifier, char& aisChannel,
		std::string& encodedData, int& fillBits) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseVDM";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	int idxVar = 0;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint expectedFieldCount = 8;

	if (fields.size() == expectedFieldCount) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "VDM") {
			++itNmea;

			uint aux;
			/*------------ Field 01 ---------------*/
			if (!impl::decodeHex(itNmea, aux, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			totalLines = aux;
			++idxVar;
			LOG_MESSAGE(debug) <<"totalLines = " << totalLines;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeHex(itNmea, aux, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			lineCount = aux;
			++idxVar;
			LOG_MESSAGE(debug) <<"lineCount = " << lineCount;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, sequenceIdentifier, -1)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "sequenciaIdentifier = " << sequenceIdentifier;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, aisChannel, defChar)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) << "aisChannel = " << aisChannel;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<std::string>(itNmea, encodedData, defString)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"encodedData = " << encodedData;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, fillBits, 0)) {
				ret.set(idxVar, true);
				++itNmea;
			}
			++idxVar;
			LOG_MESSAGE(debug) <<"fillBits = " << fillBits;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << expectedFieldCount;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;
}

NmeaParserResult NmeaParser::parseVDO(const std::string& nmea, int& totalLines,
		int& lineCount, int& sequenceIdentifier, char& aisChannel,
		std::string& encodedData, int& fillBits) {

	LOG_MESSAGE(trace)<< "NmeaParser::parseVDO";
	LOG_MESSAGE(debug) << "Nmea : " << nmea.c_str();

	NmeaParserResult ret;
	ret.reset();

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	uint expectedFieldCount = 8;

	if (fields.size() == expectedFieldCount) {

		std::vector<std::string>::iterator itNmea = fields.begin();

		if ((*itNmea).substr(3, 3) == "VDO") {
			++itNmea;

			uint aux;
			/*------------ Field 01 ---------------*/
			if (!impl::decodeHex(itNmea, aux, 0)) {
				ret.set(0, true);
				++itNmea;
			}
			totalLines = aux;
			LOG_MESSAGE(debug) <<"totalLines = " << totalLines;

			/*------------ Field 02 ---------------*/
			if (!impl::decodeHex(itNmea, aux, 0)) {
				ret.set(1, true);
				++itNmea;
			}
			lineCount = aux;
			LOG_MESSAGE(debug) <<"lineCount = " << lineCount;

			/*------------ Field 03 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, sequenceIdentifier, -1)) {
				ret.set(2, true);
				++itNmea;
			}
			LOG_MESSAGE(debug) << "sequenciaIdentifier = " << sequenceIdentifier;

			/*------------ Field 04 ---------------*/
			if (!impl::decodeDefault<char>(itNmea, aisChannel, defChar)) {
				ret.set(3, true);
				++itNmea;
			}
			LOG_MESSAGE(debug) << "aisChannel = " << aisChannel;

			/*------------ Field 05 ---------------*/
			if (!impl::decodeDefault<std::string>(itNmea, encodedData, defString)) {
				ret.set(4, true);
				++itNmea;
			}
			LOG_MESSAGE(debug) <<"encodedData = " << encodedData;

			/*------------ Field 06 ---------------*/
			if (!impl::decodeDefault<int>(itNmea, fillBits, 0)) {
				ret.set(5, true);
				++itNmea;
			}
			LOG_MESSAGE(debug) <<"fillBits = " << fillBits;

		} else {
			LOG_MESSAGE(error) << "Cabecera incorrecta";
		}
	} else {
		LOG_MESSAGE(error) << "Campos esperados : " << expectedFieldCount;
		LOG_MESSAGE(error) << "Campos recibidos : " << fields.size();
	}

	LOG_MESSAGE(debug) << "retorno binario : " << ret;

	return ret;
}

bool NmeaParser::parseTTDPayload(const std::string& trackData, std::vector<NmeaTrackData>& tracks)
{
	bool ret = false;

	LOG_MESSAGE(trace) << "NmeaParser::parseTTDPayload";
	LOG_MESSAGE(debug) << "trackData = " << trackData;

	const int TDD_TOTALBITS = 90;
	const int TDD_TOTALCHARS = 15;

    if (trackData.size() % TDD_TOTALCHARS == 0)
    {
    	ret = true;

    	// Cada track ocupa 15 caracteres.
        int trackCount = trackData.size() / TDD_TOTALCHARS;
    	LOG_MESSAGE(debug) << "trackCount = " << trackCount;
    	tracks.resize(trackCount);

        for (int i = 0; i < trackCount; ++i)
        {
        	boost::dynamic_bitset<> trackBinary(TDD_TOTALBITS);
            uint offset = i * TDD_TOTALCHARS;

            // Decodifica cadena a array de bits
            LOG_MESSAGE(debug) << "parseTTDPayload : decode INIT";
            for (int j = 0; j < TDD_TOTALCHARS; ++j)
            {
                int pos = offset + j;
                SixBit bitsetDecode = impl::decodeSixBit(trackData.at(pos));
                LOG_MESSAGE(debug) << "Char: " << trackData.at(pos) << " Bits: " << bitsetDecode;

                // Concatenate all six-bit quantities found in the payload, MSB first
                impl::concatSixBitMSBFirst(j * 6, trackBinary, bitsetDecode);
            }
            LOG_MESSAGE(debug) << "parseTTDPayload : decode END " << trackBinary;

            int cursor = 0;

            // Solo version 0
            if (trackBinary[cursor++] == false
                    && trackBinary[cursor++] == false)
            {
            	tracks[i].targetNumber = impl::decodeBitUInt(trackBinary, cursor, 10);
            	cursor += 10;
                LOG_MESSAGE(debug) << "TargetNumber = " << tracks[i].targetNumber;

                tracks[i].trueBearing = impl::decodeBitUInt(trackBinary, cursor, 12) * 0.1f;
                cursor += 12;
                LOG_MESSAGE(debug) << "TrueBearing = " << tracks[i].trueBearing;

                tracks[i].speed = impl::decodeBitUInt(trackBinary, cursor, 12) * 0.1f;
                cursor += 12;
                LOG_MESSAGE(debug) << "Speed = " << tracks[i].speed;

                tracks[i].course = impl::decodeBitUInt(trackBinary, cursor, 12) * 0.1f;
                cursor += 12;
                LOG_MESSAGE(debug) << "Course = " << tracks[i].course;

                tracks[i].aisHeading = impl::decodeBitUInt(trackBinary, cursor, 12) * 0.1f;
                cursor += 12;
                LOG_MESSAGE(debug) << "AisHeading = " << tracks[i].aisHeading;

                // Track status
                uint status = impl::decodeBitUInt(trackBinary, cursor, 3);
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
                LOG_MESSAGE(debug) << "Status = " << tracks[i].status;

                // Operation mode
                if (trackBinary[cursor++] == false)
                {
                	tracks[i].operation = Nmea_Operation_Autonomous;
                } else {
                	tracks[i].operation = Nmea_Operation_TestTarget;
                }
                LOG_MESSAGE(debug) << "Operation = " << tracks[i].operation;

                tracks[i].distance = impl::decodeBitUInt(trackBinary, cursor, 14) * 0.01f;
                cursor += 14;
                LOG_MESSAGE(debug) << "Distance = " << tracks[i].distance;

                // Speed mode
                if (trackBinary[cursor++] == false)
                {
                	tracks[i].speedMode = Nmea_SpeedMode_TrueSpeedCourse;
                } else {
                	tracks[i].speedMode = Nmea_SpeedMode_Relative;
                }
                LOG_MESSAGE(debug) << "Speed mode = " << tracks[i].speedMode;

                // Stabilisation mode
                if (trackBinary[cursor++] == false)
                {
                	tracks[i].stabilisationMode = Nmea_StabilisationMode_OverGround;
                } else {
                	tracks[i].stabilisationMode = Nmea_StabilisationMode_ThroughWater;
                }
                LOG_MESSAGE(debug) << "Stabilisation mode = " << tracks[i].stabilisationMode;

                // Reservado
                // Skip 2 bit
                cursor++;
                cursor++;

                tracks[i].correlationNumber = impl::decodeBitUInt(trackBinary, cursor, 8);
                cursor += 8;
                LOG_MESSAGE(debug) << "CorrelationNumber = " << tracks[i].correlationNumber;
            }
        }
    }

    return ret;
}

bool NmeaParser::parseAISMessageType(const std::string& encodedData, Nmea_AisMessageType& messageType)
{
	bool ret = false;

	LOG_MESSAGE(trace) << "NmeaParser::parseAISMessageType";
	LOG_MESSAGE(debug) << "encodedData = " << encodedData;

	if (encodedData.length() > 0)
	{
		ret = true;
		SixBit bitsetDecode = impl::decodeSixBit(encodedData.at(0));
		LOG_MESSAGE(debug) << "Char: " << encodedData.at(0) << " Bits: " << bitsetDecode;

		messageType = static_cast<Nmea_AisMessageType>(bitsetDecode.to_ulong());
		LOG_MESSAGE(debug) << "MessageType = " << messageType;
	}

	return ret;
}

bool NmeaParser::parseAISPositionReportClassA(const std::string& encodedData, AISPositionReportClassA& data)
{
	bool ret = false;

	LOG_MESSAGE(trace) << "NmeaParser::parseAISPositionReportClassA";
	LOG_MESSAGE(debug) << "encodedData = " << encodedData;

	const int TYPE1_TOTALBITS = 168;
	const int TYPE1_TOTALCHARS = 28;

	if (encodedData.length() >= TYPE1_TOTALCHARS)
	{

    	boost::dynamic_bitset<> binaryData(TYPE1_TOTALBITS);

        // Decodifica cadena a array de bits
        LOG_MESSAGE(debug) << "parseAISPositionReportClassA : decode INIT";
        for (int i = 0; i < TYPE1_TOTALCHARS; ++i)
        {
            SixBit bitsetDecode = impl::decodeSixBit(encodedData.at(i));
            LOG_MESSAGE(debug) << "Char: " << encodedData.at(i) << " Bits: " << bitsetDecode;

            // Concatenate all six-bit quantities found in the payload, MSB first
            impl::concatSixBitMSBFirst(i * 6, binaryData, bitsetDecode);
        }
        LOG_MESSAGE(debug) << "parseAISPositionReportClassA : decode END " << binaryData;

        int cursor = 0;

        Nmea_AisMessageType messageType = static_cast<Nmea_AisMessageType>(impl::decodeBitUInt(binaryData, cursor, 6));
        cursor += 6;

        if (messageType == Nmea_AisMessageType_PositionReportClassA || messageType == Nmea_AisMessageType_PositionReportClassA_AssignedSchedule || messageType == Nmea_AisMessageType_PositionReportClassA_ResponseToInterrogation)
        {
        	ret = true;

        	data.repeatIndicator = impl::decodeBitUInt(binaryData, cursor, 2);
        	cursor += 2;
            LOG_MESSAGE(debug) << "RepeatIndicator = " << data.repeatIndicator;

        	data.mmsi = impl::decodeBitUInt(binaryData, cursor, 30);
        	cursor += 30;
            LOG_MESSAGE(debug) << "MMSI = " << data.mmsi;

            data.navigationStatus = static_cast<Nmea_NavigationStatus>(impl::decodeBitUInt(binaryData, cursor, 4));
            cursor += 4;
            LOG_MESSAGE(debug) << "NavigationStatus = " << data.navigationStatus;

            float auxrot = impl::decodeBitInt(binaryData, cursor, 8);
            cursor += 8;
            if (auxrot == 128.0f)
            {
            	auxrot = 0.0f;
            }
            data.rateOfTurn = std::copysign((auxrot / 4.733f) * (auxrot / 4.733f), auxrot);
            LOG_MESSAGE(debug) << "RateOfTurn = " << data.rateOfTurn;

            data.speedOverGround = impl::decodeBitUInt(binaryData, cursor, 10) * 0.1f;
            cursor += 10;
            LOG_MESSAGE(debug) << "SpeedOverGround = " << data.speedOverGround;

            if (binaryData[cursor++] == false)
            {
            	data.positionAccuracy = Nmea_PositionAccuracy_UnaugmentedGNSSFix;
            } else {
            	data.positionAccuracy = Nmea_PositionAccuracy_DGPSQualityFix;
            }
            LOG_MESSAGE(debug) << "PositionAccuracy = " << data.positionAccuracy;

            data.longitude = impl::decodeBitInt(binaryData, cursor, 28) / 600000.0f;
            cursor += 28;
            LOG_MESSAGE(debug) << "Longitude = " << data.longitude;

            data.latitude = impl::decodeBitInt(binaryData, cursor, 27) / 600000.0f;
            cursor += 27;
            LOG_MESSAGE(debug) << "Latitude = " << data.latitude;

            data.courseOverGround = impl::decodeBitUInt(binaryData, cursor, 12) * 0.1f;
            cursor += 12;
            LOG_MESSAGE(debug) << "CourseOverGround = " << data.courseOverGround;

            data.trueHeading = impl::decodeBitUInt(binaryData, cursor, 9);
            cursor += 9;
            LOG_MESSAGE(debug) << "TrueHeading = " << data.trueHeading;

            data.timestapUTCSecond = impl::decodeBitUInt(binaryData, cursor, 6);
            cursor += 6;
            LOG_MESSAGE(debug) << "TimestapUTCSecond = " << data.timestapUTCSecond;

            data.maneuverIndicator = static_cast<Nmea_ManeuverIndicator>(impl::decodeBitUInt(binaryData, cursor, 2));
            cursor += 2;
            LOG_MESSAGE(debug) << "ManeuverIndicator = " << data.maneuverIndicator;

            if (binaryData[cursor++] == false)
            {
            	data.raim = Nmea_RAIM_NotInUse;
            } else {
            	data.raim = Nmea_RAIM_InUse;
            }
            LOG_MESSAGE(debug) << "RAIM = " << data.raim;
        }
	}
	return ret;
}

bool NmeaParser::parseAISBaseStationReport(const std::string& encodedData, AISBaseStationReport& data)
{
	bool ret = false;

	LOG_MESSAGE(trace) << "NmeaParser::parseAISBaseStationReport";
	LOG_MESSAGE(debug) << "encodedData = " << encodedData;

	const int TYPE4_TOTALBITS = 168;
	const int TYPE4_TOTALCHARS = 28;

	if (encodedData.length() >= TYPE4_TOTALCHARS)
	{

    	boost::dynamic_bitset<> binaryData(TYPE4_TOTALBITS);

        // Decodifica cadena a array de bits
        LOG_MESSAGE(debug) << "parseAISBaseStationReport : decode INIT";
        for (int i = 0; i < TYPE4_TOTALCHARS; ++i)
        {
            SixBit bitsetDecode = impl::decodeSixBit(encodedData.at(i));
            LOG_MESSAGE(debug) << "Char: " << encodedData.at(i) << " Bits: " << bitsetDecode;

            // Concatenate all six-bit quantities found in the payload, MSB first
            impl::concatSixBitMSBFirst(i * 6, binaryData, bitsetDecode);
        }
        LOG_MESSAGE(debug) << "parseAISBaseStationReport : decode END " << binaryData;

        int cursor = 0;

        Nmea_AisMessageType messageType = static_cast<Nmea_AisMessageType>(impl::decodeBitUInt(binaryData, cursor, 6));
        cursor += 6;

        if (messageType == Nmea_AisMessageType_BaseStationReport)
        {
        	ret = true;

        	data.repeatIndicator = impl::decodeBitUInt(binaryData, cursor, 2);
        	cursor += 2;
            LOG_MESSAGE(debug) << "RepeatIndicator = " << data.repeatIndicator;

        	data.mmsi = impl::decodeBitUInt(binaryData, cursor, 30);
        	cursor += 30;
            LOG_MESSAGE(debug) << "MMSI = " << data.mmsi;

            data.year = impl::decodeBitUInt(binaryData, cursor, 14);
            cursor += 14;
            LOG_MESSAGE(debug) << "Year = " << data.year;

            data.month = impl::decodeBitUInt(binaryData, cursor, 4);
            cursor += 4;
            LOG_MESSAGE(debug) << "Month = " << data.month;

            data.day = impl::decodeBitUInt(binaryData, cursor, 5);
            cursor += 5;
            LOG_MESSAGE(debug) << "Day = " << data.day;

            data.hour = impl::decodeBitUInt(binaryData, cursor, 5);
            cursor += 5;
            LOG_MESSAGE(debug) << "Hour = " << data.hour;

            data.minute = impl::decodeBitUInt(binaryData, cursor, 6);
            cursor += 6;
            LOG_MESSAGE(debug) << "Minute = " << data.minute;

            data.second = impl::decodeBitUInt(binaryData, cursor, 6);
            cursor += 6;
            LOG_MESSAGE(debug) << "Second = " << data.second;

            if (binaryData[cursor++] == false)
            {
            	data.positionAccuracy = Nmea_PositionAccuracy_UnaugmentedGNSSFix;
            } else {
            	data.positionAccuracy = Nmea_PositionAccuracy_DGPSQualityFix;
            }
            LOG_MESSAGE(debug) << "PositionAccuracy = " << data.positionAccuracy;

            data.longitude = impl::decodeBitInt(binaryData, cursor, 28) / 600000.0f;
            cursor += 28;
            LOG_MESSAGE(debug) << "Longitude = " << data.longitude;

            data.latitude = impl::decodeBitInt(binaryData, cursor, 27) / 600000.0f;
            cursor += 27;
            LOG_MESSAGE(debug) << "Latitude = " << data.latitude;

            data.epfd = static_cast<Nmea_EPFDFix>(impl::decodeBitUInt(binaryData, cursor, 4));
            cursor += 4;
            LOG_MESSAGE(debug) << "EPFD = " << data.epfd;

            // Spare
            cursor += 10;

            if (binaryData[cursor++] == false)
            {
            	data.raim = Nmea_RAIM_NotInUse;
            } else {
            	data.raim = Nmea_RAIM_InUse;
            }
            LOG_MESSAGE(debug) << "RAIM = " << data.raim;
        }
	}
	return ret;
}

bool NmeaParser::parseAISStaticAndVoyageRelatedData(const std::string& encodedData, AISStaticAndVoyageRelatedData& data)
{
	bool ret = false;

	LOG_MESSAGE(trace) << "NmeaParser::parseAISStaticAndVoyageRelatedData";
	LOG_MESSAGE(debug) << "encodedData = " << encodedData;

	const int TYPE5_TOTALBITS = 426;
	const int TYPE5_TOTALCHARS = 71;

	if (encodedData.length() >= TYPE5_TOTALCHARS)
	{

    	boost::dynamic_bitset<> binaryData(TYPE5_TOTALBITS);

        // Decodifica cadena a array de bits
        LOG_MESSAGE(debug) << "parseAISStaticAndVoyageRelatedData : decode INIT";
        for (int i = 0; i < TYPE5_TOTALCHARS; ++i)
        {
            SixBit bitsetDecode = impl::decodeSixBit(encodedData.at(i));
            LOG_MESSAGE(debug) << "Char: " << encodedData.at(i) << " Bits: " << bitsetDecode;

            // Concatenate all six-bit quantities found in the payload, MSB first
            impl::concatSixBitMSBFirst(i * 6, binaryData, bitsetDecode);
        }
        LOG_MESSAGE(debug) << "parseAISStaticAndVoyageRelatedData : decode END " << binaryData;

        int cursor = 0;

        Nmea_AisMessageType messageType = static_cast<Nmea_AisMessageType>(impl::decodeBitUInt(binaryData, cursor, 6));
        cursor += 6;

        if (messageType == Nmea_AisMessageType_StaticAndVoyageRelatedData)
        {
        	ret = true;

        	data.repeatIndicator = impl::decodeBitUInt(binaryData, cursor, 2);
        	cursor += 2;
            LOG_MESSAGE(debug) << "RepeatIndicator = " << data.repeatIndicator;

        	data.mmsi = impl::decodeBitUInt(binaryData, cursor, 30);
        	cursor += 30;
            LOG_MESSAGE(debug) << "MMSI = " << data.mmsi;

            data.aisVersion = impl::decodeBitUInt(binaryData, cursor, 2);
            cursor += 2;
            LOG_MESSAGE(debug) << "AisVersion = " << data.aisVersion;

            data.imoNumber = impl::decodeBitUInt(binaryData, cursor, 30);
            cursor += 30;
            LOG_MESSAGE(debug) << "ImoNumber = " << data.imoNumber;

            data.callsign = impl::decodeBitString(binaryData, cursor, 42);
            cursor += 42;
            LOG_MESSAGE(debug) << "CallSign = '" << data.callsign << "'";

            data.vesselName = impl::decodeBitString(binaryData, cursor, 120);
            cursor += 120;
            LOG_MESSAGE(debug) << "VesselName = '" << data.vesselName << "'";

            data.shipType = static_cast<Nmea_ShipType>(impl::decodeBitUInt(binaryData, cursor, 8));
            cursor += 8;
            LOG_MESSAGE(debug) << "ShipType = " << data.shipType;

            data.dimensionToBow = impl::decodeBitUInt(binaryData, cursor, 9);
            cursor += 9;
            LOG_MESSAGE(debug) << "DimensionToBow = " << data.dimensionToBow;

            data.dimensionToStern = impl::decodeBitUInt(binaryData, cursor, 9);
            cursor += 9;
            LOG_MESSAGE(debug) << "DimensionToStern = " << data.dimensionToStern;

            data.dimensionToPort = impl::decodeBitUInt(binaryData, cursor, 6);
            cursor += 6;
            LOG_MESSAGE(debug) << "DimensionToPort = " << data.dimensionToPort;

            data.dimensionToStarboard = impl::decodeBitUInt(binaryData, cursor, 6);
            cursor += 6;
            LOG_MESSAGE(debug) << "DimensionToPort = " << data.dimensionToPort;

            data.epfd = static_cast<Nmea_EPFDFix>(impl::decodeBitUInt(binaryData, cursor, 4));
            cursor += 4;
            LOG_MESSAGE(debug) << "EPFD = " << data.epfd;

            data.month = impl::decodeBitUInt(binaryData, cursor, 4);
            cursor += 4;
            LOG_MESSAGE(debug) << "Month = " << data.month;

            data.day = impl::decodeBitUInt(binaryData, cursor, 5);
            cursor += 5;
            LOG_MESSAGE(debug) << "Day = " << data.day;

            data.hour = impl::decodeBitUInt(binaryData, cursor, 5);
            cursor += 5;
            LOG_MESSAGE(debug) << "Hour = " << data.hour;

            data.minute = impl::decodeBitUInt(binaryData, cursor, 6);
            cursor += 6;
            LOG_MESSAGE(debug) << "Minute = " << data.minute;

            data.draught = impl::decodeBitUInt(binaryData, cursor, 8);
            cursor += 8;
            LOG_MESSAGE(debug) << "Draught = " << data.draught * 0.1f;

            data.destination = impl::decodeBitString(binaryData, cursor, 120);
            cursor += 120;
            LOG_MESSAGE(debug) << "Destination = " << data.destination;
        }
	}
	return ret;
}

bool NmeaParser::parseAISStandardClassBCSPositionReport(const std::string& encodedData, AISStandardClassBCSPositionReport& data)
{
	bool ret = false;

	LOG_MESSAGE(trace) << "NmeaParser::parseAISStandardClassBCSPositionReport";
	LOG_MESSAGE(debug) << "encodedData = " << encodedData;

	const int TYPE18_TOTALBITS = 168;
	const int TYPE18_TOTALCHARS = 27;

	if (encodedData.length() >= TYPE18_TOTALCHARS)
	{

    	boost::dynamic_bitset<> binaryData(TYPE18_TOTALBITS);

        // Decodifica cadena a array de bits
        LOG_MESSAGE(debug) << "parseAISStandardClassBCSPositionReport : decode INIT";
        for (int i = 0; i < TYPE18_TOTALCHARS; ++i)
        {
            SixBit bitsetDecode = impl::decodeSixBit(encodedData.at(i));
            LOG_MESSAGE(debug) << "Char: " << encodedData.at(i) << " Bits: " << bitsetDecode;

            // Concatenate all six-bit quantities found in the payload, MSB first
            impl::concatSixBitMSBFirst(i * 6, binaryData, bitsetDecode);
        }
        LOG_MESSAGE(debug) << "parseAISStandardClassBCSPositionReport : decode END " << binaryData;

        int cursor = 0;

        Nmea_AisMessageType messageType = static_cast<Nmea_AisMessageType>(impl::decodeBitUInt(binaryData, cursor, 6));
        cursor += 6;

        if (messageType == Nmea_AisMessageType_StandardClassBCSPositionReport)
        {
        	ret = true;

        	data.repeatIndicator = impl::decodeBitUInt(binaryData, cursor, 2);
        	cursor += 2;
            LOG_MESSAGE(debug) << "RepeatIndicator = " << data.repeatIndicator;

        	data.mmsi = impl::decodeBitUInt(binaryData, cursor, 30);
        	cursor += 30;
            LOG_MESSAGE(debug) << "MMSI = " << data.mmsi;

            // Reserved
            cursor += 8;

            data.speedOverGround = impl::decodeBitUInt(binaryData, cursor, 10) * 0.1f;
            cursor += 10;
            LOG_MESSAGE(debug) << "SpeedOverGround = " << data.speedOverGround;

            if (binaryData[cursor++] == false)
            {
            	data.positionAccuracy = Nmea_PositionAccuracy_UnaugmentedGNSSFix;
            } else {
            	data.positionAccuracy = Nmea_PositionAccuracy_DGPSQualityFix;
            }
            LOG_MESSAGE(debug) << "PositionAccuracy = " << data.positionAccuracy;

            data.longitude = impl::decodeBitInt(binaryData, cursor, 28) / 600000.0f;
            cursor += 28;
            LOG_MESSAGE(debug) << "Longitude = " << data.longitude;

            data.latitude = impl::decodeBitInt(binaryData, cursor, 27) / 600000.0f;
            cursor += 27;
            LOG_MESSAGE(debug) << "Latitude = " << data.latitude;

            data.courseOverGround = impl::decodeBitUInt(binaryData, cursor, 12) * 0.1f;
            cursor += 12;
            LOG_MESSAGE(debug) << "CourseOverGround = " << data.courseOverGround;

            data.trueHeading = impl::decodeBitUInt(binaryData, cursor, 9);
            cursor += 9;
            LOG_MESSAGE(debug) << "TrueHeading = " << data.trueHeading;

            data.timestapUTCSecond = impl::decodeBitUInt(binaryData, cursor, 6);
            cursor += 6;
            LOG_MESSAGE(debug) << "TimestapUTCSecond = " << data.timestapUTCSecond;
        }
	}
	return ret;
}

bool NmeaParser::parseAISStaticDataReport(const std::string& encodedData, AISStaticDataReport& data)
{
	bool ret = false;

	LOG_MESSAGE(trace) << "NmeaParser::parseAISStaticDataReport";
	LOG_MESSAGE(debug) << "encodedData = " << encodedData;

	const int TYPE24_TOTALBITS = 168;
	const int TYPE24_TOTALCHARS = 27;

	if (encodedData.length() >= TYPE24_TOTALCHARS)
	{

    	boost::dynamic_bitset<> binaryData(TYPE24_TOTALBITS);

        // Decodifica cadena a array de bits
        LOG_MESSAGE(debug) << "parseAISStaticDataReport : decode INIT";
        for (int i = 0; i < TYPE24_TOTALCHARS; ++i)
        {
            SixBit bitsetDecode = impl::decodeSixBit(encodedData.at(i));
            LOG_MESSAGE(debug) << "Char: " << encodedData.at(i) << " Bits: " << bitsetDecode;

            // Concatenate all six-bit quantities found in the payload, MSB first
            impl::concatSixBitMSBFirst(i * 6, binaryData, bitsetDecode);
        }
        LOG_MESSAGE(debug) << "parseAISStaticDataReport : decode END " << binaryData;

        int cursor = 0;

        Nmea_AisMessageType messageType = static_cast<Nmea_AisMessageType>(impl::decodeBitUInt(binaryData, cursor, 6));
        cursor += 6;

        if (messageType == Nmea_AisMessageType_StaticDataReport)
        {
        	ret = true;

        	data.repeatIndicator = impl::decodeBitUInt(binaryData, cursor, 2);
        	cursor += 2;
            LOG_MESSAGE(debug) << "RepeatIndicator = " << data.repeatIndicator;

        	data.mmsi = impl::decodeBitUInt(binaryData, cursor, 30);
        	cursor += 30;
            LOG_MESSAGE(debug) << "MMSI = " << data.mmsi;

            data.partNumber = impl::decodeBitUInt(binaryData, cursor, 2);
            cursor += 2;
            LOG_MESSAGE(debug) << "PartNumber = " << data.partNumber;

            if (data.partNumber == 0)
            {
            	data.partA.vesselName = impl::decodeBitString(binaryData, cursor, 120);
                cursor += 120;
                LOG_MESSAGE(debug) << "VesselName = '" << data.partA.vesselName << "'";
            } else if (data.partNumber == 1) {
            	data.partB.shipType = static_cast<Nmea_ShipType>(impl::decodeBitUInt(binaryData, cursor, 8));
                cursor += 8;
                LOG_MESSAGE(debug) << "ShipType = " << data.partB.shipType;

                data.partB.vendorId = impl::decodeBitString(binaryData, cursor, 18);
                cursor += 18;
                LOG_MESSAGE(debug) << "VendorId = '" << data.partB.vendorId << "'";

                data.partB.unitModelCode = impl::decodeBitUInt(binaryData, cursor, 4);
            	cursor += 4;
                LOG_MESSAGE(debug) << "UnitModelCode = " << data.partB.unitModelCode;

                data.partB.serialNumber = impl::decodeBitUInt(binaryData, cursor, 20);
            	cursor += 20;
                LOG_MESSAGE(debug) << "SerialNumber = " << data.partB.serialNumber;

                data.partB.callsign = impl::decodeBitString(binaryData, cursor, 42);
                cursor += 42;
                LOG_MESSAGE(debug) << "CallSign = '" << data.partB.callsign << "'";

                data.partB.dimensionToBow = impl::decodeBitUInt(binaryData, cursor, 9);
                cursor += 9;
                LOG_MESSAGE(debug) << "DimensionToBow = " << data.partB.dimensionToBow;

                data.partB.dimensionToStern = impl::decodeBitUInt(binaryData, cursor, 9);
                cursor += 9;
                LOG_MESSAGE(debug) << "DimensionToStern = " << data.partB.dimensionToStern;

                data.partB.dimensionToPort = impl::decodeBitUInt(binaryData, cursor, 6);
                cursor += 6;
                LOG_MESSAGE(debug) << "DimensionToPort = " << data.partB.dimensionToPort;

                data.partB.dimensionToStarboard = impl::decodeBitUInt(binaryData, cursor, 6);
                cursor += 6;
                LOG_MESSAGE(debug) << "DimensionToPort = " << data.partB.dimensionToPort;

            	data.partB.mothershipmmsi = impl::decodeBitUInt(binaryData, cursor, 30);
            	cursor += 30;
                LOG_MESSAGE(debug) << "Mothership MMSI = " << data.partB.mothershipmmsi;
            }
        }
	}
	return ret;
}
