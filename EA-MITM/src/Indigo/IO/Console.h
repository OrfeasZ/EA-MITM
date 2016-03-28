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

#ifndef INDIGO_IO_CONSOLE_H_
#define INDIGO_IO_CONSOLE_H_

#ifndef INDIGO_IO_CONSOLE_BUFFERSIZE
#define INDIGO_IO_CONSOLE_BUFFERSIZE 1024
#endif

#include <mutex>

namespace indigo {
	class Console {
	private:
		static bool is_initialized_;
		static bool is_visible_;
		static std::mutex read_mutex_;
		static std::mutex write_mutex_;

	public:

		static void Show();
		static void Hide();

		static bool IsVisible();

		static void SetTitle(std::string title);

		static void Write(std::string format, ...);
		static void WriteLine(std::string format, ...);
		static std::string ReadLine();
	};
}

#endif // INDIGO_IO_CONSOLE_H_