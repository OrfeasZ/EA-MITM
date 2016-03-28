/*
*
*   Title: EA-MITM
*   Authors: Eyaz Rehman [http://github.com/Imposter]
*   Date: 2/17/2016
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

#ifndef EA_MITM_H_
#define EA_MITM_H_

// Program information
#define PRG_NAME "EA-MITM"

#include "Indigo/Utility/Config.h"
#include "Indigo/Utility/Log.h"

namespace base {
	class EA_MITM {
	public:
		static indigo::Config *Config;
		static indigo::Log *Log;

		static void Initialize();
		static void Shutdown();
	};
}

#endif // EA_MITM_H_