#ifndef PROTOCOL_HELPER_DOT_H
#define PROTOCOL_HELPER_DOT_H
// UNIX/Linux-specific definitions
#define SOCKET int

#include "./endian_utils.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <sys/time.h>
#include <cstdint>

#ifdef WINDOWS_OS
#include <windows.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>

#define DWORD unsigned long
#endif

#define SOCKET_ERROR (-1)

static uint32_t readInt32(std::string buffer, int start, int length = 4) {
	const std::string packet_length_nw_bytes = buffer.substr(start, length);
	uint32_t packet_length_nw = 0;
	memcpy(&packet_length_nw, &packet_length_nw_bytes, length);
	const uint32_t packet_length = le32toh(packet_length_nw);
	return packet_length;
}

static std::string readCString(std::string buffer, int start) {
	std::string name;
	int i = start;
	while (buffer[i] != '\0') {
		name += buffer[i];
		i++;
	}
	return name;
}

static uint8_t readByte(std::string buffer, int start) { return buffer[start]; };

static uint16_t readLEInt16(std::string buffer, int start) {
	const int length = 2;
	const std::string packet_length_nw_bytes = buffer.substr(start, length);
	uint32_t packet_length_nw = 0;
	memcpy(&packet_length_nw, &packet_length_nw_bytes, length);
	const uint32_t packet_length = le16toh(packet_length_nw);
	return packet_length;
}

static uint32_t readLEInt24(std::string buffer, int start) {
	const int length = 3;
	std::string packet_length_nw_bytes = buffer.substr(start, length);
	// if size if 3, make it 4 bytes by adding a null byte at the end
	packet_length_nw_bytes += char(0x00);
	uint32_t packet_length_nw = 0;
	memcpy(&packet_length_nw, &packet_length_nw_bytes, length + 1);
	const uint32_t packet_length = le32toh(packet_length_nw);
	return packet_length;
}

static uint64_t readLEInt64(std::string buffer, int start) {
	const int length = 8;
	const std::string packet_length_nw_bytes = buffer.substr(start, length);
	uint64_t packet_length_nw = 0;
	memcpy(&packet_length_nw, &packet_length_nw_bytes, length);
	const uint64_t packet_length = le64toh(packet_length_nw);
	return packet_length;
};

static uint16_t readBEInt16(std::string buffer, int start) {
	const int length = 2;
	const std::string packet_length_nw_bytes = buffer.substr(start, length);
	uint16_t packet_length_nw = 0;
	memcpy(&packet_length_nw, &packet_length_nw_bytes, length);
	return be16toh(packet_length_nw);
}

static uint32_t readBEInt32(std::string buffer, int start) {
	const int length = 4;
	const std::string packet_length_nw_bytes = buffer.substr(start, length);
	uint32_t packet_length_nw = 0;
	memcpy(&packet_length_nw, &packet_length_nw_bytes, length);
	return be32toh(packet_length_nw);
}

static uint64_t readBEInt64(std::string buffer, int start) {
	const int length = 8;
	const std::string packet_length_nw_bytes = buffer.substr(start, length);
	uint64_t packet_length_nw = 0;
	memcpy(&packet_length_nw, &packet_length_nw_bytes, length);
	return be64toh(packet_length_nw);
}

class ProtocolHelper {
  public:
	static std::string GetIPAddressAsString(struct sockaddr_in *client_addr);
	static std::string GetIPPortAsString(struct sockaddr_in *client_addr);
	static bool SetReadTimeOut(SOCKET s, long second);
	static bool SetKeepAlive(SOCKET s, int flags);
	static bool ReadSocketBuffer(SOCKET s, char *bfr, int size, int *num_read);
	static bool ReadSocketBuffer(SOCKET s, char *bfr, int size);
	static bool WriteSocketBuffer(SOCKET s, char *bfr, int size);
};

#endif
