#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 8000
#define BUFFER_SIZE 1024
#define IP "127.0.0.1"

DWORD WINAPI SendThread(LPVOID lpData);

int main()
{
	WSADATA wsaData;
	int token = WSAStartup(WINSOCK_VERSION, &wsaData);

	SOCKET socketClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN servAddr = { 0 };
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(IP);
	servAddr.sin_port = htons(PORT);

	//立加
	if (connect(socketClient, (struct sockaddr *) &servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		closesocket(socketClient);
		return SOCKET_ERROR;
	}

	CreateThread(NULL, 0, SendThread, (LPVOID)socketClient, 0, NULL);
	
	DWORD dword;
	char *text = "立加 己傍.\n";
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),
		text, strlen(text), &dword, NULL);

	while (1)
	{
		char buffer[BUFFER_SIZE] = { 0 };
		int len = recv(socketClient, buffer, sizeof(buffer), 0);
		if (len <= 0)
		{
			printf("立加 辆丰");
			break;
		}

		//皋技瘤 免仿
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buffer, strlen(buffer), &dword, NULL);
	}
	//家南 秦力
	closesocket(socketClient);
	return 0;
}

DWORD WINAPI SendThread(LPVOID lpData)
{
	SOCKET socketClient = (SOCKET)lpData;
	while (1)
	{
		DWORD dword;
		char text[BUFFER_SIZE] = { 0 };
		ReadFile(GetStdHandle(STD_INPUT_HANDLE), text, BUFFER_SIZE, &dword, NULL);
		send(socketClient, text, strlen(text), 0);
	}
	return 0;
}
