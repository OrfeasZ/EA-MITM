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

#include "String.h"
#include <algorithm>
#include <stdarg.h>
#include <ctype.h>

namespace indigo {
	bool String::Contains(std::string &target, std::string contains) {
		return strstr(target.c_str(), contains.c_str()) != nullptr;
	}

	std::string String::Format(std::string format, ...) {
		std::string output;
		char temp_string[INDIGO_CORE_STRING_BUFFERSIZE];

		va_list argument_list;
		va_start(argument_list, format);
		vsnprintf_s(temp_string, sizeof(temp_string), format.c_str(), argument_list);
		va_end(argument_list);

		output.append(temp_string);

		return output;
	}

	std::string String::PadLeft(std::string &target, char character, int32_t count) {
		std::string output;

		int32_t length_to_add = count - target.size();
		output.append(length_to_add, character);
		output.append(target);

		return output;
	}

	std::string String::PadRight(std::string &target, char character, int32_t count) {
		std::string output = target;

		int32_t length_to_add = count - target.size();
		output.append(length_to_add, character);

		return output;
	}

	std::string String::Replace(std::string &source, std::string from, std::string to, bool ignore_case) {
		std::string output = source;

		for (size_t x = 0; x < output.size(); x++) {
			bool same = true;
			for (size_t y = 0; y < from.size(); y++) {
				char s = output[x + y];
				char f = from[y];

				if (ignore_case) {
					s = tolower(s);
					f = tolower(f);
				}

				if (s != f) {
					same = false;
					break;
				}
			}

			if (same) {
				output.replace(x, from.size(), to);
			}
		}

		return output;
	}

	std::vector<std::string> String::Split(std::string &source, std::string split) {
		std::string input = source;
		std::vector<std::string> output;

		int i = 0;
		while ((i = input.find(split)) != std::string::npos) {
			output.push_back(input.substr(0, i));
			input.erase(0, i + split.size());
		}

		output.push_back(input);

		return output;
	}

	std::wstring String::ToWideString(std::string &string) {
		std::wstring w_string;
		w_string.resize(string.size() + 1);

		size_t num_converted = 0;
		mbstowcs_s(&num_converted, const_cast<wchar_t*>(w_string.c_str()), string.size() + 1, string.c_str(), _TRUNCATE);

		return w_string;
	}

	std::string String::ToString(std::wstring &string) {
		std::string c_string;
		c_string.resize(string.size());

		size_t num_converted = 0;
		wcstombs_s(&num_converted, const_cast<char*>(c_string.c_str()), string.size() + 1, string.c_str(), _TRUNCATE);

		return c_string;
	}
}