/*
 * NmeaSentences.h
 *
 *  Created on: Feb 9, 2016
 *      Author: steve
 */

#ifndef SRC_NMEASENTENCES_H_
#define SRC_NMEASENTENCES_H_

#include <string>
#include <map>

/**
 * @brief Helper class to store NMEA Talker Id to Description Map
 */
class NmeaTalkerIdMap {
public:
	/**
	 * @brief Get the map instance.
	 *
	 * @return Map instance of Talker Id to Description.
	 */
	static const std::map<std::string, std::string>& get();

private:
	/**
	 * @brief Talker Id to Description Map
	 */
	static const std::map<std::string, std::string> mapTalkerId;
};

/**
 * @brief Helper class to store NMEA Sentence to Description Map
 */
class NmeaSentenceMap {
public:
	/**
	 * @brief Get the map instance.
	 *
	 * @return Map instance of NMEA Sentence to Description.
	 */
	static const std::map<std::string, std::string>& get();

private:
	/**
	 * @brief NMEA Sentence to Description Map
	 */
	static const std::map<std::string, std::string> mapSentence;

};

/**
 * @brief Extracts TalkerId from NMEA String
 *
 * @param [in] nmea NMEA String
 *
 * @return TalkerId string
 */
std::string getNmeaTalkerId(const std::string& nmea);

/**
 * @brief Extracts NmeaSentece Id from NMEA String
 *
 * @param [in] nmea NMEA String
 *
 * @return Nmea Sentence Id string
 */
std::string getNmeaSentence(const std::string& nmea);

/**
 * @brief Nmea Talker Id description
 *
 * @param [in] nmea TalkerId String containing Talker Id
 *
 * @return Talker Id description.
 */
std::string getNmeaTalkerIdName(const std::string& nmeaTalkerId);

/**
 * @brief Nmea Sentence description
 *
 * @param [in] nmea Nmea Sentence String containing Nmea Sentence Id
 *
 * @return NMEA Sentence description.
 */
std::string getNmeaSentenceDesc(const std::string& nmeaSentence);

#endif /* SRC_NMEASENTENCES_H_ */
