/***********************************************************************
*
* Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
* See LICENSE file in the project root for full license information.
*
************************************************************************/

/**
 * \file GtiLog.h
 * \brief GTI header file containing definitions for run-time debug logging.
 */

#if !defined(_GTILOG_H_)
#define _GTILOG_H_

#include <iostream>
#include <memory>
#include <string>


#define LOG(level)  GtiLog().GetStream(GTI_LOG_##level)
#define LOGLEVEL(level)  GtiLog::setLevel(GTI_LOG_##level)
#define LOGLEVEL_ABOVE(level)  ((int)(GtiLog::getLevel())>(int)(GTI_LOG_##level))
#define LOGLEVEL_IS(level)  (GtiLog::getLevel()==GTI_LOG_##level)

enum GTI_LOG_LEVEL {
	GTI_LOG_ERROR,
	GTI_LOG_WARNING,
	GTI_LOG_APP,
	GTI_LOG_NOTICE,
	GTI_LOG_INFO,
	GTI_LOG_DEBUG,
	GTI_LOG_TRACE,
	GTI_LOG_ALL,
	GTI_LOG_BUTT
};

class GtiLog
{
public:

	GtiLog();
	std::ostream& GetStream(GTI_LOG_LEVEL level = GTI_LOG_INFO);
	void SetStream(std::ostream *newOs) { os = std::shared_ptr<std::ostream>(newOs); }
	~GtiLog();
	static void setLevel(GTI_LOG_LEVEL newLevel) { mDefaultLevel = newLevel; }
	static GTI_LOG_LEVEL getLevel() { return mDefaultLevel; }
	static void setLogFile(char * filename) { logFilename = filename; }
private:
	std::shared_ptr<std::ostream> os;
	GTI_LOG_LEVEL mLevel;
	static GTI_LOG_LEVEL mDefaultLevel;
	static std::string logFilename;
};

#endif
