#include "net.h"

namespace tcp4 {

	client::~client()
	{
		closesocket(sock);
	}

	int client::create_connection(const char *ip, int port, int timeout)
	{
		u_long imode;
		struct timeval tv;
		fd_set fdset;

		closesocket(sock);
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

		inet_pton(AF_INET, dns_resolver(ip).c_str(), &(addrin.sin_addr));
		addrin.sin_family = AF_INET;
		addrin.sin_port = htons(port);

		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&recvtimeout, sizeof(int)); //timeout recv
		setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&sendtimeout, sizeof(int)); //timeout send

		ioctlsocket(sock, FIONBIO, &imode);

		connect(sock, (SOCKADDR *)&addrin, sizeof(addrin));
		FD_ZERO(&fdset);
		FD_SET(sock, &fdset);
		tv.tv_sec = timeout;  // seconds timeout 
		tv.tv_usec = 0;

		if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1) {
			int so_error;
			socklen_t len = sizeof(so_error);
			getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&so_error, &len);
		}
		else {
			return SOCKET_ERROR;
		}
		imode = 0;
		ioctlsocket(sock, FIONBIO, &imode); //set as blocking

		return sock;
	}

	string client::recv_buffer(int size)
	{
		return _recv_buffer(sock, size);
	}

	int client::send_buffer(string sendbuffer, int size)
	{
		return _send_buffer(sock, sendbuffer, size);
	}

	server::server(SOCKET server_sock) {
		this->server_sock = server_sock;
	}

	server::~server()
	{
		closesocket(sock);
	}

	void server::accept_connection()
	{
		closesocket(sock);
		addrlen = sizeof(addrin);
		sock = accept(server_sock, (SOCKADDR *)&addrin, &addrlen);
	}

	string server::recv_buffer(int size)
	{
		return _recv_buffer(sock, size);
	}

	int server::send_buffer(string sendbuffer, int size)
	{
		return _send_buffer(sock, sendbuffer, size);
	}
	//functions																			/////

	int _send_buffer(SOCKET sock, string sendbuffer, int size)
	{
		return send(sock, sendbuffer.c_str(), size, 0);
	}

	string _recv_buffer(SOCKET sock, int size)
	{
		char *buffer = new char[size]();
		string r;

		int rs = recv(sock, (char *)buffer, size, 0);
		r.append(buffer, 0, rs);

		delete[] buffer;
		return r;
	}

	SOCKET create_bind(int port)
	{
		SOCKADDR_IN addrin;
		SOCKET sock;

		int timeout_send_recv = 5000;

		addrin.sin_addr.s_addr = INADDR_ANY;
		addrin.sin_port = htons(port);
		addrin.sin_family = AF_INET;

		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout_send_recv, sizeof(int)); //
		setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout_send_recv, sizeof(int)); //                  ^

		if (bind(sock, (SOCKADDR *)&addrin, sizeof(addrin)) == 0) {
			if (listen(sock, 3) == 0)
				return sock;
		}
		closesocket(sock);
		return SOCKET_ERROR;
	}

	string dns_resolver(const char* ip) 
	{
		char addrstr[100];
		ADDRINFO phints, *presult;

		memset(&phints, 0, sizeof(phints));
		phints.ai_family = PF_INET;
		phints.ai_socktype = SOCK_STREAM;

		if (getaddrinfo(ip, NULL, &phints, &presult) != 0) {
			return (char *)"DNS_ERROR";
		}

		inet_ntop(presult->ai_family, &((struct sockaddr_in *) presult->ai_addr)->sin_addr, addrstr, 100);

		freeaddrinfo(presult);
		return addrstr;
	}
}
