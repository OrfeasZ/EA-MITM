/*
*
*   Title: EA-MITM
*   Authors: Eyaz Rehman [http://github.com/Imposter],
*			 Luigi Auriemma [http://aluigi.altervista.org]
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

#include "ACPDump.h"
#include <time.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace indigo {
	// ip2.h
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PSH  0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECN  0x40
#define TH_CWR  0x80

#pragma pack(1)
	struct iph {
		uint8_t ihl_ver;
		uint8_t tos;
		uint16_t tot_len;
		uint16_t id;
		uint16_t frag_off;
		uint8_t ttl;
		uint8_t protocol;
		uint16_t check;
		uint32_t saddr;
		uint32_t daddr;
	};

	struct udph {
		uint16_t source;
		uint16_t dest;
		uint16_t len;
		uint16_t check;
	};

	struct udph_pseudo {
		uint32_t saddr;
		uint32_t daddr;
		uint8_t zero;
		uint8_t protocol;
		uint16_t length;
	};

	struct tcph {
		uint16_t source;
		uint16_t dest;
		uint32_t seq;
		uint32_t ack_seq;
		uint8_t doff;
		uint8_t flags;
		uint16_t window;
		uint16_t check;
		uint16_t urg_ptr;
	};

	struct icmph {
		uint8_t icmp_type;
		uint8_t  icmp_code;
		uint16_t icmp_cksum;
		union {
			uint8_t ih_pptr;
			uint32_t ih_gwaddr;
			struct {
				uint16_t icd_id;
				uint16_t icd_seq;
			} ih_idseq;
			uint32_t ih_void;
			struct {
				uint16_t ipm_void;
				uint16_t ipm_nextmtu;
			} ih_pmtu;
			struct {
				uint8_t irt_num_addrs;
				uint8_t irt_wpa;
				uint16_t irt_lifetime;
			} ih_rtradv;
		};

#define	icmp_pptr icmp_hun.ih_pptr
#define	icmp_gwaddr icmp_hun.ih_gwaddr
#define	icmp_id icmp_hun.ih_idseq.icd_id
#define	icmp_seq icmp_hun.ih_idseq.icd_seq
#define	icmp_void icmp_hun.ih_void
#define	icmp_pmvoid icmp_hun.ih_pmtu.ipm_void
#define	icmp_nextmtu icmp_hun.ih_pmtu.ipm_nextmtu
#define	icmp_num_addrs icmp_hun.ih_rtradv.irt_num_addrs
#define	icmp_wpa icmp_hun.ih_rtradv.irt_wpa
#define	icmp_lifetime icmp_hun.ih_rtradv.irt_lifetime

		union {
			struct {
				uint32_t its_otime;
				uint32_t its_rtime;
				uint32_t its_ttime;
			} id_ts;
			struct {
				iph idi_ip;
			} id_ip;
			struct {
				uint32_t ira_addr;
				uint32_t ira_preference;
			} id_radv;
			uint32_t id_mask;
			uint8_t id_data[1];
		} icmp_dun;

#define	icmp_otime icmp_dun.id_ts.its_otime
#define	icmp_rtime icmp_dun.id_ts.its_rtime
#define	icmp_ttime icmp_dun.id_ts.its_ttime
#define	icmp_ip icmp_dun.id_ip.idi_ip
#define	icmp_radv icmp_dun.id_radv
#define	icmp_mask icmp_dun.id_mask
#define	icmp_data icmp_dun.id_data
	};

	struct igmph {
		uint8_t igmp_type;
		uint8_t igmp_code;
		uint16_t igmp_cksum;
		uint32_t igmp_group;
	};
#pragma pack()

	// acpdump2.h
#define ACPDUMP_VER "0.2e"

	struct timevalx {
		int32_t     tv_sec;
		int32_t     tv_usec;
	};

	uint32_t str2ip(uint8_t *data) {
		unsigned a, b, c, d;
		if (!data[0]) {
			return 0;
		}
		sscanf_s((char *)data, "%u.%u.%u.%u", &a, &b, &c, &d);
		return (a & 0xff) | ((b & 0xFF) << 8) | ((c & 0xFF) << 16) | ((d & 0xFF) << 24);
	}

	uint8_t *ip2str(uint32_t ip) {
		static char data[16];
		sprintf_s(data, "%u.%u.%u.%u", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
		return (uint8_t *)data;
	}

	uint16_t net16(uint16_t num) {
		int endian = 1; // Big endian
		if (!*(char *)&endian) {
			return num;
		}
		return (num << 8) | (num >> 8);
	}

	uint32_t net32(uint32_t num) {
		int endian = 1; // Big endian

		if (!*(char *)&endian) {
			return(num);
		}
		return ((num & 0xFF000000) >> 24) | ((num & 0x00FF0000) >> 8) | ((num & 0x0000FF00) << 8) | ((num & 0x000000FF) << 24);
	}

	uint16_t in_cksum(void *data, int len, uint32_t *ret_sum) {
		uint32_t sum = 0;
		int i = len >> 1, endian = 1; // Big endian
		uint16_t crc, *p = (uint16_t *)data;

		if (*(char *)&endian) {
			endian = 0;
		}
		if (ret_sum) {
			sum = *ret_sum;
		}
		while (i--) {
			sum += *p++;
		}
		if (len & 1) {
			sum += *p & (endian ? 0xFF00 : 0xFF);
		}
		if (ret_sum) {
			*ret_sum = sum;
		}
		crc = sum = (sum >> 16) + (sum & 0xFFFF);
		if (sum >>= 16) {
			crc += sum;
		}
		if (!endian) {
			crc = (crc >> 8) | (crc << 8);
		}
		return ~crc;
	}

	void putxx(FILE *fd, uint32_t num, int bits) {
		for (int i = 0; i < bits >> 3; i++) {
			fputc(num >> (i << 3), fd);
		}
	}

	void create_acp(FILE *fd) {
		if (!fd) {
			return;
		}

		putxx(fd, 0xA1B2C3D4, 32);
		putxx(fd, 2, 16);
		putxx(fd, 4, 16);
		putxx(fd, 0, 32);
		putxx(fd, 0, 32);
		putxx(fd, 65535, 32);
		putxx(fd, 1, 32);
		fflush(fd);
	}

	void acp_dump(FILE *fd, int type, int protocol, uint32_t src_ip, uint16_t src_port, uint32_t dst_ip, uint16_t dst_port, uint8_t *data, int len, uint32_t *seq1, uint32_t *ack1, uint32_t *seq2, uint32_t *ack2) {
		static uint32_t lame_tmp[4] = { 0, 0, 0, 0 };
		
		struct {
			struct timevalx ts;
			uint32_t        caplen;
			uint32_t        len;
		} acp_pck;
		
		char ethdata[14];
		udph_pseudo udp_ps;
		uint32_t crc;
		iph ip;
		udph udp;
		tcph tcp;
		icmph icmp;
		igmph igmp;
		int size, tpsize, close_tcp;
		uint8_t *tp;

		if (!fd) {
			return;
		}
		if (!seq1) {
			seq1 = &lame_tmp[0];
		}
		if (!ack1) {
			ack1 = &lame_tmp[1];
		}
		if (!seq2) {
			seq2 = &lame_tmp[2];
		}
		if (!ack2) {
			ack2 = &lame_tmp[3];
		}
		if (type < 0) {
			type = 1;  // WSASocket!
		}
		if (protocol < 0) {
			protocol = 6;  // WSASocket!
		}
		if (type == 4) {
			type = 2;  // SOCK_RDM
		}
		if (type == 5) {
			type = 1;  // SOCK_SEQPACKET
		}
		
		if (!type && !protocol) {
			protocol = 6;
		} else if ((type == 1) && !protocol) {
			protocol = 6;
		} else if ((type == 2) && !protocol) {
			protocol = 17;
		} else if ((type == 3) && !protocol) {
			protocol = 255;
		}

		tp = NULL;
		tpsize = 0;
		
		if (type == 3) {
			// SOCK_RAW
		} else if (protocol == 6) {
			tp = (uint8_t *)&tcp;
			tpsize = sizeof(tcph);
		} else if (protocol == 17) {
			tp = (uint8_t *)&udp;
			tpsize = sizeof(udph);
		} else if (protocol == 1) {
			tp = (uint8_t *)&icmp;
			tpsize = sizeof(icmph);
		} else if (protocol == 2) {
			tp = (uint8_t *)&igmp;
			tpsize = sizeof(igmph);
		}

		close_tcp = 0;
		if (len < 0) {
			close_tcp = 1;
			len = 0;
		}

		if (!data) {
			len = 0;
		}

		if ((type == 3) && (protocol == 255)) {
			// SOCK_RAW, IPPROTO_RAW
			size = len;
		} else {
			size = sizeof(iph) + tpsize + len;
		}

		memset(ethdata, 0, sizeof(ethdata));
		ethdata[12] = 8; // Type

		if ((sizeof(acp_pck) + sizeof(ethdata) + size) > 0xFFFF) { // Divides the packet if it's too big
			size = len; // Use size as new "len" so acp_dump can be called with the same arguments
			if ((type == 3) && (protocol == 255)) {
				len = 0xFFFF - (sizeof(acp_pck) + sizeof(ethdata));
			} else {
				len = 0xFFFF - (sizeof(acp_pck) + sizeof(ethdata) + sizeof(iph) + tpsize);
			}
			if (len < 0) {
				len = size;
			}
			while (size > 0) {
				if (size < len) {
					len = size;
				}
				acp_dump(fd, type, protocol, src_ip, src_port, dst_ip, dst_port, data, len, seq1, ack1, seq2, ack2);
				size -= len;
				data += len;
			}
			return;
		}

		// use the following if gettimeofday doesn't exist on Windows
		acp_pck.ts.tv_sec  = time(NULL);
		acp_pck.ts.tv_usec = GetTickCount();

		acp_pck.caplen = sizeof(ethdata) + size;
		acp_pck.len = sizeof(ethdata) + size;

		ip.ihl_ver = 0x45;
		ip.tos = 0;
		ip.tot_len = net16(size);
		ip.id = net16(1);
		ip.frag_off = net16(0);
		ip.ttl = 128;
		ip.protocol = protocol;
		ip.check = net16(0);
		ip.saddr = src_ip;
		ip.daddr = dst_ip;
		ip.check = net16(in_cksum((uint8_t *)&ip, sizeof(iph), NULL));

		if (!tp) {
			// SOCK_RAW
		} else if (protocol == 6) {
			tcp.source = src_port;
			tcp.dest = dst_port;
			tcp.seq = net32(*seq1);
			tcp.ack_seq = net32(*ack1);
			tcp.doff = sizeof(tcph) << 2;
			if (close_tcp) {
				tcp.flags = TH_RST | TH_FIN | TH_ACK;
			} else if (*seq1 == 1 && *ack1 == 0) {
				tcp.flags = TH_SYN;
			} else if (*seq1 == 1 && *ack1 == 2) {
				tcp.flags = TH_SYN | TH_ACK;
			} else if (*seq1 == 2 && *ack1 == 2 && !data) {
				tcp.flags = TH_ACK;
			} else {
				tcp.flags = TH_PSH | TH_ACK;
				*ack2 = *seq1;
				(*seq1) += len;
			}
			tcp.window = net16(65535);
			tcp.check = net16(0);
			tcp.urg_ptr = net16(0);

		} else if (protocol == 17) {
			udp.source = src_port;
			udp.dest = dst_port;
			udp.check = net16(0);
			udp.len = net16(sizeof(udph) + len);

			udp_ps.saddr = ip.saddr;
			udp_ps.daddr = ip.daddr;
			udp_ps.zero = 0;
			udp_ps.protocol = 17;
			udp_ps.length = udp.len;
			crc = 0;
			in_cksum(&udp_ps, sizeof(udph_pseudo), &crc);
			in_cksum(&udp, sizeof(udph), &crc);
			udp.check = net16(in_cksum(data, len, &crc));

		} else if (protocol == 1) {
			memset(&icmp, 0, sizeof(icmph));
			icmp.icmp_type = 8;
			icmp.icmp_code = 0;
			crc = 0;
			in_cksum(&icmp, sizeof(udph_pseudo), &crc);
			icmp.icmp_cksum = net16(in_cksum(data, len, &crc));

		} else if (protocol == 2) {
			igmp.igmp_type = 0x11;
			igmp.igmp_code = 0;
			igmp.igmp_cksum = net16(0);
			igmp.igmp_group = net32(0);
			crc = 0;
			in_cksum(&igmp, sizeof(udph_pseudo), &crc);
			igmp.igmp_cksum = net16(in_cksum(data, len, &crc));
		}

		fwrite(&acp_pck, sizeof(acp_pck), 1, fd);
		fwrite(ethdata, sizeof(ethdata), 1, fd);
		if (!(type == 3 && protocol == 255)) {
			fwrite(&ip, sizeof(iph), 1, fd);
		}
		if (tp) {
			fwrite(tp, tpsize, 1, fd);
		}
		fwrite(data, len, 1, fd);
		fflush(fd);
	}

	void acp_dump_handshake(FILE *fd, int type, int protocol, uint32_t src_ip, uint16_t src_port, uint32_t dst_ip, uint16_t dst_port, uint8_t *data, int len, uint32_t *seq1, uint32_t *ack1, uint32_t *seq2, uint32_t *ack2) {
		if (!fd) {
			return;
		}
		if (!seq1 || !ack1 || !seq2 || !ack2) {
			return;
		}

		*seq1 = 0;  // useless initialization
		*ack1 = 0;
		*seq2 = 0;
		*ack2 = 0;
		if (!((!type && !protocol) || (type == 1 && !protocol) || protocol == 6)) {
			return; // if it's not tcp don't make handshake
		}

		*seq1 = 1;
		*ack1 = 0;
		acp_dump(fd, type, protocol, src_ip, src_port, dst_ip, dst_port, NULL, 0, seq1, ack1, seq2, ack2);

		*ack2 = *seq1 + 1;
		*seq2 = 1;
		acp_dump(fd, type, protocol, dst_ip, dst_port, src_ip, src_port, NULL, 0, seq2, ack2, seq1, ack1);

		*ack1 = *seq2 + 1;
		(*seq1)++;
		acp_dump(fd, type, protocol, src_ip, src_port, dst_ip, dst_port, data, len, seq1, ack1, seq2, ack2);

		(*seq2)++;
	}

	// ACPDump.h
	ACPDump::ACPDump() : file_(nullptr) {
	}

	bool ACPDump::Open(std::string file_name) {
		if (is_open_) {
			return false;
		}

		if (fopen_s(&file_, file_name.c_str(), "wb") != 0 || file_ == nullptr) {
			return false;
		}

		create_acp(file_);
		is_open_ = true;

		return true;
	}

	void ACPDump::Close() {
		fclose(file_);
		is_open_ = false;
	}

	bool ACPDump::Write(int32_t type, int32_t protocol, uint32_t source_address, uint16_t source_port, uint32_t destination_address, uint16_t destination_port, char *buffer, size_t length) {
		if (!is_open_) {
			return false;
		}

		acp_dump(file_, type, protocol, source_address, source_port, destination_address, destination_port, reinterpret_cast<uint8_t *>(buffer), length, nullptr, nullptr, nullptr, nullptr);
	
		return true;
	}
}