  #include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

using namespace std;


int main(void)
{
	system("chcp 1251");
	const char SERVER_IP[] = "127.0.0.1";					
	const short SERVER_PORT_NUM = 5555;				
	const short BUFF_SIZE = 1024;				

	int erStat;										


	in_addr ip_to_num;
	inet_pton(AF_INET, SERVER_IP, &ip_to_num);

	WSADATA wsData;
	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erStat != 0) {
		cout << "Ошибка: Неудачная инициализация версии WinSock : ";
		cout << WSAGetLastError();
		return 1;
	}
	else
		cout << "Библиотеки WinSock инициализированны успешно" << endl;


	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ClientSock == INVALID_SOCKET) {
		cout << "Ошибка: Неуспешное создание сокета : " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
	}
	else
		cout << "Сокет успешно создан" << endl;


	sockaddr_in servInfo;

	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(SERVER_PORT_NUM);
	
	erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		cout << "Ошибка : неуспешное соединение с сервером : " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else {
		char serverIP[22];

		inet_ntop(AF_INET, &servInfo.sin_addr, serverIP, INET_ADDRSTRLEN);
	
		cout << "Соединение с сервером по IP = " << serverIP << " порт = " << SERVER_PORT_NUM << " создано успешно" << endl;
	}
		



	vector <char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);							
	short packet_size = 0;												

	while (true) {

		cout << "Ваше сообщение: ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);

		if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
			shutdown(ClientSock, SD_BOTH);
			closesocket(ClientSock);
			WSACleanup();
			return 0;
		}

		packet_size = send(ClientSock, clientBuff.data(), clientBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			cout << "Ошибка : Не удалось отпрвить сообщение на сервер : " << WSAGetLastError() << endl;
			closesocket(ClientSock);
			WSACleanup();
			return 1;
		}

		packet_size = recv(ClientSock, servBuff.data(), servBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			cout << "Ошибка : Не удалось принять сообщение от сервера : # " << WSAGetLastError() << endl;
			closesocket(ClientSock);
			WSACleanup();
			return 1;
		}
		else
			cout << "Сообщение сервера: " << servBuff.data() << endl;

	}

	closesocket(ClientSock);
	WSACleanup();

	return 0;
}