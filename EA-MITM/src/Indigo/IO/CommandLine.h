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

#ifndef INDIGO_IO_COMMAND_LINE_H_
#define INDIGO_IO_COMMAND_LINE_H_

#include <string>
#include <mutex>
#include <map>

namespace indigo {
	class CommandLine {
		static bool initialized_;
		static std::string command_line_;
		static std::mutex parse_mutex_;
		static std::map<std::string, std::string> values_;

		static bool parseCommandLine();

	public:
		static bool FlagExists(std::string key_name);
		static int32_t GetInt32(std::string key_name, int32_t default_value);
		static std::string GetString(std::string key_name, std::string default_value);
	};
}

#endif // INDIGO_IO_COMMAND_LINE_H_