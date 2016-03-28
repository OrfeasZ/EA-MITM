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

#ifndef INDIGO_UTILITY_CONFIG_H_
#define INDIGO_UTILITY_CONFIG_H_

#include "../Core/String.h"
#include <stdint.h>
#include <map>
#include <mutex>
#include <string>

namespace indigo {
	class Config {
		std::string buffer_;
		std::mutex mutex_;
		std::map<std::string, std::map<std::string, std::string>> data_;

	public:
		bool Open(std::string buffer);
		void Close();

		std::string GetString(std::string section, std::string key, std::string default_value = std::string());
		int64_t GetInteger(std::string section, std::string key, int64_t default_value = 0);
		float_t GetFloat(std::string section, std::string key, float_t default_value = 0);
		std::vector<std::string> GetStringList(std::string section, std::string key, std::vector<std::string> default_value = std::vector<std::string>());
		std::vector<int64_t> GetIntegerList(std::string section, std::string key, std::vector<int64_t> default_value = std::vector<int64_t>());
		std::vector<float_t> GetFloatList(std::string section, std::string key, std::vector<float_t> default_value = std::vector<float_t>());
		// TODO: Get*Map

		void SetString(std::string section, std::string key, std::string value);
		void SetInt32(std::string section, std::string key, int32_t value);
		void SetFloat(std::string section, std::string key, float_t value);
		void SetStringList(std::string section, std::string key, std::vector<std::string> value);
		void SetIntegerList(std::string section, std::string key, std::vector<int64_t> value);
		void SetFloatList(std::string section, std::string key, std::vector<float_t> value);
		void SetStringMap(std::string section, std::string key, std::map<std::string, std::string> value);
		void SetIntegerMap(std::string section, std::string key, std::map<std::string, int64_t> value);
		void SetFloatMap(std::string section, std::string key, std::map<std::string, float_t> value);
		
		std::string GetBuffer();
	};
}

#endif // INDIGO_UTILITY_CONFIG_H_