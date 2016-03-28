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

#include "Config.h"

namespace indigo {
	bool Config::Open(std::string buffer) {
		buffer_ = buffer;
		
		std::vector<std::string> lines;
		if (String::Contains(buffer_, "\r\n")) {
			lines = String::Split(buffer_, "\r\n");
		} else {
			lines = String::Split(buffer_, "\n");
		}
		std::string section_name;
		std::map<std::string, std::string> section;
		for (auto &line : lines) {
			if (line.size() > 0) {
				if (line.find_first_of('[') == 0 && line.find_last_of(']') == line.size() - 1) {
					if (section_name.size() != 0) {
						data_.insert(std::make_pair(section_name, section));
						section.clear();
					}
					section_name = line.substr(1, line.size() - 2);
				} else if (line.find_first_of('=') != std::string::npos) {
					uint32_t key_end = line.find_first_of('=');
					std::string key = line.substr(0, key_end);
					std::string value = line.substr(key_end + 1);
					section.insert(std::make_pair(key, value));
				}
			}
		}
		if (!section.empty()) {
			data_.insert(std::make_pair(section_name, section));
			section.clear();
		}

		return true;
	}

	void Config::Close() {
		buffer_.clear();
		data_.clear();
	}

	std::string Config::GetString(std::string section, std::string key, std::string default_value) {
		if (data_.find(section) == data_.end() || data_[section].find(key) == data_[section].end()) {
			return default_value;
		}
		return data_[section][key];
	}

	int64_t Config::GetInteger(std::string section, std::string key, int64_t default_value) {
		if (data_.find(section) == data_.end() || data_[section].find(key) == data_[section].end()) {
			return default_value;
		}
		return std::strtoll(data_[section][key].c_str(), NULL, 0);
	}

	float_t Config::GetFloat(std::string section, std::string key, float_t default_value) {
		if (data_.find(section) == data_.end() || data_[section].find(key) == data_[section].end()) {
			return default_value;
		}
		return stof(data_[section][key]);
	}

	std::vector<std::string> Config::GetStringList(std::string section, std::string key, std::vector<std::string> default_value) {
		if (data_.find(section) == data_.end() || data_[section].find(key) == data_[section].end()) {
			return default_value;
		}
		int32_t size = std::atoi(data_[section][key].c_str());
		std::vector<std::string> result;
		for (size_t i = 0; i < size; i++) {
			result.push_back(data_[section][key + "." + std::to_string(i)]);
		}
		return result;
	}

	std::vector<int64_t> Config::GetIntegerList(std::string section, std::string key, std::vector<int64_t> default_value) {
		if (data_.find(section) == data_.end() || data_[section].find(key) == data_[section].end()) {
			return default_value;
		}
		int32_t size = std::atoi(data_[section][key].c_str());
		std::vector<int64_t> result;
		for (size_t i = 0; i < size; i++) {
			result.push_back(std::atoi(data_[section][key + "." + std::to_string(i)].c_str()));
		}
		return result;
	}

	std::vector<float_t> Config::GetFloatList(std::string section, std::string key, std::vector<float_t> default_value) {
		if (data_.find(section) == data_.end() || data_[section].find(key) == data_[section].end()) {
			return default_value;
		}
		int32_t size = std::atoi(data_[section][key].c_str());
		std::vector<float_t> result;
		for (size_t i = 0; i < size; i++) {
			result.push_back(std::stof(data_[section][key + "." + std::to_string(i)].c_str()));
		}
		return result;
	}

	// TODO: Get*Map

	void Config::SetString(std::string section, std::string key, std::string value) {
		data_[section][key] = value;
	}

	void Config::SetInt32(std::string section, std::string key, int32_t value) {
		data_[section][key] = std::to_string(value);
	}

	void Config::SetFloat(std::string section, std::string key, float_t value) {
		data_[section][key] = std::to_string(value);
	}

	void Config::SetStringList(std::string section, std::string key, std::vector<std::string> value) {
		data_[section][key] = std::to_string(value.size());
		for (size_t i = 0; i < value.size(); i++) {
			data_[section][key + "." + std::to_string(i)] = value[i];
		}
	}

	void Config::SetIntegerList(std::string section, std::string key, std::vector<int64_t> value) {
		data_[section][key] = std::to_string(value.size());
		for (size_t i = 0; i < value.size(); i++) {
			data_[section][key + "." + std::to_string(i)] = std::to_string(value[i]);
		}
	}

	void Config::SetFloatList(std::string section, std::string key, std::vector<float_t> value) {
		data_[section][key] = std::to_string(value.size());
		for (size_t i = 0; i < value.size(); i++) {
			data_[section][key + "." + std::to_string(i)] = std::to_string(value[i]);
		}
	}

	void Config::SetStringMap(std::string section, std::string key, std::map<std::string, std::string> value) {
		data_[section][key] = std::to_string(value.size());
		for (auto &pair : value) {
			data_[section][key + "." + pair.first] = pair.second;
		}
	}

	void Config::SetIntegerMap(std::string section, std::string key, std::map<std::string, int64_t> value) {
		data_[section][key] = std::to_string(value.size());
		for (auto &pair : value) {
			data_[section][key + "." + pair.first] = std::to_string(pair.second);
		}
	}

	void Config::SetFloatMap(std::string section, std::string key, std::map<std::string, float_t> value) {
		data_[section][key] = std::to_string(value.size());
		for (auto &pair : value) {
			data_[section][key + "." + pair.first] = std::to_string(pair.second);
		}
	}

	std::string Config::GetBuffer() {
		buffer_.clear();
		for (auto &section : data_) {
			buffer_.append("[" + section.first + "]\r\n");
			for (auto &data : section.second) {
				buffer_.append(data.first + "=" + data.second + "\r\n");
			}
		}

		return buffer_;
	}
}