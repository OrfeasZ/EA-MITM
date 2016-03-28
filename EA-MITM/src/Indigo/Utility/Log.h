/*
*
*   Title: EA-MITM
*   Authors: Eyaz Rehman [http://github.com/Imposter]
*   Date: 1/24/2016
*
*   Copyright (C) 2016 Eyaz Rehman. All Rights Reserved.
*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU General Public License
*   as published by the Free Software Foundation; either version 2
*   of the License, or any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
*   02110-1301, USA.
*
*/

#ifndef INDIGO_UTILITY_LOG_H_
#define INDIGO_UTILITY_LOG_H_

#include <mutex>
#include <deque>
#include <thread>
#include <fstream>

#ifndef INDIGO_UTILITY_LOG_MESSAGELENGTH
#define INDIGO_UTILITY_LOG_MESSAGELENGTH 1024
#endif

namespace indigo {
	enum LogType {
		kLogType_Info,
		kLogType_Warning,
		kLogType_Error,
		kLogType_Debug,
	};

	class Log {
		std::string file_name_;
		bool is_open_;
		LogType log_level_;

		std::ofstream file_;
		bool console_;
		std::mutex mutex_;
		std::thread thread_;

		std::deque<std::string> messages_;

		static const char day_names_[7][10];
		static const char month_names_[12][10];

	public:
		Log(std::string file_name, LogType log_level, bool console = false);
		~Log();

		bool IsOpen();

		bool Open();
		void Close();

		void Write(LogType log_type, std::string class_name, std::string format, ...);
	};
}

#endif // INDIGO_UTILITY_LOG_H_