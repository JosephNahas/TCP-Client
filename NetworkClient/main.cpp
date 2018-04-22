#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main() 
{
	string ipAddress = ""; // ip Address of the server
	int port = 54000; // listening port number on the server

	// initialize winsock
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);

	if (wsResult != 0)
	{
		cerr << "Can't startup winsock! Quitting" << endl;
		return;
	}
	
	// create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket! Quitting" << endl;
		WSACleanup();
		return;
	}

	// fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server! Quitting" << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	// do-while loop to send and receive data
	char buf[4096];
	string userInput;

	bool signedIn = false;

	do
	{
		if (!signedIn) {
			ZeroMemory(buf, 4096);
			int bytesReceived = recv(sock, buf, 4096, 0);
			if (bytesReceived > 0)
			{
				// echo response to console
				cout << string(buf, 0, bytesReceived) << endl;
			}

			// prompt the user for gamertag
			cout << ">Please enter your gamertag \n";
			signedIn = true;
		}

		getline(cin, userInput);

		if (userInput.size() > 0) // make sure the user has typed something
		{
			// send the input
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				// wait for response
				ZeroMemory(buf, 4096);
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0)
				{
					// echo response to console
					cout << "The Server received: " << string(buf, 0, bytesReceived) << endl;
				}

			}

		}

	} while (userInput.size() > 0);

	// cleanup winsock
	closesocket(sock);
	WSACleanup();
}