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

class NmeaTalkerIdMap {
public:
	static const std::map<std::string, std::string>& get();

private:
	static const std::map<std::string, std::string> mapTalkerId;
};

class NmeaSentenceMap {
public:
	static const std::map<std::string, std::string>& get();

private:
	static const std::map<std::string, std::string> mapSentence;

};

std::string getNmeaTalkerId(const std::string& nmea);
std::string getNmeaSentence(const std::string& nmea);
std::string getNmeaTalkerIdName(const std::string& nmeaTalkerId);
std::string getNmeaSentenceDesc(const std::string& nmeaSentence);

#endif /* SRC_NMEASENTENCES_H_ */
