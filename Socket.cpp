#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h> //for work with protocol TCP/IP 
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

//Send a message from the clietn to the server and vice versa

int stage1(WSADATA& wsData);
int stage2(SOCKET& ServSocket);
int stage3(SOCKET& ServSocket, sockaddr_in& servInfo);
int stage4_forServer(SOCKET& ServSocket);

void start_socket()
{
	//Этап 1: Инициализация сокетных интерфейсов Win32API
	
	WSADATA wsData; //определяем с какой версией сокетов мы работаем 
	stage1(wsData);

	/*Подитог: через создание объекта WSADATA мы получили от ОС версию сокетов,
	а через вызов функции WSAStartup(диапазон версии сокетов, которые мы хотим 
	использовать; указатель на структуру wsData) запустили программный интерфейс 
	сокетов, для работы с ними*/
	
	//Этап 2: Создание сокета и его инициализация

	SOCKET ServSocket;
	stage2(ServSocket);

	/*Подитог: инициализировали сокет ServSocket через функцию socket, в которую
	передали аргументы, уточняющие работу сокета, что он будет работать с 
	адресами IPv4, TCP*/

	//Этап 3: Привязка сокета к паре IP-адрес/Порт

	sockaddr_in servInfo; //в него указывает адрес и порт для сокета
	stage3(ServSocket, servInfo);

	//Подитог: для привязки IP и порта к сокету, используем bind(), для которого
	//вторым параметром нужно передать структуру sockaddr_in и привести ее к sockaddr,
	//в которой хранится инфа о IP и номере порта, к которым мы крепим наш сокет

	//Этап 4 (для сервера): "Прослушивание» привязанного порта для идентификации подключений"

	stage4_forServer(ServSocket);

	//Подитог: мы настроили сервер на прослушивание через определенный сокет и 
	//указали кол-во макс. процессов подключений - выбрали максимальное значение

	//Этап 4 (для Клиента). Организация подключения к серверу



}

int stage1(WSADATA& wsData)
{
	int errorStat = WSAStartup(MAKEWORD(2, 2), &wsData); //функция запуска сокетов

	if (errorStat == 0)
		std::cout << "WinSocket initialization complite " << std::endl;
	else
	{
		std::cout << "Error of WinSocket initialization #" << WSAGetLastError();
		return 1;
	}
}

int stage2(SOCKET& ServSocket)
{
	//Инициализируем сокет через функцию socket(), передаем аргументы:
	//AF_INET - работаем с семейством адресов IPv4
	//Тип сокета - работа с транспортым протоколом TCP - SOCK_STREAM
	//Тип протокола - необязателен, для TCP передаем нуль
	ServSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSocket == INVALID_SOCKET)
	{
		std::cout << "Error of initializated socket #" << WSAGetLastError();
		return 1;
	}
	else
		std::cout << "Socket initializated complite" << std::endl;
}

int stage3(SOCKET& ServSocket, sockaddr_in& servInfo)
{
	//ZeroMemory(&servInfo, sizeof(servInfo));
	servInfo.sin_family = AF_INET; //вид адреса, к которому будет прикреплен сокет - IPv4
	in_addr ip_to_num; //для перевода адреса IPv4 в байт код для bind()
	//ниже переводим строку из привычного IPv4 формата в байт код
	//127.0.0.1 - локальный адрес устройства 
	int errorStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);
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
}

int stage4_forServer(SOCKET& ServSocket)
{
	int errorStat = listen(ServSocket, SOMAXCONN);
	if (errorStat != 0)
	{
		std::cout << "Can't start to listen. Error #" << WSAGetLastError();
		closesocket(ServSocket);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "Socket is listeting..." << std::endl;
}

//https://habr.com/ru/post/582370/