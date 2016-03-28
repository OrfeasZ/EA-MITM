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

#include "Console.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace indigo {
	bool Console::is_initialized_ = false;
	bool Console::is_visible_ = false;
	std::mutex Console::read_mutex_;
	std::mutex Console::write_mutex_;

	void Console::Show() {
		if (is_visible_) {
			return;
		}

		if (!is_initialized_) {
			// Create console
			AllocConsole();
			AttachConsole(GetCurrentProcessId());

			// Open streams
			FILE *stream;
			freopen_s(&stream, "CONIN$", "r", stdin);
			freopen_s(&stream, "CONOUT$", "w", stdout);
			freopen_s(&stream, "CONOUT$", "w", stderr);

			// Set ready
			is_initialized_ = true;
		}

		ShowWindow(GetConsoleWindow(), SW_SHOW);

		is_visible_ = true;
	}

	void Console::Hide() {
		if (!is_initialized_) {
			return;
		}

		if (is_visible_) {
			ShowWindow(GetConsoleWindow(), SW_HIDE);
			is_visible_ = false;
		}

		read_mutex_.unlock();
		write_mutex_.unlock();
	}

	bool Console::IsVisible() {
		return is_visible_;
	}

	void Console::SetTitle(std::string title) {
		SetConsoleTitleA(title.c_str());
	}

	void Console::Write(std::string format, ...) {
		if (!is_visible_) {
			return;
		}

		write_mutex_.lock();

		// Create the variable argument list
		va_list arguments;

		// Start reading arguments after format
		va_start(arguments, format);

		// Copy the message
		char write_line[INDIGO_IO_CONSOLE_BUFFERSIZE];
		vsnprintf_s(write_line, INDIGO_IO_CONSOLE_BUFFERSIZE, format.c_str(), arguments);

		// Print
		printf(write_line);

		// End the argument list
		va_end(arguments);

		write_mutex_.unlock();
	}

	void Console::WriteLine(std::string format, ...) {
		if (!is_visible_) {
			return;
		}

		write_mutex_.lock();

		// Create the variable argument list
		va_list arguments;

		// Start reading arguments after format
		va_start(arguments, format);

		// Copy the message
		char write_line[INDIGO_IO_CONSOLE_BUFFERSIZE];
		vsnprintf_s(write_line, INDIGO_IO_CONSOLE_BUFFERSIZE, format.c_str(), arguments);

		// Print
		printf("%s\n", write_line);

		// End the argument list
		va_end(arguments);

		write_mutex_.unlock();
	}

	std::string Console::ReadLine() {
		read_mutex_.lock();

		// Create read buffer
		std::string output;
		static char read_line[INDIGO_IO_CONSOLE_BUFFERSIZE];

		// Clear the buffer
		ZeroMemory(read_line, INDIGO_IO_CONSOLE_BUFFERSIZE);

		// Read characters to fill the buffer
		for (int i = 0; i < INDIGO_IO_CONSOLE_BUFFERSIZE; i++) {
			// Get the character
			char c = getchar();

			// Check if the character has a value
			if (c != EOF) {
				// Check if ENTER was pressed
				if (c == '\n') {
					break;
				}

				// Set the character in the buffer position
				read_line[i] = c;
			}
		}

		read_mutex_.unlock();

		output.append(read_line);

		return output;
	}
}