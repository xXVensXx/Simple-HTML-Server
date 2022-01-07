#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <Windows.h>
#include <iostream>

#pragma warning(disable : 4996)

typedef enum ServerStatus 
{
	ServerSetupFail,
	ServerUnloadFail,
	FailToRecvFromClient,
	FailedToSendToClient,
	BindSocketFailure,
	ClientSocketError,
	HTMLFileError,
	ListeningSocketFailure,
	ServerStatusSuccess,
};

namespace Server 
{
	static std::string ServerIp = "127.0.0.1";
	namespace Sockets 
	{
		static SOCKET listening = INVALID_SOCKET;
		static SOCKET client = INVALID_SOCKET;
	};
	ServerStatus InitServer(void);
	ServerStatus WaitForConnectionThenAccept(const bool Log = true);
	ServerStatus LoadHTMLIntoMemory(const char* FilePath);
	ServerStatus SendHTMLToClientSocket(void);
	ServerStatus DestroyServer(void);

	static std::pair<PVOID, uint32_t>htmlInfo;

	static uint32_t ConnectionsRequested =0;
};
