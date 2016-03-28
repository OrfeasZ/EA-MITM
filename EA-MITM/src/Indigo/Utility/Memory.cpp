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

#include "Memory.h"
#include "../Core/String.h"

namespace indigo {
	DWORD Memory::GetProcessBaseAddress() {
		return reinterpret_cast<DWORD>(GetCurrentProcess());
	}

	DWORD Memory::FindPattern(DWORD start_address, DWORD search_length, const char *pattern, const char *mask) {
		size_t mask_length = strlen(mask);

		for (DWORD i = 0; i < search_length - mask_length; ++i) {
			int found_bytes = 0;

			for (DWORD j = 0; j < mask_length; ++j) {
				BYTE byte_read = *reinterpret_cast<BYTE*>(start_address + i + j);

				if (byte_read != ((BYTE*)pattern)[j] && mask[j] != '?') {
					break;
				}

				++found_bytes;

				if (found_bytes == mask_length) {
					return start_address + i;
				}
			}
		}

		return NULL;
	}

	DWORD Memory::FindPattern(DWORD start_address, DWORD search_length, const char *pattern) {
		// Generate pattern
		std::vector<std::string> split_pattern = String::Split(std::string(pattern), " ");
		std::string mask;
		uint8_t *clean_pattern = new uint8_t[split_pattern.size()];
		for (size_t i = 0; i < split_pattern.size(); i++) {
			const char *byte_string = split_pattern[i].c_str();

			// Convert to number
			long byte = strtol(byte_string, nullptr, 16);
			clean_pattern[i] = static_cast<uint8_t>(byte);

			if (strcmp(byte_string, "??") == 0) {
				mask.append("?");
			} else {
				mask.append("x");
			}
		}

		DWORD result = FindPattern(start_address, search_length, reinterpret_cast<const char*>(const_cast<const uint8_t*>(clean_pattern)), mask.c_str());

		delete[] clean_pattern;

		return result;
	}

	DWORD Memory::FindPattern(const char *pattern, const char *mask) {
		return FindPattern(GetProcessBaseAddress(), GetProcessImageSize(), pattern, mask);
	}

	DWORD Memory::FindPattern(const char *pattern) {
		return FindPattern(GetProcessBaseAddress(), GetProcessImageSize(), pattern);
	}

	bool Memory::GetInstructionPointer(DWORD start_address, DWORD search_length, const char *pattern, const char *mask, void *destination, int offset) {
		*static_cast<void**>(destination) = *reinterpret_cast<DWORD**>(FindPattern(start_address, search_length, pattern, mask) + offset);
		return destination != nullptr;
	}

	bool Memory::GetInstructionPointer(const char *pattern, const char *mask, void *destination, int offset) {
		return GetInstructionPointer(GetProcessBaseAddress(), GetProcessImageSize(), pattern, mask, destination, offset);
	}

	bool Memory::GetFunctionPointer(DWORD start_address, DWORD search_length, const char *pattern, const char *mask, void *destination, int offset) {
		*static_cast<void**>(destination) = reinterpret_cast<DWORD*>(FindPattern(start_address, search_length, pattern, mask) + offset);
		return destination != nullptr;
	}

	bool Memory::GetFunctionPointer(const char *pattern, const char *mask, void *destination, int offset) {
		return GetFunctionPointer(GetProcessBaseAddress(), GetProcessImageSize(), pattern, mask, destination, offset);
	}

	void *Memory::GetThis(void *address, int offset) {
		return reinterpret_cast<void*>(*reinterpret_cast<DWORD*>(reinterpret_cast<int>(address) + offset));
	}

	IMAGE_NT_HEADERS *Memory::GetPEHeader() {
		IMAGE_DOS_HEADER *prg_dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(GetProcessBaseAddress());
		IMAGE_NT_HEADERS *prg_nt_header = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<int*>(prg_dos_header) + static_cast<int>(prg_dos_header->e_lfanew / 4));

		if (prg_nt_header->Signature != IMAGE_NT_SIGNATURE) {
			return nullptr;
		}

