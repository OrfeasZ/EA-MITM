/*
*
*   Title: EA-MITM
*   Authors: Eyaz Rehman [http://github.com/Imposter]
*   Date: 2/18/2016
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

#ifndef INDIGO_UTILITY_ACP_DUMP_H_
#define INDIGO_UTILITY_ACP_DUMP_H_

#include <stdint.h>
#include <stdio.h>
#include <mutex>

namespace indigo {
	class ACPDump {
		FILE *file_;
		bool is_open_;
		std::mutex mutex_;

	public:
		ACPDump();

		bool Open(std::string file_name);
		void Close();

		bool Write(int32_t type, int32_t protocol, uint32_t source_address, uint16_t source_port, uint32_t destination_address, uint16_t destination_port, char *buffer, size_t length);
	};
}

#endif // INDIGO_UTILITY_ACP_DUMP_H_