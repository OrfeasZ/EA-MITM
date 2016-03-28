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

#ifndef EA_PROTO_SSL_H_
#define EA_PROTO_SSL_H_

#ifndef EA_PROTO_SSL_FILE_NAME
#define EA_PROTO_SSL_FILE_NAME "protossl_dump"
#endif

#include "../Indigo/Utility/ACPDump.h"
#include <WinSock2.h>
#include <stdint.h>
#include <map>
#include <mutex>

namespace ea {
	class ProtoSSL {
		struct Connection {
			struct Endpoint {
				std::string AddressString;
				uint32_t Address;
				uint16_t Port;
			};

			uint32_t ID;
			void *State;
			Endpoint LocalEndpoint;
			Endpoint RemoteEndpoint;
		};

		typedef int(__cdecl *tConnect)(void *state, int32_t secure, char *address_string, uint32_t address, uint16_t port);
		static tConnect pConnect;
		static int __cdecl mConnect(void *state, int32_t secure, char *address_string, uint32_t address, uint16_t port);

		typedef int(__cdecl *tSend)(void *state, const char *buffer, unsigned int length);
		static tSend pSend;
		static int __cdecl mSend(void *state, const char *buffer, unsigned int length);

		typedef int(__cdecl *tRecv)(void *state, const char *buffer, unsigned int length);
		static tRecv pRecv;
		static int __cdecl mRecv(void *state, const char *buffer, unsigned int length);

		static WSADATA wsa_data_;
		static indigo::ACPDump dump_;
		static std::map<void *, Connection> connections_;
		static std::mutex connections_mutex_;

	public:
		static bool Initialize();
		static bool Shutdown();
	};
}

#endif // EA_PROTO_SSL_H_