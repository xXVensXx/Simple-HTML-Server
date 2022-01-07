#include "Server.h"

#include <Windows.h>

int main(void) 
{
	ServerStatus status;

	status = Server::InitServer();

	if (status != ServerStatus::ServerStatusSuccess)
	{
		printf("InitServer Problem error: %#0x\n", status);
		_asm {jmp DestroyServer}
	}

	status = Server::LoadHTMLIntoMemory("page.html"); //HTML File path

	if (status != ServerStatus::ServerStatusSuccess)
	{
		printf("LoadHTMLIntoMemory Problem error: %#0x\n", status);
		_asm {jmp DestroyServer}
	}

	while (status == ServerStatus::ServerStatusSuccess)
	{
		status = Server::WaitForConnectionThenAccept();

		if (status != ServerStatus::ServerStatusSuccess) 
		{
			printf("WaitForConnectionThenAccept Problem error: %#0x\n", status);
			_asm {jmp DestroyServer}
		}

		status = Server::SendHTMLToClientSocket();

		if (status != ServerStatus::ServerStatusSuccess)
		{
			printf("SendHTMLToClientSocket Problem error: %#0x\n", status);
			_asm {jmp DestroyServer}
			break;
		}
	}

DestroyServer:
	status = Server::DestroyServer();

	uint8_t QuiteTries = 0;
	do {
		QuiteTries++;
		if (QuiteTries < 5) 
		{
			break;
		}
		status = Server::DestroyServer();
	} while (status != ServerStatus::ServerStatusSuccess);

	return 0;
}