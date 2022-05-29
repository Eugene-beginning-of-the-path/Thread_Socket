#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h> //for work with protocol TCP/IP 
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

//Send a message from the clietn to the server and vice versa

int start_socket()
{
	//Этап 1: Инициализация сокетных интерфейсов Win32API
	
	WSADATA wsData; //определяем с какой версией сокетов мы работаем 
	int errorStat = WSAStartup(MAKEWORD(2,2), &wsData); //функция запуска сокетов

	if (errorStat == 0)
		std::cout << "WinSocket initialization complite " << std::endl;
	else
	{
		std::cout << "Error of WinSocket initialization #" << WSAGetLastError();
		return 1;
	}

	//Подитог: через создание объекта WSADATA мы получили от ОС версию сокетов,
	//а через вызов функции WSAStartup(диапазон версии сокетов, которые мы хотим 
	//использовать; указатель на структуру wsData) запустили программный интерфейс 
	//сокетов, для работы с ними
	
	//Этап 2: Создание сокета и его инициализация

	//Инициализируем сокет через функцию socket(), передаем аргументы:
	//AF_INET - работаем с семейством адресов IPv4
	//Тип сокета - работа с транспортым протоколом TCP - SOCK_STREAM
	//Тип протокола - необязателен, для TCP передаем нуль
	SOCKET ServSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSocket == INVALID_SOCKET)
	{
		std::cout << "Error of initializated socket #" << WSAGetLastError();
		return 1;
	}
	else
		std::cout << "Socket initializated complite" << std::endl;

	//Подитог: инициализировали сокет ServSocket через функцию socket, в которую
	//передали аргументы, уточняющие работу сокета, что он будет работать с 
	//адресами IPv4, TCP

	//Этап 3: Привязка сокета к паре IP-адрес/Порт

	sockaddr_in servInfo; //в него указывает адрес и порт для сокета
	//ZeroMemory(&servInfo, sizeof(servInfo));
	servInfo.sin_family = AF_INET; //вид адреса, к которому будет прикреплен сокет - IPv4
	in_addr ip_to_num; //для перевода адреса IPv4 в байт код для bind()
	//ниже переводим строку из привычного IPv4 формата в байт код
	//127.0.0.1 - локальный адрес устройства 
	errorStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);
	if (errorStat <= 0)
	{
		std::cout << "Error in IP translation to byte code";
		return 1;
	}
	servInfo.sin_addr = ip_to_num; //привели адрес IP, в нужном для bind() виде
	servInfo.sin_port = htons(1234); //номер порта, к которому крепим сокет


	errorStat = bind(ServSocket, (sockaddr*)&servInfo, sizeof(servInfo));
	if (errorStat != 0)
	{
		std::cout << "Error socket binding to server info. Error # " << WSAGetLastError();
		closesocket(ServSocket);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "Binding socket to server info complite" << std::endl;

	//Подитог: для привязки IP и порта к сокету, используем bind(), для которого
	//вторым параметром нужно передать структуру sockaddr_in и привести ее к sockaddr,
	//в которой хранится инфа о IP и номере порта, к которым мы крепим наш сокет

	//Этап 4 (для сервера): "Прослушивание» привязанного порта для идентификации подключений"

	errorStat = listen(ServSocket, SOMAXCONN);
	if (errorStat != 0)
	{
		std::cout << "Can't start to listen. Error #" << WSAGetLastError();
		closesocket(ServSocket);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "Socket is listeting..." << std::endl;		

	//Подитог: мы настроили сервер на прослушивание через определенный сокет и 
	//указали кол-во макс. процессов подключений - выбрали максимальное значение

	//Этап 5 (только для Сервера). Подтверждение подключения

	sockaddr_in clientInfo;

	ZeroMemory(&clientInfo, sizeof(clientInfo));

	int clientInfo_size = sizeof(clientInfo);

	/*Если подключение подтверждено, то вся информация по текущему соединению передаётся на 
	новый сокет, который будет отвечать со стороны Сервера за конкретное соединение с конкретным Клиентом*/

	SOCKET ClientConn = accept(ServSocket, (sockaddr*)&clientInfo, &clientInfo_size);

	if (ClientConn == INVALID_SOCKET) {
		std::cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << std::endl;
		closesocket(ServSocket);
		closesocket(ClientConn);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "Connection to a client established successfully" << std::endl;


}

//https://habr.com/ru/post/582370/