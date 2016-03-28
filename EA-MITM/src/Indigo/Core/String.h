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

#ifndef INDIGO_CORE_STRING_H_
#define INDIGO_CORE_STRING_H_

#include <vector>
#include <string>
#include <stdint.h>

#ifndef INDIGO_CORE_STRING_BUFFERSIZE
#define INDIGO_CORE_STRING_BUFFERSIZE 1024
#endif

namespace indigo {
	class String {
	public:
		static bool Contains(std::string &target, std::string contains);
		static std::string Format(std::string format, ...);
		static std::string PadLeft(std::string &target, char character, int32_t count);
		static std::string PadRight(std::string &target, char character, int32_t count);
		static std::string Replace(std::string &source, std::string from, std::string to, bool ignore_case = false);
		static std::vector<std::string> Split(std::string &source, std::string split);

		static std::wstring ToWideString(std::string &string);
		static std::string ToString(std::wstring &string);
	};
}

#endif // INDIGO_CORE_STRING_H_