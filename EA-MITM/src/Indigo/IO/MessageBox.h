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

#ifndef INDIGO_IO_MESSAGE_BOX_H_
#define INDIGO_IO_MESSAGE_BOX_H_

#include <string>

#ifdef MessageBox
#undef MessageBox
#endif

#ifndef INDIGO_IO_MESSAGEBOX_BUFFERSIZE
#define INDIGO_IO_MESSAGEBOX_BUFFERSIZE 1024
#endif

namespace indigo {
	enum MessageBoxType {
		kMessageBoxType_Information,
		kMessageBoxType_Exclamation,
		kMessageBoxType_Error,
	};

	class MessageBox {
	public:
		static void Show(MessageBoxType type, std::string title, std::string format, ...);
	};
}

#endif INDIGO_IO_MESSAGE_BOX_H_