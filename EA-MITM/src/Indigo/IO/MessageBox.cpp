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

#include "MessageBox.h"
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef MessageBox
#undef MessageBox
#endif

namespace indigo {
	void MessageBox::Show(MessageBoxType type, std::string title, std::string format, ...) {
		UINT message_type = MB_OK;
		switch (type) {
		case kMessageBoxType_Information:
			message_type |= MB_ICONINFORMATION;
			break;
		case kMessageBoxType_Exclamation:
			message_type |= MB_ICONEXCLAMATION;
			break;
		case kMessageBoxType_Error:
			message_type |= MB_ICONERROR;
			break;
		}

		char temp_string[INDIGO_IO_MESSAGEBOX_BUFFERSIZE];

		va_list argument_list;
		va_start(argument_list, format);

		vsnprintf_s(temp_string, sizeof(temp_string), format.c_str(), argument_list);

		va_end(argument_list);

		MessageBoxA(nullptr, temp_string, title.c_str(), message_type);
	}
}