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

#include "CommandLine.h"
#include "../Core/String.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlwapi.h>

namespace indigo {
	bool CommandLine::initialized_;
	std::string CommandLine::command_line_;
	std::mutex CommandLine::parse_mutex_;
	std::map<std::string, std::string> CommandLine::values_;

	bool CommandLine::parseCommandLine() {
		if (initialized_) {
			return false;
		}

		char current_path[MAX_PATH];
		if (!GetCurrentDirectoryA(sizeof(current_path), current_path)) {
			return false;
		}

		parse_mutex_.lock();

		command_line_ = GetCommandLineA();
		command_line_ = String::Replace(command_line_, current_path, "");

		for (size_t i = 0; i < command_line_.size(); i++) {
			if (command_line_[i] == '-') {
				std::string key;
				std::string value;

				bool flag_only = false;
				int32_t key_end = ++i;
				for (size_t j = i; j < command_line_.size(); j++) {
					if (command_line_[j] == '=' || command_line_[j] == ' ') {
						if (command_line_[j] == ' ') {
							flag_only = true;
						}

						key_end = j;
						break;
					}

					key.push_back(command_line_[j]);
				}

				if (!flag_only) {
					auto is_string = false;
					for (size_t j = ++key_end; j < command_line_.size(); j++) {
						if (!is_string && command_line_[j] == '"') {
							is_string = true;
							continue;
						}

						if ((!is_string && command_line_[j] == ' ') || command_line_[j] == '"') {
							key_end = j;
							break;
						}

						value.push_back(command_line_[j]);
					}
				}

				values_.insert(std::make_pair(key, value));

				i = key_end;
			}
		}

		initialized_ = true;

		parse_mutex_.unlock();

		return true;
	}

	bool CommandLine::FlagExists(std::string key_name) {
		if (!initialized_ && !parseCommandLine()) {
			return false;
		}

		return values_.find(key_name) != values_.end();
	}

	int32_t CommandLine::GetInt32(std::string key_name, int32_t default_value) {
		if (!initialized_ && !parseCommandLine()) {
			return default_value;
		}

		auto value = values_.find(key_name);
		if (value == values_.end()) {
			return default_value;
		}

		return atoi(value->second.c_str());
	}

	std::string CommandLine::GetString(std::string key_name, std::string default_value) {
		if (!initialized_ && !parseCommandLine()) {
			return default_value;
		}

		auto value = values_.find(key_name);
		if (value == values_.end()) {
			return default_value;
		}

		return value->second;
	}
}