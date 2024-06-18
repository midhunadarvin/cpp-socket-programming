#pragma once 
#include "socket/Socket.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <any>
#include <string>

typedef struct
{
    int client_port;       // Socket Handle which represents a Client
    SOCKET forward_port;   // In a non proxying mode, it will be -1
    char remote_addr[32];  //  Address of incoming endpoint
    void *ptr_to_instance; //  Pointer to the instance to which this ClientDATA belongs
    char node_info[255];   //   Node Information
    int mode;              //   R/W mode - Read Or Write
    char ProtocolName[255];
    Socket * client_socket;
} CLIENT_DATA;

typedef struct
{
    char node_info[255];   // Encoded Current Node Information as String
    int mode;              // R/W
    void * ptr_to_instance; // Pointer to CServerSocket class
} NODE_INFO;

const std::string CRLF = "\r\n";

typedef struct std::unordered_map<std::string, std::any> EXECUTION_CONTEXT;

typedef struct
{
    std::string ipaddress; 			// ip address of the Remote Endpoint
    int port;              			// target port to which proxy connects
    int r_w;               			//  Read Endpoint or Write EndPoint
    std::string alias;     			//  unused
    float reserved;        			//  unused
    char Buffer[255];      			// unused
    int weight;            			// To load balance connections based on weight
	std::string source_hostname;    // To redirect the requests based on source hostname match
} RESOLVED_SERVICE;

typedef struct
{
    std::string name; // name of the proxy server
    int proxyPort;    // port at which the proxy listens
    std::vector<RESOLVED_SERVICE> services;
} TARGET_ENDPOINT_CONFIG;

//====================================
// The following DTO represents an EndPoint
//             - Needs tidying up
typedef struct
{
    std::string ipaddress; // ip address of the Remote Endpoint
    int port;         //  port at which Remote Listens
    int r_w;          //  Read Endpoint or Write EndPoint
    std::string alias;     //  unused
    float reserved;   //  unused
    char Buffer[255]; // unused
} END_POINT;
