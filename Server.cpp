#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using std::cout;
using std::endl;

int server()
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) //������� ��������� ��� ������ � �������� windows 
	{
		cout << "Error in initialization of WinSock interface" << endl;
		return 1;
	}

	ADDRINFO hints; //���������� ��������� ��� ������ ������ �� ������� ������� 
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	ADDRINFO* addrResult;
	//���� �� ������� ������� �����, � ������� ��� �����������, ����������� ����� hints
	//addrResult ������� �����(-�), ��������������� ������ ������� 
	int errorStat = getaddrinfo(NULL, "666", &hints, &addrResult);
	if (errorStat != 0)
	{
		cout << "getaddrinfo failed with error" << endl;
		WSACleanup();
		return 1;
	}
	
	SOCKET lisetnSocket = INVALID_SOCKET; //��������� �����, � ����� ��������� �������� �������������� ������ �� ����� �����(toClientSocket), � ��� ���������� ������� � ������ ����� ������� � �������������� �� ����� ������ (����� �� ���������)
	SOCKET toClientSocket = INVALID_SOCKET; //������� ����� � �������� ��� ��������� 
	toClientSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); //������� ����� �� ���� ���������� ������-������ addrResult
	if (toClientSocket == INVALID_SOCKET)
	{
		cout << "Socket creation failled" << endl;
		WSACleanup();
		freeaddrinfo(addrResult);
		return 1;
	}

	errorStat = bind(lisetnSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); //����������� �������������� ����� � �����������
	if (errorStat == SOCKET_ERROR)
	{
		cout << "Binding socket failed";
		closesocket(lisetnSocket);

		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	errorStat = listen(lisetnSocket, SOMAXCONN); //����������� ��������, ���� ������� �� ����������� ������
	if (errorStat == SOCKET_ERROR)
	{
		cout << "Listening socket failed";
		closesocket(lisetnSocket);

		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	toClientSocket = accept(lisetnSocket, NULL, NULL); //����� ��������� �������� lisetnSocket � ������� �������, lisetnSocket �������������� ������ �� ������ �����, � ��� �������� ����� ����� ����������. accept() ���������� ���������� ����� 
	if (toClientSocket == INVALID_SOCKET)
	{
		cout << "Accepting socket failed";
		closesocket(lisetnSocket);

		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//�.�. ������ �������� �� �� ����, ��������� ��������� ����� listenSocket
	closesocket(lisetnSocket);

	const char* sendBuffer = "Hello from Server";
	char recvBuffer[512];

	do
	{
		ZeroMemory(recvBuffer, sizeof(recvBuffer));
		errorStat = recv(toClientSocket, recvBuffer, 512, 0);//�������� ���� �� ����������� ������ 
		if (errorStat > 0)
		{
			cout << "Received " << errorStat << " bytes" << endl;
			cout << "Received data: " << recvBuffer << endl;

			errorStat = send(toClientSocket, sendBuffer, sizeof(sendBuffer), 0);
			if (errorStat == SOCKET_ERROR)
			{
				cout << "Failed to send data back";
				closesocket(toClientSocket);
				toClientSocket = INVALID_SOCKET;

				freeaddrinfo(addrResult);
				WSACleanup();
				return 1;
			}
		}
		else if (errorStat == 0)
		{
			cout << "Connection closing" << endl; //������� �� ������� ������� ���������
			closesocket(toClientSocket);
			toClientSocket = INVALID_SOCKET;

			freeaddrinfo(addrResult);
			WSACleanup();
			return 1;
		}
		else
			cout << "recv failed" << endl;
	} while (errorStat > 0);

	errorStat = shutdown(toClientSocket, SD_SEND);
	if (errorStat == SOCKET_ERROR)
	{
		cout << "Shutdown error";
		closesocket(toClientSocket);

		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	closesocket(toClientSocket);
	toClientSocket = INVALID_SOCKET;

	freeaddrinfo(addrResult);
	WSACleanup();
	return 0;

}