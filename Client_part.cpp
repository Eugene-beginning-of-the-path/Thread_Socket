#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h> 
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

using std::cout;
using std::endl;

int main()
{
	//Key constants
	const char SERVER_IP[] = "";					// Enter IPv4 address of Server
	const short SERVER_PORT_NUM = 0;				// Enter Listening port on Server side
	const short BUFF_SIZE = 1024;					// Maximum size of buffer for exchange info between server and client

	// Key variables for all program
	int errorStat;										// For checking errors in sockets functions

	//IP in string format to numeric format for socket functions. Data is in "ip_to_num"
	in_addr ip_to_num;
	inet_pton(AF_INET, SERVER_IP, &ip_to_num);


	// WinSock initialization
	WSADATA wsData;
	errorStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (errorStat != 0) {
		cout << "Error WinSock version initializaion #";
		cout << WSAGetLastError();
		return 1;
	}
	else
		cout << "WinSock initialization is OK" << endl;

	// Socket initialization
	SOCKET ClientSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (ClientSocket == INVALID_SOCKET) {
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ClientSocket);
		WSACleanup();
	}
	else
		cout << "Client socket initialization is OK" << endl;

	// Establishing a connection to Server
	sockaddr_in servInfo;

	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(SERVER_PORT_NUM);

	errorStat = connect(ClientSocket, (sockaddr*)&servInfo, sizeof(servInfo));

	if (errorStat != 0) {
		cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	else
		cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << endl;


	//Exchange text data between Server and Client. Disconnection if a Client send "xxx"

	std::vector <char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);							// Buffers for sending and receiving data
	short packet_size = 0;												// The size of sending / receiving packet in bytes

	while (true) {

		cout << "Your (Client) message to Server: ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);

		// Check whether client like to stop chatting 
		if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
			shutdown(ClientSocket, SD_BOTH);
			closesocket(ClientSocket);
			WSACleanup();
			return 0;
		}

		packet_size = send(ClientSocket, clientBuff.data(), clientBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

		packet_size = recv(ClientSocket, servBuff.data(), servBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			cout << "Can't receive message from Server. Error # " << WSAGetLastError() << endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		else
			cout << "Server message: " << servBuff.data() << endl;

	}

	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}