#include "Server.h"

ServerStatus Server::InitServer(void)
{
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
	{
		return ServerStatus::ServerSetupFail;
	}
	printf("[+] wsaStartup successful!\n");

	SOCKADDR_IN hints;
	hints.sin_addr.S_un.S_addr = inet_addr(ServerIp.c_str());
	hints.sin_port = htons(80);
	hints.sin_family = AF_INET;

	Sockets::listening = socket(AF_INET, SOCK_STREAM, 0);

	if (bind(Sockets::listening, (sockaddr*)&hints, sizeof(hints)) != 0)
	{
		return ServerStatus::BindSocketFailure;
	}

	if (listen(Sockets::listening, SOMAXCONN) != 0)
	{
		return ServerStatus::ListeningSocketFailure;
	}

	if (Sockets::listening == INVALID_SOCKET)
	{
		printf("[-] Failed to create listening socket\n");
		return ServerStatus::ListeningSocketFailure;
	}

	printf("[+] Listening socket succesfully created!\n\n");

	return ServerStatus::ServerStatusSuccess;
}

ServerStatus Server::WaitForConnectionThenAccept(const bool Log)
{
	SOCKADDR_IN client;
	int clientSize = sizeof(client);
	Sockets::client = accept(Sockets::listening, (sockaddr*)&client, &clientSize);

	if (Sockets::client == INVALID_SOCKET) 
	{
		return ServerStatus::ClientSocketError;
	}

	ConnectionsRequested++;

	static char AddrBuf[128];
	ZeroMemory(AddrBuf, sizeof(AddrBuf));

	if (inet_ntop(AF_INET, &client.sin_addr, AddrBuf, sizeof(AddrBuf)) == nullptr) 
	{
		return ServerStatus::FailToRecvFromClient;
	}

	if (Log) 
	{
		printf("Connection from %s\n", AddrBuf);
	}

	return ServerStatus::ServerStatusSuccess;
}

ServerStatus Server::LoadHTMLIntoMemory(const char* FilePath)
{

	HANDLE hFile = CreateFileA(FilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		return ServerStatus::HTMLFileError;
	}

	htmlInfo.second = GetFileSize(hFile, 0);
	if (htmlInfo.second <= 0)
	{
		CloseHandle(hFile);
		return ServerStatus::HTMLFileError;
	}

	htmlInfo.first = VirtualAlloc(0, htmlInfo.second, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (htmlInfo.first == nullptr)
	{
		CloseHandle(hFile);
		return ServerStatus::HTMLFileError;
	}
	
	if (ReadFile(hFile, htmlInfo.first, htmlInfo.second, 0, 0) != TRUE)
	{
		CloseHandle(hFile);
		return ServerStatus::HTMLFileError;
	}

	CloseHandle(hFile);

	return ServerStatus::ServerStatusSuccess;
}

ServerStatus Server::SendHTMLToClientSocket(void)
{
	const char htmlOk[] = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\n\n";

	if (send(Server::Sockets::client, htmlOk, sizeof(htmlOk), 0) == 0) 
	{
		return ServerStatus::FailedToSendToClient;
	}

	if (send(Server::Sockets::client, (char*)htmlInfo.first, htmlInfo.second, 0) == 0)
	{
		return ServerStatus::FailedToSendToClient;
	}

	return ServerStatus::ServerStatusSuccess;
}

ServerStatus Server::DestroyServer(void)
{
	closesocket(Server::Sockets::client);
	closesocket(Server::Sockets::listening);

	if (WSACleanup() != 0) 
	{
		return ServerStatus::ServerUnloadFail;
	}

	return ServerStatus::ServerStatusSuccess;
}
