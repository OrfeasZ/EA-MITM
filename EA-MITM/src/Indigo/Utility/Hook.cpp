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

#include "Hook.h"
#include "Memory.h"
#include "MinHook.h"

namespace indigo {
	bool Hook::InstallHook(void *target, void *function, void *original) {
		MH_STATUS status = MH_CreateHook(target, function, reinterpret_cast<void **>(original));
		if (status == MH_ERROR_NOT_INITIALIZED) {
			MH_Initialize();
			MH_CreateHook(target, function, reinterpret_cast<void **>(original));
		}

		status = MH_EnableHook(target);

		return status == MH_OK;
	}

	bool Hook::InstallHook(char *pattern, char *mask, void *function, void *original, int offset) {
		DWORD address = Memory::FindPattern(Memory::GetProcessBaseAddress() + Memory::GetProcessTextSectionStart(),
			Memory::GetProcessTextSectionEnd(), pattern, mask) + offset;
		if (address == NULL) {
			return false;
		}

		return InstallHook(reinterpret_cast<void *>(address), function, original);
	}

	bool Hook::InstallHook(char *pattern, void *function, void *original, int offset) {
		DWORD address = Memory::FindPattern(Memory::GetProcessBaseAddress() + Memory::GetProcessTextSectionStart(),
			Memory::GetProcessTextSectionEnd(), pattern) + offset;

		if (address == NULL) {
			return false;
		}

		return InstallHook(reinterpret_cast<void*>(address), function, original);
	}

	bool Hook::RemoveHook(void *target) {
		void *target_function = *reinterpret_cast<void **>(target);

		if (MH_DisableHook(target_function) != MH_OK) {
			return false;
		}

		return MH_RemoveHook(target_function) == MH_OK;
	}
}