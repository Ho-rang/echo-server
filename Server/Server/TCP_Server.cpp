#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SOCKET  10 // �ִ� ���� ���� ��
#define PORT 8000 
#define BUFFER_SIZE 1024
int main()
{
	WSADATA wsaData;
	int token = WSAStartup(WINSOCK_VERSION, &wsaData);

	//���� ������ ���� �迭
	SOCKET socketArry[MAX_SOCKET] = { 0 };
	HANDLE hEventArry[MAX_SOCKET] = { 0 };

	socketArry[0] = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(PORT);

	//bind()
	if (bind(socketArry[0], (sockaddr *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		closesocket(socketArry[0]);
		return -1;
	}

	//listen()
	if (listen(socketArry[0], SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(socketArry[0]);
		return -1;
	}

	for (int i = 0; i < MAX_SOCKET; i++)
	{
		hEventArry[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (hEventArry[i] == INVALID_HANDLE_VALUE)
		{
			closesocket(socketArry[0]);
			return SOCKET_ERROR;
		}
	}

	if (WSAEventSelect(socketArry[0], hEventArry[0], FD_ACCEPT) == SOCKET_ERROR)
	{
		closesocket(socketArry[0]);
		CloseHandle(hEventArry[0]);
		return SOCKET_ERROR;
	}

	printf("���� ������\n");

	int clinet = 1;
	while (true)
	{
		//���� ���
		DWORD dObject = WaitForMultipleObjectsEx(clinet, hEventArry, FALSE, INFINITE, 0);
		if (dObject == INFINITE) continue;

		if (dObject == WAIT_OBJECT_0)
		{
			//Ŭ���̾�Ʈ ���� ����
			SOCKADDR_IN clntAddr;
			int clntLen = sizeof(clntAddr);
			SOCKET socketClient = accept(socketArry[0], (SOCKADDR*)&clntAddr, &clntLen);

			int index = -1;
			for (int c = 1; c < MAX_SOCKET; c++)
			{
				if (socketArry[c] == 0)
				{
					index = c;
					break;
				}
			}

			if (index > 0)
			{
				if (WSAEventSelect(socketClient, hEventArry[index], FD_READ | FD_CLOSE) == SOCKET_ERROR)
				{
					continue;
				}

				printf("- �����%d ���� -\n", index);

				//���� ����
				socketArry[index] = socketClient;
				clinet = max(clinet, index + 1);
			}
			else  //��� ���� �ʰ�
			{
				printf("����ο� �ʰ�\n");
				closesocket(socketClient);
			}
		}
		else
		{
			int index = (dObject - WAIT_OBJECT_0);

			DWORD dword;
			char buffer[BUFFER_SIZE] = { 0 };
			wsprintfA(buffer, "�����%d : ", index);
			WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buffer, strlen(buffer), &dword, NULL);

			// �޽��� ����
			memset(buffer, 0, sizeof(buffer));
			int cnt = recv(socketArry[index], buffer, sizeof(buffer), 0);
			if (cnt <= 0)
			{
				//���� ����
				closesocket(socketArry[index]);
				printf("�����%d ���� ����..\n", index);

				//�迭�� ���� ����
				socketArry[index] = 0;
				continue;
			}
			//�޽��� ���
			WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buffer, cnt, &dword, NULL);

			//����
			char send_buffer[BUFFER_SIZE] = { 0 };
			wsprintfA(send_buffer, "�����%d : %s", index, buffer);

			for (int c = 1; c < MAX_SOCKET; c++)
			{
				if (socketArry[c] != 0 && c != index)
				{
					//���Ź��� Ŭ���̾�Ʈ ����
					send(socketArry[c], send_buffer, strlen(send_buffer), 0);
				}
			}
		}

	}
	//���� ���� ����
	closesocket(socketArry[0]);
	CloseHandle(hEventArry[0]);
	return 0;
}