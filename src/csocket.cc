#include <cstdio> //perror

#include <iostream> // cout

#include <sys/socket.h> //socket
#include <arpa/inet.h> // struct sockaddr_in & inet_ntoa & ntohs
#include <unistd.h> // close

#include "csocket.h"

using namespace std;

CSocket::CSocket()
{
	Master=0;
#ifdef _USE_VSOCK_
	cout<<"Type : AF_VSOCK"<<endl;
	Type=AF_VSOCK;
#else
	cout<<"Type : AF_INET"<<endl;
	Type=AF_INET;
#endif
}

CSocket::CSocket(TypeSocket type)
{
	Master=0;
	Type=type;
}

CSocket::~CSocket()
{
	Close();
}

bool CSocket::Configure()
{
	//create a master socket

	Master = socket(Type , SOCK_STREAM , 0);

	if( Master == SOCKET_ERROR )
	{
		perror("socket : failed");
		return false;
	}

	return true;
}

ssize_t CSocket::Send(Descriptor descriptor, const char* data, ssize_t sizeOfData)
{
	return send(descriptor, data, sizeOfData, 0);
}

ssize_t CSocket::Read(Descriptor descriptor, char* data, ssize_t sizeOfData)
{
	return recv(descriptor , data, sizeOfData, 0);
}

void CSocket::ShowInfo(Descriptor descriptor)
{
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	getpeername(descriptor , (struct sockaddr*)&address , (socklen_t*)&addrlen);
	cout<<"ip "<<inet_ntoa(address.sin_addr)<<" , port "<<ntohs(address.sin_port);
}

CSocket::operator int()
{
	return Master;
}

void CSocket::Close()
{
	close(Master);
}


