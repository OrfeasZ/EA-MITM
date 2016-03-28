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

#include "ProtoSSL.h"
#include "../EA-MITM.h"
#include "../Indigo/IO/CommandLine.h"
#include "../Indigo/Utility/Hook.h"
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace ea {
	ProtoSSL::tConnect ProtoSSL::pConnect;
	ProtoSSL::tSend ProtoSSL::pSend;
	ProtoSSL::tRecv ProtoSSL::pRecv;

	WSADATA ProtoSSL::wsa_data_;
	indigo::ACPDump ProtoSSL::dump_;
	std::map<void *, ProtoSSL::Connection> ProtoSSL::connections_;
	std::mutex ProtoSSL::connections_mutex_;

	bool ProtoSSL::Initialize() {
		if (WSAStartup(MAKEWORD(2, 2), &wsa_data_) != 0) {
			return false;
		}

		std::string file_name = indigo::String::Format(EA_PROTO_SSL_FILE_NAME "_%i.acp", time(nullptr));
		if (!dump_.Open(file_name)) {
			base::EA_MITM::Log->Write(indigo::kLogType_Error, "EA::ProtoSSL", "Failed to open %s", file_name.c_str());
			return false;
		}

		void *protossl_connect = (void *)base::EA_MITM::Config->GetInteger("ProtoSSL", "Connect");
		void *protossl_send = (void *)base::EA_MITM::Config->GetInteger("ProtoSSL", "Send");
		void *protossl_recv = (void *)base::EA_MITM::Config->GetInteger("ProtoSSL", "Recv");

		if (protossl_connect == nullptr || !indigo::Hook::InstallHook(protossl_connect, &mConnect, &pConnect)) {
			base::EA_MITM::Log->Write(indigo::kLogType_Error, "EA::ProtoSSL", "Failed to hook ProtoSSLConnect");
			return false;
		}
		if (protossl_send == nullptr || !indigo::Hook::InstallHook(protossl_send, &mSend, &pSend)) {
			base::EA_MITM::Log->Write(indigo::kLogType_Error, "EA::ProtoSSL", "Failed to hook ProtoSSLSend");
			return false;
		}
		if (protossl_recv == nullptr || !indigo::Hook::InstallHook(protossl_recv, &mRecv, &pRecv)) {
			base::EA_MITM::Log->Write(indigo::kLogType_Error, "EA::ProtoSSL", "Failed to hook ProtoSSLRecv");
			return false;
		}

		base::EA_MITM::Log->Write(indigo::kLogType_Info, "EA::ProtoSSL", "Successfully initialized");
		base::EA_MITM::Log->Write(indigo::kLogType_Info, "EA::ProtoSSL", "ProtoSSL initialized: ");
		base::EA_MITM::Log->Write(indigo::kLogType_Info, "EA::ProtoSSL", "\tConnect: 0x%08x", protossl_connect);
		base::EA_MITM::Log->Write(indigo::kLogType_Info, "EA::ProtoSSL", "\tSend: 0x%08x", protossl_send);
		base::EA_MITM::Log->Write(indigo::kLogType_Info, "EA::ProtoSSL", "\tRecv: 0x%08x", protossl_recv);
		base::EA_MITM::Log->Write(indigo::kLogType_Info, "EA::ProtoSSL", "Dump file: %s", file_name.c_str());

		return true;
	}

	bool ProtoSSL::Shutdown() {
		dump_.Close();

		if (WSACleanup() != 0) {
			return false;
		}

		indigo::Hook::RemoveHook(mConnect);
		indigo::Hook::RemoveHook(mSend);
		indigo::Hook::RemoveHook(mRecv);

		return true;
	}

	int __cdecl ProtoSSL::mConnect(void *state, int32_t secure, char *address_string, uint32_t address, uint16_t port) {
		ProtoSSL::Connection connection;
		connection.State = state;
		connection.LocalEndpoint.AddressString = "127.0.0.1";
		connection.LocalEndpoint.Address = inet_addr("127.0.0.1");
		connection.LocalEndpoint.Port = htons((uint16_t)state);

		// Get remote address info
		addrinfo *result = nullptr;
		addrinfo hints;
		memset(&hints, 0, sizeof(hints));

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		int return_value = getaddrinfo(address_string, std::to_string(port).c_str(), &hints, &result);
		if (return_value == 0) {
			return_value = SOCKET_ERROR;
			for (addrinfo *ptr = result; ptr != NULL; ptr = ptr->ai_next) {
				if (ptr->ai_family == AF_INET) {
					sockaddr_in *endpoint_info = (sockaddr_in *)ptr->ai_addr;
					connection.RemoteEndpoint.AddressString = inet_ntoa(endpoint_info->sin_addr);
					connection.RemoteEndpoint.Address = endpoint_info->sin_addr.S_un.S_addr;
					connection.RemoteEndpoint.Port = htons(port);

					connections_mutex_.lock();
					connection.ID = connections_.size();
					connections_[state] = connection;
					connections_mutex_.unlock();

					return_value = S_OK;
					break;
				}
			}
		} else if (strcmp(address_string, "fesl.ea.com") == 0 && return_value == HOST_NOT_FOUND) {
			// FESL patch
			connection.RemoteEndpoint.AddressString = address_string;
			connection.RemoteEndpoint.Address = htonl(address);
			connection.RemoteEndpoint.Port = htons(port);

			connections_mutex_.lock();
			connection.ID = connections_.size();
			connections_[state] = connection;
			connections_mutex_.unlock();

			return_value = S_OK;
		}
		
		if (return_value != 0) {
			base::EA_MITM::Log->Write(indigo::kLogType_Error, "EA::ProtoSSL", 
				"Connect: [%i] Failed to get address information for %s:%i. This connection will not be monitored.", 
				connection.ID, address_string, port);
		} else {
			base::EA_MITM::Log->Write(indigo::kLogType_Info, "EA::ProtoSSL", 
				"Connect: [%i] Connecting to %s:%i... (Host Address: %s, SSL Enabled: %s)", connection.ID, address_string, 
				port, connection.RemoteEndpoint.AddressString.c_str(), secure ? "True" : "False");
		}

		return pConnect(state, secure, address_string, address, port);
	}

	int __cdecl ProtoSSL::mSend(void *state, const char *buffer, unsigned int length) {
		if (length > 0) {
			connections_mutex_.lock();
			if (connections_.find(state) != connections_.end()) {
				ProtoSSL::Connection &connection = connections_[state];
				dump_.Write(SOCK_STREAM, IPPROTO_TCP, connection.LocalEndpoint.Address, connection.LocalEndpoint.Port,
					connection.RemoteEndpoint.Address, connection.RemoteEndpoint.Port, const_cast<char *>(buffer), length);

				base::EA_MITM::Log->Write(indigo::kLogType_Info, "EA::ProtoSSL", "Send: [%i] Sent %i bytes to %s:%i", 
					connection.ID, length, connection.RemoteEndpoint.AddressString.c_str(), connection.RemoteEndpoint.Port);
			}
			connections_mutex_.unlock();			
		}

		return pSend(state, buffer, length);
	}

	int __cdecl ProtoSSL::mRecv(void *state, const char *buffer, unsigned int length) {
		int read_count = pRecv(state, buffer, length);

		if (read_count > 0) {
			connections_mutex_.lock();
			if (connections_.find(state) != connections_.end()) {
				ProtoSSL::Connection &connection = connections_[state];
				dump_.Write(SOCK_STREAM, IPPROTO_TCP, connection.RemoteEndpoint.Address, connection.RemoteEndpoint.Port,
					connection.LocalEndpoint.Address, connection.LocalEndpoint.Port, const_cast<char *>(buffer), length);

				base::EA_MITM::Log->Write(indigo::kLogType_Info, "EA::ProtoSSL", 
					"Recv: [%i] Received %i bytes from %s:%i", connection.ID, length, 
					connection.RemoteEndpoint.AddressString.c_str(), connection.RemoteEndpoint.Port);
			}
			connections_mutex_.unlock();
		}

		return read_count;
	}
}