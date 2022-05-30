#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using std::cout;
using std::endl;

int client()
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
	int errorStat = getaddrinfo("localhost", "666", &hints, &addrResult); 
	if (errorStat != 0)
	{
		cout << "getaddrinfo failed with error" << endl;
		WSACleanup();
		return 1;
	}

	SOCKET ConnectSocket = INVALID_SOCKET; //������� ����� � �������� ��� ��������� 
	ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); //������� ����� �� ���� ���������� ������-������ addrResult
	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "Socket creation failled" << endl;
		WSACleanup();
		freeaddrinfo(addrResult);
		return 1;
	}

	errorStat = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (errorStat == SOCKET_ERROR)
	{
		cout << "Unable connect to server";
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;

		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	const char* sendBuffer = "Hello from Client";
	errorStat = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0); //send() ���������� ���-�� ���������� ������/SOCKET_ERROR

	if (errorStat == SOCKET_ERROR)
	{
		cout << "Unable send a message to Server";
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;

		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	cout << "Bytes sent" << errorStat << " bytes" << endl; //������� ������ ������������� ���������

	errorStat = shutdown(ConnectSocket, SD_SEND);
	//SD_SEND - �������� ������ ����� ����� ����������, � ����� �� ����� ���������� �������� - ��������� ����� �� �������� ������
	if (errorStat == SOCKET_ERROR)
	{
		cout << "Shutdown error";
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;

		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	char recvBuffer[512];

	do
	{
		ZeroMemory(recvBuffer, sizeof(recvBuffer));
		errorStat = recv(ConnectSocket, recvBuffer, 512, 0);
		if (errorStat > 0)
		{
			cout << "Received " << errorStat << " bytes" << endl;
			cout << "Received data: " << recvBuffer << endl;
		}
		else if (errorStat == 0)
			cout << "Connection closed" << endl;
		else
			cout << "recv failed" << endl;
	} while (errorStat > 0);

	closesocket(ConnectSocket);
	ConnectSocket = INVALID_SOCKET;

	freeaddrinfo(addrResult);
	WSACleanup();
	return 0;

}