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

#include "Log.h"
#include "../Core/String.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace indigo {
	const char Log::day_names_[7][10] = {
		"Sunday",
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday",
	};

	const char Log::month_names_[12][10] = {
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December",
	};

	Log::Log(std::string file_name, LogType log_level, bool console) : file_name_(file_name), log_level_(log_level), console_(console) {

	}

	Log::~Log() {
		if (is_open_) {
			is_open_ = false;
			thread_.join();
			file_.close();
		}
	}

	bool Log::IsOpen() {
		return is_open_;
	}

	bool Log::Open() {
		if (is_open_) {
			return false;
		}

		file_.open(file_name_, std::ios_base::app);
		if (!file_.is_open()) {
			return false;
		}

		file_ << "\r\n--------------------------------------------------\r\n";

		time_t current_time;
		time(&current_time);
		tm local_time;
		localtime_s(&local_time, &current_time);

		file_ << String::Format("Log created on %s, %s %i, %i %.2i:%.2i:%.2i\r\n",
			day_names_[local_time.tm_wday],
			month_names_[local_time.tm_mon],
			local_time.tm_mday,
			local_time.tm_year + 1900,
			local_time.tm_hour,
			local_time.tm_min,
			local_time.tm_sec);

		file_ << "--------------------------------------------------\r\n";

		if (console_) {
			AllocConsole();
			AttachConsole(GetCurrentProcessId());

			FILE *stream;
			if (!SUCCEEDED(freopen_s(&stream, "CONIN$", "r", stdin))
				|| !SUCCEEDED(freopen_s(&stream, "CONOUT$", "w", stdout))
				|| !SUCCEEDED(freopen_s(&stream, "CONOUT$", "w", stderr))) {
				mutex_.unlock();
				return false;
			}
		}

		is_open_ = true;

		thread_ = std::thread([=]() {
			while (IsOpen()) {
				mutex_.lock();

				if (messages_.empty()) {
					mutex_.unlock();
					Sleep(1);

					continue;
				}

				std::string string = messages_.front();
				messages_.pop_front();

				mutex_.unlock();

				file_ << string;
				file_.flush();

				OutputDebugStringA(string.c_str());
				if (console_) {
					printf("%s", string.c_str());
				}

				Sleep(1);
			}
		});

		file_.flush();

		return true;
	}

	void Log::Close() {
		if (!is_open_) {
			return;
		}

		is_open_ = false;
		thread_.join();

		file_.close();
	}

	void Log::Write(LogType log_type, std::string class_name, std::string format, ...) {
		if (!is_open_) {
			return;
		}

		if (log_type > log_level_) {
			return;
		}

		mutex_.lock();

		time_t current_time;
		time(&current_time);
		tm local_time;
		localtime_s(&local_time, &current_time);

		char* log_type_string = nullptr;
		switch (log_type) {
		case kLogType_Info:
			log_type_string = "INFO";
			break;
		case kLogType_Error:
			log_type_string = "ERROR";
			break;
		case kLogType_Warning:
			log_type_string = "WARNING";
			break;
		case kLogType_Debug:
			log_type_string = "DEBUG";
			break;
		}

		va_list arguments;
		va_start(arguments, format);

		char formatted_arguments[INDIGO_UTILITY_LOG_MESSAGELENGTH];
		vsnprintf_s(formatted_arguments, INDIGO_UTILITY_LOG_MESSAGELENGTH, format.c_str(), arguments);

		va_end(arguments);

		char message[INDIGO_UTILITY_LOG_MESSAGELENGTH];
		sprintf_s(message, "%.2d:%.2d:%.2d %s %s: %s\r\n",
			local_time.tm_hour,
			local_time.tm_min,
			local_time.tm_sec,
			log_type_string,
			class_name.c_str(),
			formatted_arguments);

		messages_.push_back(message);

		mutex_.unlock();
	}
}