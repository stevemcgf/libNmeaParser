/*
 * test.cpp
 *
 *  Created on: Mar 2, 2016
 *      Author: steve
 */

#define BOOST_TEST_MODULE libNmeaParser test
#include <boost/test/included/unit_test.hpp>

#include "../src/NmeaParser.h"

BOOST_AUTO_TEST_CASE( parseGLL )
{
	std::string nmea1 = "";
	double latitude;
	double longitude;
	boost::posix_time::time_duration mtime;
	BOOST_REQUIRE_NO_THROW( NmeaParser::parseGLL(nmea1, latitude, longitude, mtime));


}

BOOST_AUTO_TEST_CASE( parseGGA )
{
	// COPIAR EL EJEMPLO

}
