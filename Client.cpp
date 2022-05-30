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
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) //открыли интерфесы для работы с сокетами windows 
	{
		cout << "Error in initialization of WinSock interface" << endl;
		return 1;
	}

	ADDRINFO hints; //выставляем параметры для поиска сокета на стороне сервера 
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	ADDRINFO* addrResult;
	//ищем на стороне сервера сокет, с нужными нам параметрами, переданными через hints
	//addrResult получит сокет(-ы), удовлетворяющие нашему запросу 
	int errorStat = getaddrinfo("localhost", "666", &hints, &addrResult); 
	if (errorStat != 0)
	{
		cout << "getaddrinfo failed with error" << endl;
		WSACleanup();
		return 1;
	}

	SOCKET toServerSocket = INVALID_SOCKET; //создаем сокет и временно его закрываем 
	toServerSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); //создаем сокет на базе найденного Сервер-сокета addrResult
	if (toServerSocket == INVALID_SOCKET)
	{
		cout << "Socket creation failled" << endl;
		WSACleanup();
		freeaddrinfo(addrResult);
		return 1;
	}

	errorStat = connect(toServerSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (errorStat == SOCKET_ERROR)
	{
		cout << "Unable connect to server";
		closesocket(toServerSocket);
		toServerSocket = INVALID_SOCKET;

		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	const char* sendBuffer = "Hello from Client";
	errorStat = send(toServerSocket, sendBuffer, (int)strlen(sendBuffer), 0); //send() возвращает кол-во переданных данных/SOCKET_ERROR

	if (errorStat == SOCKET_ERROR)
	{
		cout << "Unable send a message to Server";
		closesocket(toServerSocket);
		toServerSocket = INVALID_SOCKET;

		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	cout << "Bytes sent" << errorStat << " bytes" << endl; //выводим размер отправленного сообщения

	errorStat = shutdown(toServerSocket, SD_SEND);
	//SD_SEND - передача данных через сокет прекращена, а прием на сокет продолжает работать - отключили сокет от передачи данных
	if (errorStat == SOCKET_ERROR)
	{
		cout << "Shutdown error";
		closesocket(toServerSocket);
		toServerSocket = INVALID_SOCKET;

		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	char recvBuffer[512];

	do
	{
		ZeroMemory(recvBuffer, sizeof(recvBuffer));
		errorStat = recv(toServerSocket, recvBuffer, 512, 0);
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

	closesocket(toServerSocket);
	toServerSocket = INVALID_SOCKET;

	freeaddrinfo(addrResult);
	WSACleanup();
	return 0;

}