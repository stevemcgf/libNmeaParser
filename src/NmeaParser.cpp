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

class NmeaParser::impl {
public:
	static bool tokenizeSentence(const std::string& nmea,
			std::vector<std::string>& tokens);
	static boost::posix_time::time_duration decodeTime(
			std::vector<std::string>::iterator &i);
	static double decodeLatLng(std::vector<std::string>::iterator &i);
	static boost::gregorian::date decodeDate(
			std::vector<std::string>::iterator &i);

	static double toDecimalDegree(double degrees, double minutes,
			double seconds, char hemisphere);
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
boost::posix_time::time_duration NmeaParser::impl::decodeTime(
		std::vector<std::string>::iterator &i) {
	boost::smatch m;
	if (boost::regex_match(*i, m, hms)) {
		i++;  // consume the match
		const boost::posix_time::hours hr(boost::lexical_cast<int>(m[1].str()));
		const boost::posix_time::minutes min(
				boost::lexical_cast<int>(m[2].str()));
		const boost::posix_time::seconds sec(
				boost::lexical_cast<int>(m[3].str()));
		const boost::posix_time::milliseconds ms( (m[4].matched ? boost::lexical_cast<int>(
					boost::lexical_cast<double>(m[4].str()) * 1000) : 0.0 ));
		return boost::posix_time::time_duration(hr + min + sec + ms);
	} else
		throw std::invalid_argument(*i + " is not hms");
}

double NmeaParser::impl::toDecimalDegree(double degrees, double minutes,
		double seconds, char hemisphere) {
	degrees += minutes / 60.0 + seconds / 3600.0;
	if ((hemisphere == 'S') || (hemisphere == 'W'))
		degrees = -degrees;
	return degrees;
}

const boost::regex latlng("(\\d{2,3})(\\d{2}.\\d+)");
double NmeaParser::impl::decodeLatLng(std::vector<std::string>::iterator &i) {
	boost::smatch m;
	if (boost::regex_match(*i, m, latlng)) {
		i++; // consume the match
		double deg = boost::lexical_cast<double>(m[1].str());
		double min = boost::lexical_cast<double>(m[2].str());
		char hemisphere = (*i++).at(0);
		return toDecimalDegree(deg, min, 0.0, hemisphere);
	} else
		throw std::invalid_argument(*i + " is not lat or lng");
}

const boost::regex dmy("(\\d{2})(\\d{2})(\\d{2})");
boost::gregorian::date NmeaParser::impl::decodeDate(
		std::vector<std::string>::iterator &i) {
	boost::smatch m;
	if (boost::regex_match(*i, m, dmy)) {
		i++; // consume the match
		int day = boost::lexical_cast<int>(m[1].str());
		int month = boost::lexical_cast<int>(m[2].str());
		int year = boost::lexical_cast<int>(m[3].str()) + 2000;
		return boost::gregorian::date(year, month, day);
	} else
		throw std::invalid_argument(*i + " is not dmy");
}

void NmeaParser::parseGLL(const std::string& nmea, double& latitude,
		double& longitude, boost::posix_time::time_duration& mtime) {

	std::vector<std::string> fields;

	impl::tokenizeSentence(nmea, fields);

	std::vector<std::string>::iterator itNmea = fields.begin();

	if ((*itNmea).substr(2, 3) == "GLL") {
		itNmea++;  // consume the $GPGLL token
		latitude = impl::decodeLatLng(itNmea);
		longitude = impl::decodeLatLng(itNmea);
		mtime = impl::decodeTime(itNmea);
	} else {
		// ERROR
	}
}

void NmeaParser::parseGGA(const std::string& nmea, int& hours, int& minutes,
		double& seconds, double& latitude, double& longitude,
		Nmea_GPSQualityIndicator& quality, int& numSV, double& hdop,
		float& orthometricheight, double& geoidseparation, double& agediffgps,
		std::string& refid) {
}

void NmeaParser::parseVTG(const std::string& nmea, double& coursetrue,
		double& coursemagnetic, double& speedknots, double& speedkph) {
}

void NmeaParser::parseRMC(const std::string& nmea,
		boost::posix_time::time_duration& mtime, double& latitude,
		double& longitude, double& speedknots, double& coursetrue,
		boost::gregorian::date& mdate, double& magneticvar) {
}
