#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <string>

using namespace std;

namespace tcp4 {

	class client {

	private:

		SOCKADDR_IN addrin;

	public:

		SOCKET sock;

		int recvtimeout = 5000; //recvtimeout in ms
		int sendtimeout = 5000; //sendtimeout in ms

		~client();
		int create_connection(const char *ip, int port, int timeout);
		string recv_buffer(int size);
		int send_buffer(string sendbuffer, int size);

	};

	class server {

	private:

		SOCKET server_sock;
		int addrlen = sizeof(addrin);
	
	public:

		SOCKET sock;
		SOCKADDR_IN addrin;

		server(SOCKET server_sock);
		~server();
		void accept_connection();
		string recv_buffer(int size);
		int send_buffer(string sendbuffer, int size);

	};

	int _send_buffer(SOCKET sock, string sendbuffer, int size);
	string _recv_buffer(SOCKET sock, int size);
	SOCKET create_bind(int port);
	string dns_resolver(const char *ip);

}

