/*
*
*   Title: EA-MITM
*   Authors: Eyaz Rehman [http://github.com/Imposter],
*			 NoFaTe (Orfeas Zafeiris) [https://github.com/OrfeasZ],
*			 Thordin [https://github.com/Thordin],
*			 ThirteenAG [https://github.com/ThirteenAG],
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

#ifndef INDIGO_UTILITY_MEMORY_H_
#define INDIGO_UTILITY_MEMORY_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace indigo {
	class Memory {
	public:
		static DWORD GetProcessBaseAddress();
		static IMAGE_NT_HEADERS *GetPEHeader();
		static DWORD GetProcessTextSectionStart();
		static DWORD GetProcessTextSectionEnd();
		static DWORD GetProcessImageSize();
		static DWORD FindPattern(DWORD start_address, DWORD search_length, const char *pattern, const char *mask);
		static DWORD FindPattern(DWORD start_address, DWORD search_length, const char *pattern);
		static DWORD FindPattern(const char *pattern, const char *mask);
		static DWORD FindPattern(const char *pattern);

		static bool GetInstructionPointer(DWORD start_address, DWORD search_length, const char *pattern, const char *mask, void *destination, int offset = 0);
		static bool GetInstructionPointer(const char *pattern, const char *mask, void *destination, int offset = 0);

		static bool GetFunctionPointer(DWORD start_address, DWORD search_length, const char *pattern, const char *mask, void *destination, int offset = 0);
		static bool GetFunctionPointer(const char *pattern, const char *mask, void *destination, int offset = 0);

		static void *GetThis(void *address, int offset = 0);

		static void *GetDllExport(char *export_name, char *library, bool system = false);
		static void *GetDllExport(int ordinal, char *library, bool system = false);

		static void Patch(void *address, void *data, int size);
		static DWORD SetProtection(int address, int size, DWORD flag);
		static void Nop(int address, int size);
		static void FillWithZeroes(int address, int size);
		static void RedirectCall(int address, void *func);
		static void RedirectJump(int address, void *func);
		static void SetChar(int address, char value);
		static void SetUChar(int address, unsigned char value);
		static void SetShort(int address, short value);
		static void SetUShort(int address, unsigned short value);
		static void SetInt(int address, int value);
		static void SetUInt(int address, unsigned int value);
		static void SetUIntWithCheck(int address, unsigned int value, unsigned int expected_value);
		static void SetFloat(int address, float value);
		static void SetDouble(int address, double value);
		static void SetPointer(int address, void *value);
	};
}

#endif // INDIGO_UTILITY_MEMORY_H_