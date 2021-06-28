#include "net.h"
#include <Windows.h>
#include <iostream>

using namespace std;

int main() {

	WSADATA data;
	WSAStartup(MAKEWORD(2, 1), &data);

	string msg;

	//ipv4 examples 

	cout << "Google IVP4: " << tcp4::dns_resolver("google.com") << endl; //get ivp4

	tcp4::client *client_test = new tcp4::client();

	client_test->recvtimeout = 10000; // 10s. change max timeout on recv and send buffer
	client_test->sendtimeout = 10000;											

	if (client_test->create_connection("google.com", 80, 5) != SOCKET_ERROR) //connect to google.com at port 80 and 5s max timeout
	{
		string buffer = "GET / HTTP/1.1\r\nHost: google.com\r\n\r\n";
		client_test->send_buffer(buffer, buffer.size()); //send buffer
		msg = client_test->recv_buffer(4096); //recv buffer with max size 4096
		cout << "\nResponse: " << msg << endl;
	}

	client_test->~client(); //close socket and delete client


	SOCKET bind = tcp4::create_bind(80); // create bind at port 80

	cout << "Waiting connection at port 80..." << endl;

	tcp4::server *server_test = new tcp4::server(bind); //create server

	server_test->accept_connection(); //recv a connection 

	server_test->send_buffer("Hello", 5);

	server_test->~server(); //close connection and delete object 
	closesocket(bind); //close bind socket



	WSACleanup();
}