		return prg_nt_header;
	}

	DWORD Memory::GetProcessTextSectionStart() {
		return GetPEHeader()->OptionalHeader.BaseOfCode;
	}

	DWORD Memory::GetProcessTextSectionEnd() {
		return GetPEHeader()->OptionalHeader.SizeOfCode;
	}

	DWORD Memory::GetProcessImageSize() {
		return GetPEHeader()->OptionalHeader.SizeOfImage;
	}

	void *Memory::GetDllExport(char *export_name, char *library_name, bool system) {
		char dll_path[MAX_PATH];
		ZeroMemory(dll_path, sizeof(dll_path));

		if (system) {
			GetSystemDirectoryA(dll_path, sizeof(dll_path));
		}
		else {
			GetCurrentDirectoryA(sizeof(dll_path), dll_path);
		}
		strcat_s(dll_path, "/");
		strcat_s(dll_path, library_name);

		HMODULE library = LoadLibraryA(dll_path);
		if (!library) {
			char output_string[INDIGO_CORE_STRING_BUFFERSIZE];
			strcpy_s(output_string, String::Format("Failed to load %s! Error: %i", library_name, GetLastError()).c_str());
			MessageBoxA(nullptr, output_string, "FATAL ERROR", MB_ICONERROR);

			return nullptr;
		}

		void *address = GetProcAddress(library, export_name);
		if (address == nullptr) {
			char output_string[INDIGO_CORE_STRING_BUFFERSIZE];
			strcpy_s(output_string, String::Format("The procedure entry point %s could not be located in the dynamic link library %s.",
				export_name, library_name).c_str());
			MessageBoxA(nullptr, output_string, "FATAL ERROR", MB_ICONERROR);

			FreeLibrary(library);
			return nullptr;
		}

		return address;
	}

	void *Memory::GetDllExport(int ordinal, char *library_name, bool system) {
		char dll_path[MAX_PATH];
		ZeroMemory(dll_path, sizeof(dll_path));

		if (system) {
			GetSystemDirectoryA(dll_path, sizeof(dll_path));
		}
		else {
			GetCurrentDirectoryA(sizeof(dll_path), dll_path);
		}
		strcat_s(dll_path, "/");
		strcat_s(dll_path, library_name);

		HMODULE library = LoadLibraryA(dll_path);
		if (!library) {
			char output_string[INDIGO_CORE_STRING_BUFFERSIZE];
			strcpy_s(output_string, String::Format("Failed to load %s! Error: %i", library_name, GetLastError()).c_str());
			MessageBoxA(nullptr, output_string, "FATAL ERROR", MB_ICONERROR);

			return nullptr;
		}

		void *address = GetProcAddress(library, MAKEINTRESOURCEA(ordinal));
		if (address == nullptr) {
			char output_string[INDIGO_CORE_STRING_BUFFERSIZE];
			strcpy_s(output_string, String::Format("The procedure entry point %i could not be located in the dynamic link library %s.",
				ordinal, library_name).c_str());
			MessageBoxA(nullptr, output_string, "FATAL ERROR", MB_ICONERROR);

			FreeLibrary(library);
			return nullptr;
		}

		return address;
	}

	void Memory::Patch(void *address, void *data, int size) {
		unsigned long protect[2];
		VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &protect[0]);
		memcpy(address, data, size);
		VirtualProtect(address, size, protect[0], &protect[1]);
	}

	DWORD Memory::SetProtection(int address, int size, DWORD flag) {
		DWORD old_protect;
		VirtualProtect(reinterpret_cast<void *>(address), size, flag, &old_protect);
		return old_protect;
	}

	void Memory::Nop(int address, int size) {
		unsigned long protect[2];
		VirtualProtect(reinterpret_cast<void *>(address), size, PAGE_EXECUTE_READWRITE, &protect[0]);
		memset(reinterpret_cast<void *>(address), 0x90, size);
		VirtualProtect(reinterpret_cast<void *>(address), size, protect[0], &protect[1]);
	}

	void Memory::FillWithZeroes(int address, int size) {
		unsigned long protect[2];
		VirtualProtect(reinterpret_cast<void *>(address), size, PAGE_EXECUTE_READWRITE, &protect[0]);
		memset(reinterpret_cast<void *>(address), 0x00, size);
		VirtualProtect(reinterpret_cast<void *>(address), size, protect[0], &protect[1]);
	}

	void Memory::RedirectCall(int address, void *func) {
		int temp = 0xE8;
		Patch(reinterpret_cast<void *>(address), &temp, 1);
		temp = reinterpret_cast<int>(func) - (static_cast<int>(address) + 5);
		Patch(reinterpret_cast<void *>(static_cast<int>(address) + 1), &temp, 4);
	}

	void Memory::RedirectJump(int address, void *func) {
		int temp = 0xE9;
		Patch(reinterpret_cast<void *>(address), &temp, 1);
		temp = reinterpret_cast<int>(func) - (static_cast<int>(address) + 5);
		Patch(reinterpret_cast<void *>(static_cast<int>(address) + 1), &temp, 4);
	}

	void Memory::SetChar(int address, char value) {
		Patch(reinterpret_cast<void *>(address), &value, sizeof(char));
	}

	void Memory::SetUChar(int address, unsigned char value) {
		Patch(reinterpret_cast<void *>(address), &value, sizeof(unsigned char));
	}

	void Memory::SetShort(int address, short value) {
		Patch(reinterpret_cast<void *>(address), &value, sizeof(short));
	}

	void Memory::SetUShort(int address, unsigned short value) {
		Patch(reinterpret_cast<void *>(address), &value, sizeof(unsigned short));
	}

	void Memory::SetInt(int address, int value) {
		Patch(reinterpret_cast<void *>(address), &value, sizeof(int));
	}

	void Memory::SetUInt(int address, unsigned int value) {
		Patch(reinterpret_cast<void *>(address), &value, sizeof(unsigned int));
	}

	void Memory::SetUIntWithCheck(int address, unsigned int value, unsigned int expectedValue) {
		if (*reinterpret_cast<unsigned int *>(address) == expectedValue) {
			Patch(reinterpret_cast<void *>(address), &value, sizeof(unsigned int));
		}
	}

	void Memory::SetFloat(int address, float value) {
		Patch(reinterpret_cast<void *>(address), &value, sizeof(float));
	}

	void Memory::SetDouble(int address, double value) {
		Patch(reinterpret_cast<void *>(address), &value, sizeof(double));
	}

	void Memory::SetPointer(int address, void *value) {
		Patch(reinterpret_cast<void *>(address), &value, sizeof(void*));
	}
}