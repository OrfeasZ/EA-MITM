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

#ifndef INDIGO_UTILITY_HOOK_H_
#define INDIGO_UTILITY_HOOK_H_

namespace indigo {
	class Hook {
	public:
		static bool InstallHook(void *target, void *function, void *original);
		static bool InstallHook(char *pattern, char *mask, void *function, void *original, int offset = 0);
		static bool InstallHook(char *pattern, void *function, void *original, int offset = 0);
		static bool RemoveHook(void *target);
	};
}

#endif // INDIGO_UTILITY_HOOK_H_